# Windows API Knowledge Map v1.0

## 1. 定位

Windows API Knowledge Map 不是 API 手册。

目标：

建立：

程序行为 → Windows能力 → DLL模块 → 核心API → 实际场景 → 分析方法

的映射关系。

服务于：

-   Windows 用户态程序观察
-   EXE 行为分析
-   x64dbg 动态调试
-   异常分析
-   DLL / Hook 后续学习

# 2. 总体分类树

    00 Foundation
    01 Process
    02 Thread
    03 Memory
    04 File System
    05 DLL / Module
    06 Window / GUI
    07 Input / Automation
    08 Network
    09 Registry / System
    10 Security
    11 Exception / Debug
    12 PE Format
    13 Debug API
    14 Hook / Injection
    15 Case Study

# 00 Foundation - Windows程序生命周期

目标：

理解 EXE 从启动到退出发生什么。

关注：

-   Windows Loader
-   EntryPoint
-   CRT初始化
-   main / WinMain
-   Process Exit

模块：

-   ntdll.dll
-   kernel32.dll

# 01 Process - 进程模型

目标：

理解 Windows 如何管理运行中的程序。

核心：

-   Process对象
-   Handle
-   父子进程

API：

-   CreateProcess
-   OpenProcess
-   TerminateProcess

# 02 Thread - 线程模型

目标：

理解程序如何执行任务。

核心：

-   Thread生命周期
-   同步
-   等待机制

API：

-   CreateThread
-   WaitForSingleObject
-   Sleep

# 03 Memory - 内存管理

目标：

理解程序数据和代码如何存在。

核心：

-   Stack
-   Heap
-   Virtual Memory
-   Memory Protection

API：

-   VirtualAlloc
-   VirtualProtect
-   VirtualFree
-   HeapAlloc

# 04 File System - 文件系统

目标：

理解程序如何访问文件。

API：

-   CreateFile
-   ReadFile
-   WriteFile

场景：

-   配置文件
-   日志
-   数据文件

# 05 DLL / Module - 模块系统

目标：

理解程序如何加载代码模块。

核心：

-   DLL加载
-   Import
-   Export

API：

-   LoadLibrary
-   GetProcAddress
-   FreeLibrary

# 06 Window / GUI - 窗口系统

模块：

-   user32.dll
-   gdi32.dll

API：

-   CreateWindowEx
-   GetMessage
-   DispatchMessage
-   SendMessage

# 07 Input / Automation - 输入与自动化

目标：

理解程序如何接收和发送操作。

API：

-   SendMessage
-   PostMessage
-   FindWindow
-   EnumWindows

# 08 Network - 网络通信

模块：

-   ws2_32.dll
-   winhttp.dll
-   wininet.dll

API：

-   socket
-   connect
-   send
-   recv

# 09 Registry / System - 系统交互

模块：

-   advapi32.dll

API：

-   RegOpenKey
-   RegQueryValue
-   RegSetValue

# 10 Security - 安全模型

核心：

-   Token
-   SID
-   Privilege
-   UAC

API：

-   OpenProcessToken
-   GetTokenInformation

# 11 Exception / Debug - 异常与调试

目标：

理解程序崩溃。

重点：

ExceptionCode:

c0000005 Access Violation

分类：

-   Read violation
-   Write violation
-   Execute violation

# 12 PE Format - 可执行文件结构

内容：

-   DOS Header
-   PE Header
-   Section
-   Import
-   Export
-   Resource

# 13 Debug API - 调试能力

内容：

-   Debug Object
-   Breakpoint
-   Memory Access
-   Process Debugging

# 14 Hook / Injection - 程序扩展

目标：

理解如何扩展已有程序。

内容：

-   DLL Injection
-   API Hook
-   IAT Hook
-   Inline Hook

最终目标：

目标程序 → Injected DLL → 内部数据 → HTTP API

# 15 Case Study - 综合案例

案例：

-   普通EXE分析
-   GUI程序分析
-   网络程序分析
-   第三方客户端分析
-   Agent构建

# 使用方式

每个分类后续扩展：

Knowledge Map \| +-- Demo \| +-- x64dbg分析记录 \| +-- 实际案例

目标：

不是记住 API。

而是：

看到程序行为，知道可能涉及哪些 Windows 能力，并知道如何验证。
