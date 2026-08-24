# NET-001-version-check

## 实验目标

模拟一个 Windows Native 应用在启动时检查最新版本。

程序流程：

```text
启动
  ↓
读取 config.ini
  ↓
读取 update.enabled
  ↓
读取 update_url
  ↓
enabled=false
  ↓
跳过版本检查

enabled=true
  ↓
WinHTTP 发起 HTTP GET
  ↓
读取响应内容
  ↓
获取 latest version
  ↓
与 CURRENT_VERSION 比较
  ↓
打印版本检查结果
  ↓
Application is ready
  ↓
Sleep(2000)
  ↓
Application is shutting down
  ↓
退出
```

本实验重点观察 Windows WinHTTP API，以及在无符号环境下如何通过字符串引用、API 断点和网络响应 Buffer 定位网络逻辑。

---

## 场景说明

程序继续使用前面 Demo 中的：

```text
config.ini
```

配置：

```ini
[application]
app_id=windows-native-analysis-lab

[update]
enabled=true
update_url=http://127.0.0.1:8080/version
```

程序自身版本硬编码为：

```text
1.0.0
```

本地版本服务返回：

```text
1.1.0
```

因此正常运行时应观察到：

```text
Current version: 1.0.0
Latest version: 1.1.0
Update available
```

本实验使用本地 HTTP 服务，避免 DNS、TLS、代理和公网状态影响实验结果。

---

## 为什么使用 WinHTTP

本实验目标是观察一个真实 HTTP 版本检查流程，因此使用 Windows 提供的：

```text
WinHTTP
```

而不是直接使用 Winsock。

主要 API：

```text
WinHttpOpen
WinHttpConnect
WinHttpOpenRequest
WinHttpSendRequest
WinHttpReceiveResponse
WinHttpReadData
WinHttpCloseHandle
```

后续可以再单独使用 Winsock 做：

```text
socket
connect
send
recv
```

用于观察更底层的 TCP 通信。

---

## 本地版本服务

使用 Python 启动一个极简 HTTP 服务：

```text
http://127.0.0.1:8080/version
```

响应：

```text
1.1.0
```

启动：

```text
python version_server.py
```

然后再运行：

```text
NET-001-version-check.exe
```

---

## 主要 Windows API

本实验主要涉及：

```text
GetPrivateProfileStringA

WinHttpOpen
WinHttpConnect
WinHttpOpenRequest
WinHttpSendRequest
WinHttpReceiveResponse
WinHttpReadData
WinHttpCloseHandle

Sleep
```

其中：

- `GetPrivateProfileStringA`：读取 `update.enabled` 和 `update_url`
- `WinHttpOpen`：创建 WinHTTP Session
- `WinHttpConnect`：建立到目标主机的逻辑连接
- `WinHttpOpenRequest`：创建 HTTP Request
- `WinHttpSendRequest`：发送 HTTP 请求
- `WinHttpReceiveResponse`：等待 HTTP 响应
- `WinHttpReadData`：读取响应 Body
- `WinHttpCloseHandle`：释放 WinHTTP Handle
- `Sleep`：让程序在 ready 状态停留约 2 秒

---

## x64dbg 分析思路

正式分析时不依赖源码函数名或调试符号。

推荐从这些字符串开始：

```text
127.0.0.1
/version
update_url
Current version
Latest version
Update available
Application is ready
```

分析路径：

```text
String Reference
        ↓
Code Reference
        ↓
WinHTTP API
        ↓
Arguments Observation
        ↓
Network Response Buffer
```

---

## WinHttpConnect

重点观察：

```text
WinHttpConnect
```

参数：

```text
RCX = Session Handle
RDX = Server Name
R8  = Port
R9  = Reserved
```

本实验预期：

```text
RDX → "127.0.0.1"
R8  = 8080
```

---

## WinHttpOpenRequest

本实验请求：

```text
GET /version
```

重点观察：

- HTTP method
- object path
- flags

应能在参数中看到：

```text
GET
/version
```

---

## WinHttpSendRequest

调用：

```text
WinHttpSendRequest
```

表示程序正式发送 HTTP Request。

本实验没有 Request Body。

---

## WinHttpReceiveResponse

调用：

```text
WinHttpReceiveResponse
```

表示程序开始等待服务器 HTTP 响应。

---

## WinHttpReadData

这是本实验最重要的观察点之一。

调用前重点记录：

```text
lpBuffer
```

调用后重新查看同一块内存。

正常情况下应看到：

```text
1.1.0
```

因此形成：

```text
HTTP Server
    ↓
WinHttpReadData
    ↓
Process Buffer
    ↓
"1.1.0"
```

这与 FILE-001 中：

```text
File
    ↓
ReadFile
    ↓
Process Buffer
```

属于非常相似的分析模式。

---

## 版本比较

程序当前版本：

```text
1.0.0
```

服务返回：

```text
1.1.0
```

当前 Demo 只做简单字符串比较：

```text
相同
→ Already up to date

不同
→ Update available
```

本实验不实现完整语义版本比较，也不执行实际更新。

---

## 失败场景

如果 Python 版本服务没有启动：

```text
WinHttpConnect / WinHttpSendRequest / WinHttpReceiveResponse
        ↓
失败
        ↓
打印版本检查失败
```

这个场景也可以作为 x64dbg 的第二条观察路径。

---

## 实验结果预期

正常运行：

```text
NET-001-version-check

update.enabled=true
update_url=http://127.0.0.1:8080/version

Current version: 1.0.0
Latest version: 1.1.0
Update available

Application is ready.
Application is shutting down.
```

本实验用于建立对 Windows HTTP 客户端行为的基本观察能力，并为后续 Winsock、THREAD 和 PROC Demo 提供真实网络场景。
