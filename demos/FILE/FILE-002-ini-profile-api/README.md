# FILE-002-ini-profile-api

## 实验目标

模拟一个传统 Windows Native 应用使用 INI Profile API 管理配置文件的启动流程：

1. 启动程序
2. 检查 `config.ini` 是否存在
3. 若配置文件不存在，则写入一组默认配置
4. 使用 Windows INI Profile API 读取配置
5. 在控制台打印配置摘要
6. 输出 `Application is ready`
7. 等待 2 秒
8. 获取当前时间
9. 在退出前写入 `[runtime] last_close_time`
10. 输出 `Application is shutting down`
11. 正常退出

本实验重点观察 Windows 提供的 INI Profile API，而不是直接使用 `CreateFileA / ReadFile / WriteFile` 自己解析和修改配置文件。

与 FILE-001 相比，本实验关注的是：

```text
Application Logic
        ↓
INI Profile API
        ↓
Windows
        ↓
config.ini
```

---

## 场景说明

程序使用：

```text
config.ini
```

保存应用配置。

配置内容：

```ini
[application]
app_id=windows-native-analysis-lab
debug=true

[update]
enabled=true
update_url=http://127.0.0.1:8080/version

[log]
log_dir=logs

[runtime]
last_close_time=
```

其中：

- `app_id`：应用标识
- `debug`：调试开关
- `enabled`：是否启用更新功能
- `update_url`：后续 NET Demo 使用的版本检查地址
- `log_dir`：日志目录
- `last_close_time`：程序上一次正常退出时间

当前实验不会执行网络请求，也不会真正初始化日志系统。

---

## 配置文件不存在

`config.ini` 不是必须预先存在。

程序启动时首先检查：

```text
config.ini
```

如果不存在：

```text
config.ini missing
        ↓
Use built-in defaults
        ↓
WritePrivateProfileStringA
        ↓
Create config.ini
        ↓
Write default values
```

默认配置由程序内部硬编码。

创建完成后，再进入正常读取流程。

因此 FILE-002 可以观察两种运行路径：

### 第一次运行

```text
config.ini 不存在
        ↓
创建默认配置
        ↓
读取配置
        ↓
运行
        ↓
写入 last_close_time
```

### 后续运行

```text
config.ini 已存在
        ↓
读取已有配置
        ↓
运行
        ↓
更新 last_close_time
```

---

## 配置读取

本实验不再手工：

```text
CreateFileA
    ↓
ReadFile
    ↓
搜索 key=value
    ↓
解析 INI
```

而是直接使用：

```text
GetPrivateProfileStringA
```

例如读取：

```ini
[application]
app_id=windows-native-analysis-lab
```

逻辑上对应：

```text
section = application
key     = app_id
default = unknown
file    = config.ini
```

读取后的配置会打印到控制台：

```text
app_id=windows-native-analysis-lab
debug=true
update.enabled=true
update_url=http://127.0.0.1:8080/version
log_dir=logs
```

---

## 配置写入

程序退出前获取当前本地时间，并写入：

```ini
[runtime]
last_close_time=2026-08-24 13:45:12.417
```

时间格式：

```text
yyyy-MM-dd HH:mm:ss.SSS
```

写入通过：

```text
WritePrivateProfileStringA
```

完成。

该 API 的四个参数非常适合在 Windows x64 下直接观察：

```text
RCX = section
RDX = key
R8  = value
R9  = ini file
```

例如：

```text
RCX = "runtime"
RDX = "last_close_time"
R8  = "2026-08-24 13:45:12.417"
R9  = ".\config.ini"
```

---

## 主要 Windows API

本实验主要涉及：

```text
GetFileAttributesA
GetPrivateProfileStringA
WritePrivateProfileStringA
GetLocalTime
Sleep
```

其中：

- `GetFileAttributesA`：判断 `config.ini` 是否存在
- `GetPrivateProfileStringA`：读取 INI 配置项
- `WritePrivateProfileStringA`：创建配置文件、写入默认配置、更新运行状态
- `GetLocalTime`：生成毫秒级 `last_close_time`
- `Sleep`：让程序在 ready 状态停留约 2 秒

本实验不直接调用：

```text
CreateFileA
ReadFile
WriteFile
```

INI 文件的底层文件访问由 Windows Profile API 封装。

---

## x64dbg 分析思路

正式分析时仍然不依赖源码函数名或调试符号。

推荐从这些字符串开始：

```text
config.ini
application
app_id
update
update_url
runtime
last_close_time
Application is ready
Application is shutting down
```

分析路径：

```text
String Reference
        ↓
Code Reference
        ↓
GetPrivateProfileStringA
或
WritePrivateProfileStringA
        ↓
Arguments Observation
        ↓
config.ini Result
```

---

## 配置读取链

例如从：

```text
"app_id"
```

开始：

```text
"app_id"
    ↓
String Reference
    ↓
GetPrivateProfileStringA
    ↓
观察 section / key / default / buffer
    ↓
配置值进入 Buffer
```

重点观察：

```text
GetPrivateProfileStringA
```

调用前的参数，以及调用后输出 Buffer 中的内容。

---

## 配置创建链

删除 `config.ini` 后重新运行程序。

观察：

```text
GetFileAttributesA
        ↓
INVALID_FILE_ATTRIBUTES
        ↓
WritePrivateProfileStringA
        ↓
创建 config.ini
```

然后确认磁盘上出现：

```text
config.ini
```

并包含默认配置。

---

## last_close_time 写入链

可以从：

```text
"last_close_time"
```

搜索字符串引用。

随后定位：

```text
WritePrivateProfileStringA
```

调用前观察：

```text
RCX = "runtime"
RDX = "last_close_time"
R8  = timestamp
R9  = ".\config.ini"
```

继续执行后检查：

```ini
[runtime]
last_close_time=...
```

是否已经写入文件。

这条链非常适合练习：

```text
String Reference
        ↓
API Breakpoint
        ↓
Register Arguments
        ↓
Filesystem Side Effect
```

---

## 与 FILE-001 的区别

FILE-001 直接操作普通文件：

```text
CreateFileA
    ↓
ReadFile / WriteFile
    ↓
CloseHandle
```

FILE-002 使用更高层的 Windows INI 抽象：

```text
GetPrivateProfileStringA
WritePrivateProfileStringA
```

因此：

```text
FILE-001
应用自己负责文件读写和解析

FILE-002
应用调用 Windows Profile API，
由 Windows 封装 INI 文件操作
```

这也是分析第三方 Windows 程序时的重要区别。

如果程序明显读取或修改 `.ini` 文件，但业务代码附近没有直接看到：

```text
ReadFile
WriteFile
```

应该考虑它是否使用：

```text
GetPrivateProfileStringA/W
WritePrivateProfileStringA/W
```

---

## 重点观察

### GetFileAttributesA

观察：

```text
RCX = ".\config.ini"
```

并比较：

```text
文件存在
文件不存在
```

两种情况下的返回值和控制流。

### GetPrivateProfileStringA

重点观察：

- section
- key
- default value
- output buffer
- buffer size
- ini path

调用后重点查看：

```text
output buffer
```

是否已经得到配置值。

### WritePrivateProfileStringA

重点观察：

```text
RCX
RDX
R8
R9
```

尤其是写入 `last_close_time` 时：

```text
runtime
last_close_time
timestamp
config.ini
```

四个值之间的对应关系。

### Sleep

观察：

```text
Sleep(2000)
```

调用时：

```text
RCX = 2000
```

`Sleep` 仍然只是辅助观察点。

---

## 实验结果预期

### config.ini 不存在

程序第一次运行后：

1. 创建 `config.ini`
2. 写入默认配置
3. 读取并打印配置
4. 输出 `Application is ready`
5. 等待约 2 秒
6. 写入 `last_close_time`
7. 输出 `Application is shutting down`
8. 正常退出

### config.ini 已存在

再次运行：

1. 保留已有配置
2. 读取并打印配置
3. 等待约 2 秒
4. 更新 `last_close_time`
5. 正常退出

本实验用于建立对 Windows INI Profile API 的观察能力，并与 FILE-001 的原始文件读写方式形成对照。
