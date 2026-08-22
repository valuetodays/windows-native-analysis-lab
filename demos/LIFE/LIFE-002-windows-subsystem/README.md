# LIFE-002-windows-subsystem

## 实验目标

观察 Windows GUI Subsystem 程序的启动流程，并与 LIFE-001 的 Console Subsystem 程序进行对照。

重点验证：

- `-mwindows` 对 PE `Subsystem` 的影响
- `main` 与 `wWinMain` 的启动语义差异
- PE EntryPoint 与 `wWinMain` 的关系
- MinGW CRT Startup 在 GUI Subsystem 程序中的作用
- `MessageBoxW` 调用方式是否因 Subsystem 改变

## 程序形式

本实验使用：

```text
wWinMain
+
-mwindows
+
IMAGE_SUBSYSTEM_WINDOWS_GUI
+
MessageBoxW
```

与 LIFE-001 对照：

```text
LIFE-001
main
+
默认 Console Subsystem
+
IMAGE_SUBSYSTEM_WINDOWS_CUI
+
MessageBoxW
```

需要注意：

> 能够显示窗口，不代表程序属于 Windows GUI Subsystem。

LIFE-001 虽然调用了 `MessageBoxW`，但它仍是 Console Subsystem 程序。

本实验通过 `-mwindows` 构建真正的 Windows GUI Subsystem 程序。

## API 选择

继续使用 `MessageBoxW`，使 LIFE-001 与 LIFE-002 保持相同的可观察行为。

`MessageBoxW` 接收 UTF-16 字符串。

源码直接调用 `MessageBoxW`，因此不依赖 `UNICODE` 宏是否定义。

保持相同 API 的目的，是将观察重点放在：

```text
Subsystem
CRT Startup
main / wWinMain
```

而不是 API 行为差异。

## 验证内容

- PE `Subsystem`
- PE EntryPoint
- Loader 初始观察
- CRT Startup
- `wWinMain` 定位
- `MessageBoxW` 调用
- Windows x64 参数传递
- LIFE-001 / LIFE-002 差异

## 涉及知识

- PE Format
- Process Lifecycle
- PE Subsystem
- CRT Startup
- `main` / `wWinMain`
- Windows x64 Calling Convention
- Import Address Table

## 观察问题

实验过程中重点回答：

1. `-mwindows` 修改了 PE 中的什么信息？
2. LIFE-001 与 LIFE-002 的 `Subsystem` 是否不同？
3. PE EntryPoint 是否直接等于 `wWinMain`？
4. 从 EntryPoint 到 `wWinMain` 中间是否仍经过 CRT Startup？
5. 没有源码符号时，能否通过字符串引用定位 `wWinMain` 对应逻辑？
6. `MessageBoxW` 的参数传递方式是否与 LIFE-001 相同？
7. Console Subsystem 与 Windows GUI Subsystem 的核心区别是什么？

## x64dbg 观察点

- Loader / System Breakpoint
- PE EntryPoint
- PE `Subsystem`
- String Reference
- `wWinMain` 对应逻辑区域
- `MessageBoxW`
- RCX / RDX / R8 / R9

## 与 LIFE-001 的关系

LIFE-001 建立：

```text
Windows Loader
    ↓
PE EntryPoint
    ↓
MinGW CRT Startup
    ↓
main
    ↓
MessageBoxW
```

LIFE-002 将验证：

```text
Windows Loader
    ↓
PE EntryPoint
    ↓
MinGW CRT Startup
    ↓
wWinMain
    ↓
MessageBoxW
```

两个实验共同用于理解：

> PE Subsystem、程序入口语义和 Win32 API 调用是不同层次的概念。
