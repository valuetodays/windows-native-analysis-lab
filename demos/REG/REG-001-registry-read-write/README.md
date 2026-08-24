# REG-001-registry-read-write

## 实验目标

模拟一个 Windows Native 应用使用注册表保存运行状态。

程序使用：

```text
HKEY_CURRENT_USER\Software\TEST-WindowsNativeAnalysisLab
```

保存：

```text
install_id       REG_SZ
launch_count     REG_DWORD
last_start_time  REG_SZ
last_close_time  REG_SZ
```

程序流程：

```text
启动
  ↓
打开或创建注册表 Key
  ↓
读取 install_id
  ↓
不存在则写入默认 install_id
  ↓
读取 launch_count
  ↓
launch_count + 1
  ↓
写回 launch_count
  ↓
写入 last_start_time
  ↓
打印当前注册表状态
  ↓
Application is ready
  ↓
Sleep(2000)
  ↓
写入 last_close_time
  ↓
Application is shutting down
  ↓
关闭注册表 Key
  ↓
退出
```

本实验重点观察 Windows Registry API，以及在无符号环境下如何通过字符串引用、API 断点和注册表副作用定位程序逻辑。

---

## 注册表位置

实验只使用当前用户范围：

```text
HKEY_CURRENT_USER
└── Software
    └── TEST-WindowsNativeAnalysisLab
```

使用 `TEST-` 前缀，明确表示这是实验数据。

不需要管理员权限。

---

## 注册表值

### install_id

类型：

```text
REG_SZ
```

首次运行时如果不存在，则写入：

```text
WNL-REG-001
```

后续运行直接读取已有值。

### launch_count

类型：

```text
REG_DWORD
```

首次运行：

```text
launch_count = 1
```

后续每运行一次：

```text
launch_count = launch_count + 1
```

### last_start_time

类型：

```text
REG_SZ
```

程序启动时写入当前本地时间：

```text
yyyy-MM-dd HH:mm:ss.SSS
```

例如：

```text
2026-08-24 14:30:12.417
```

### last_close_time

类型：

```text
REG_SZ
```

程序退出前写入当前本地时间：

```text
yyyy-MM-dd HH:mm:ss.SSS
```

---

## 第一次运行

如果注册表 Key 不存在：

```text
RegCreateKeyExA
        ↓
创建
HKEY_CURRENT_USER\Software\TEST-WindowsNativeAnalysisLab
        ↓
install_id 不存在
        ↓
写入默认 install_id
        ↓
launch_count = 1
        ↓
写入 last_start_time
        ↓
Sleep(2000)
        ↓
写入 last_close_time
```

---

## 后续运行

如果注册表 Key 已存在：

```text
RegCreateKeyExA
        ↓
打开已有 Key
        ↓
RegQueryValueExA
        ↓
读取 install_id
        ↓
读取 launch_count
        ↓
launch_count + 1
        ↓
RegSetValueExA
        ↓
更新 launch_count
        ↓
更新 last_start_time
        ↓
Sleep(2000)
        ↓
更新 last_close_time
```

---

## 主要 Windows API

本实验主要涉及：

```text
RegCreateKeyExA
RegQueryValueExA
RegSetValueExA
RegCloseKey
GetLocalTime
Sleep
```

其中：

- `RegCreateKeyExA`：创建或打开注册表 Key
- `RegQueryValueExA`：读取注册表值
- `RegSetValueExA`：写入或更新注册表值
- `RegCloseKey`：关闭注册表 Key Handle
- `GetLocalTime`：生成毫秒级时间戳
- `Sleep`：让程序在 ready 状态停留约 2 秒

---

## x64dbg 分析思路

正式分析时不依赖源码函数名或调试符号。

推荐从这些字符串开始：

```text
Software\TEST-WindowsNativeAnalysisLab
install_id
launch_count
last_start_time
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
Registry API
        ↓
Arguments Observation
        ↓
Registry Side Effect
```

---

## 注册表 Key 创建链

从：

```text
"Software\\TEST-WindowsNativeAnalysisLab"
```

查找字符串引用。

定位到：

```text
RegCreateKeyExA
```

重点观察：

```text
RCX = HKEY_CURRENT_USER
RDX = "Software\\TEST-WindowsNativeAnalysisLab"
```

以及后续参数。

执行后观察返回的：

```text
HKEY
```

---

## install_id 读取链

从：

```text
"install_id"
```

查找字符串引用。

定位：

```text
RegQueryValueExA
```

如果值不存在，则进入：

```text
RegSetValueExA
```

写入：

```text
WNL-REG-001
```

---

## launch_count 更新链

读取：

```text
launch_count
```

类型：

```text
REG_DWORD
```

程序对读取值执行：

```text
+1
```

然后调用：

```text
RegSetValueExA
```

写回。

这个场景适合观察：

```text
RegQueryValueExA
        ↓
内存中的 DWORD
        ↓
加 1
        ↓
RegSetValueExA
```

---

## last_close_time 写入链

从：

```text
"last_close_time"
```

查找字符串引用。

程序在退出前调用：

```text
RegSetValueExA
```

写入：

```text
yyyy-MM-dd HH:mm:ss.SSS
```

随后可以使用 `regedit` 验证实际结果。

---

## 重点观察

### RegCreateKeyExA

重点观察：

- Root Key
- SubKey 路径
- Desired Access
- Result HKEY

### RegQueryValueExA

重点观察：

- HKEY
- Value Name
- Value Type
- Output Buffer
- Buffer Size

### RegSetValueExA

重点观察：

- HKEY
- Value Name
- Value Type
- Data Buffer
- Data Size

### RegCloseKey

观察注册表 Handle 的关闭。

### Sleep

观察：

```text
Sleep(2000)
```

调用时：

```text
RCX = 2000
```

`Sleep` 只是辅助观察点。

---

## 实验结果预期

第一次运行后，使用 `regedit` 可以看到：

```text
HKEY_CURRENT_USER
└── Software
    └── TEST-WindowsNativeAnalysisLab
        ├── install_id       REG_SZ      WNL-REG-001
        ├── launch_count     REG_DWORD   1
        ├── last_start_time  REG_SZ      ...
        └── last_close_time  REG_SZ      ...
```

再次运行后：

```text
launch_count
```

增加 1，同时：

```text
last_start_time
last_close_time
```

更新为本次运行时间。

本实验用于建立对 Windows Registry API 的基本观察能力，并为后续真实应用配置、持久化状态和启动行为分析打基础。
