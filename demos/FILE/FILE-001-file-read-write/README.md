# FILE-001-file-read-write

## 实验目标

模拟一个小型 Windows Native 应用的启动初始化流程：

1. 启动程序
2. 读取 `config.ini`
3. 解析应用、更新和日志配置
4. 在控制台打印配置摘要
5. 确保日志目录存在
6. 按日期创建或打开日志文件
7. 将启动状态和配置摘要追加到日志
8. 输出 `Application is ready`
9. 等待 2 秒
10. 输出 `Application is shutting down`
11. 正常退出

本实验重点观察 Windows 文件读写相关 API，以及在无符号环境下如何通过字符串引用、API 断点和运行行为定位文件访问逻辑。

---

## 场景说明

程序使用一个简单的 INI 配置文件：

```ini
[application]
app_id=windows-native-analysis-lab
debug=true

[update]
enabled=true
auto_update=false
check_at_startup=true
update_url=https://example.com/version

[log]
log_dir=./logs
```

当前实验只读取并打印这些配置。

`update_url`、`enabled`、`auto_update`、`check_at_startup` 暂时不会触发任何网络行为，它们将在后续 NET Demo 中继续使用。

---

## 日志行为

程序不引入第三方日志库，而是直接通过 Windows 文件 API 写入日志文件。

日志目录来自：

```ini
log_dir=./logs
```

日志文件按日期生成：

```text
logs/
└── 2026-08-22.log
```

日志采用追加方式写入，同一天多次启动程序不会覆盖已有内容。

日志格式：

```text
yyyy-MM-dd HH:mm:ss.SSS [LEVEL] [CATEGORY] message
```

示例：

```text
2026-08-22 12:00:01.104 [INFO] [Startup] Application starting
2026-08-22 12:00:01.109 [INFO] [Config] Configuration loaded from config.ini
2026-08-22 12:00:01.110 [INFO] [Config] app_id=windows-native-analysis-lab
2026-08-22 12:00:01.110 [INFO] [Config] debug=true
2026-08-22 12:00:01.111 [INFO] [Config] update.enabled=true
2026-08-22 12:00:01.111 [INFO] [Config] update.auto_update=false
2026-08-22 12:00:01.112 [INFO] [Config] update.check_at_startup=true
2026-08-22 12:00:01.112 [INFO] [Config] update.url=https://example.com/version
2026-08-22 12:00:01.113 [INFO] [Startup] Application is ready
2026-08-22 12:00:03.114 [INFO] [Startup] Application is shutting down
```

`Startup`、`Config`、`Update` 等 category 属于程序内部硬编码的日志分类，不来自配置文件。

---

## 配置文件缺失

如果 `config.ini` 不存在或无法读取：

```text
CreateFileW
    ↓
失败
    ↓
控制台输出错误
    ↓
返回非 0
    ↓
程序退出
```

这样可以同时观察 `CreateFileW` 成功与失败两种路径。

---

## 主要 Windows API

本实验主要涉及：

```text
CreateFileW
ReadFile
WriteFile
CloseHandle
CreateDirectoryW
GetLocalTime
Sleep
```

其中：

- `CreateFileW`：打开配置文件、创建或打开日志文件
- `ReadFile`：读取 `config.ini`
- `CreateDirectoryW`：确保日志目录存在
- `WriteFile`：追加日志内容
- `CloseHandle`：关闭文件 Handle
- `GetLocalTime`：生成日志文件日期和毫秒级时间戳
- `Sleep`：在 ready 与 shutdown 之间等待 2 秒

`Sleep` 只是附带观察点，本实验核心仍然是文件读取和写入。

---

## x64dbg 分析思路

正式分析时不依赖源码函数名或调试符号。

推荐从程序中的字符串开始：

```text
config.ini
log_dir
Application starting
Application is ready
Application is shutting down
```

分析路径：

```text
String Reference
        ↓
Code Reference
        ↓
Function / Logic Identification
        ↓
API Breakpoint
        ↓
Arguments / Buffer Observation
        ↓
Filesystem Result
```

### 配置读取链

```text
"config.ini"
    ↓
String Reference
    ↓
CreateFileW
    ↓
ReadFile
    ↓
配置内容进入内存
    ↓
解析 log_dir 等字段
```

### 日志写入链

```text
"Application starting"
    ↓
String Reference
    ↓
日志字符串构造
    ↓
CreateFileW
    ↓
WriteFile
    ↓
yyyy-MM-dd.log
```

---

## 重点观察

### CreateFileW

观察：

- `lpFileName`
- `dwDesiredAccess`
- `dwShareMode`
- `dwCreationDisposition`

由于 `CreateFileW` 超过 4 个参数，本实验也可以开始观察 Windows x64 调用约定中：

```text
RCX
RDX
R8
R9
```

以及后续栈参数。

### ReadFile

重点观察：

- 文件 Handle
- Buffer 地址
- 请求读取长度
- 调用前后的 Buffer 内容变化

### WriteFile

重点观察：

- 文件 Handle
- Buffer 地址
- 写入字节数
- Buffer 中实际日志内容

### Sleep

观察：

```text
Sleep(2000)
```

调用时：

```text
RCX = 2000
```

---

## 实验结果预期

程序正常运行后：

1. 控制台打印配置摘要
2. 自动创建 `logs` 目录（如果不存在）
3. 创建或追加当天日志文件
4. 打印并记录 `Application is ready`
5. 等待约 2 秒
6. 打印并记录 `Application is shutting down`
7. 正常退出

本实验为后续 NET、REG、THREAD、PROC Demo 提供基础应用场景。
