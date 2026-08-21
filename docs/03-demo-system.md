# Windows Native Lab Demo System v1.1

## 1. 定位

Demo System 是 Windows API Knowledge Map 的实验验证体系。

目标：

通过最小、可控、可重复的实验程序：

    Windows能力
        ↓
    Demo程序
        ↓
    x64dbg观察
        ↓
    分析记录
        ↓
    知识沉淀

建立 Windows 用户态程序观察、分析和实验能力。

------------------------------------------------------------------------

# 2. Demo体系设计原则

## 2.1 Demo不是课程章节

Demo 不对应 Knowledge Map 的章节编号。

例如：

Knowledge Map：

    Network

Demo：

    NET-001-socket-client
    NET-002-http-client

关系：

    Knowledge Map
            |
            |
            v

    Demo Lab
            |
            |
            v

    Analysis Report

------------------------------------------------------------------------

## 2.2 一个Demo验证一个核心能力

原则：

    一个知识点

    ↓

    一个最小实验

    ↓

    一次完整分析

避免：

-   一个程序包含太多功能
-   无法确定观察结果来源
-   难以复用

------------------------------------------------------------------------

# 3. Demo编号规则

格式：

    <领域缩写>-<序号>-<名称>

例如：

    PROC-001-create-process

    MEM-001-virtual-alloc

    NET-001-socket-client

    EXC-001-access-violation

------------------------------------------------------------------------

# 4. 领域编号定义

  领域                 编号     说明
  -------------------- -------- ----------------
  Lifecycle            LIFE     程序生命周期
  Process              PROC     进程
  Thread               THREAD   线程
  Memory               MEM      内存
  File System          FILE     文件
  DLL / Module         DLL      模块
  Window / GUI         GUI      窗口
  Input / Automation   INPUT    输入自动化
  Network              NET      网络
  Registry / System    REG      系统
  Security             SEC      安全
  Exception            EXC      异常
  Debug                DBG      调试
  PE Format            PE       可执行文件结构
  Hook / Injection     HOOK     程序扩展

------------------------------------------------------------------------

# 5. 第一批Demo规划

## LIFE - 程序生命周期

### LIFE-001-process-start

目标：

理解 EXE 启动流程。

观察：

-   EntryPoint
-   Module加载
-   main/WinMain

------------------------------------------------------------------------

# PROC - 进程

## PROC-001-create-process

目标：

理解 Windows 创建进程。

涉及：

-   CreateProcess
-   Process Handle

观察：

-   父子进程关系
-   Process生命周期

------------------------------------------------------------------------

# THREAD - 线程

## THREAD-001-create-thread

目标：

理解 Windows 创建线程。

涉及：

-   CreateThread
-   WaitForSingleObject

观察：

-   Thread入口
-   Thread ID

------------------------------------------------------------------------

# MEM - 内存

## MEM-001-virtual-alloc

目标：

理解虚拟内存申请。

涉及：

-   VirtualAlloc
-   VirtualFree

观察：

-   Memory Map
-   地址变化

------------------------------------------------------------------------

## MEM-002-memory-protect

目标：

理解内存权限变化。

涉及：

-   VirtualProtect

观察：

-   PAGE_READWRITE
-   PAGE_EXECUTE

------------------------------------------------------------------------

# FILE - 文件

## FILE-001-file-read-write

目标：

理解文件访问。

涉及：

-   CreateFile
-   ReadFile
-   WriteFile

观察：

-   文件句柄
-   IO行为

------------------------------------------------------------------------

# DLL - 模块

## DLL-001-load-library

目标：

理解 DLL 加载。

涉及：

-   LoadLibrary
-   GetProcAddress

观察：

-   Module列表
-   DLL生命周期

------------------------------------------------------------------------

# GUI - 窗口

## GUI-001-create-window

目标：

理解 Windows GUI。

涉及：

-   CreateWindowEx
-   Message Loop

观察：

-   Window对象
-   消息流

------------------------------------------------------------------------

# INPUT - 输入自动化

## INPUT-001-window-message

目标：

理解窗口消息控制。

涉及：

-   SendMessage
-   PostMessage
-   FindWindow

观察：

-   消息传递

------------------------------------------------------------------------

# NET - 网络

## NET-001-socket-client

目标：

理解 TCP通信。

涉及：

-   socket
-   connect
-   send
-   recv

观察：

-   网络连接
-   数据流

------------------------------------------------------------------------

## NET-002-http-client

目标：

理解 HTTP 请求。

涉及：

-   WinHTTP
-   WinINet

观察：

-   请求流程

------------------------------------------------------------------------

# EXC - 异常

## EXC-001-access-violation

目标：

模拟真实崩溃。

异常：

    ExceptionCode:

    c0000005

类型：

-   Read violation
-   Write violation
-   Execute violation

观察：

-   Exception地址
-   Call Stack

------------------------------------------------------------------------

# PE - 可执行文件

## PE-001-import-table

目标：

理解 EXE依赖。

观察：

-   Import Table
-   DLL依赖

------------------------------------------------------------------------

# HOOK - 高级扩展

## HOOK-001-dll-injection

目标：

理解 DLL进入目标进程。

------------------------------------------------------------------------

## HOOK-002-api-hook

目标：

理解 API Hook。

包括：

-   IAT Hook
-   Inline Hook

------------------------------------------------------------------------

## HOOK-003-agent

最终目标：

    目标程序

    ↓

    Injected DLL

    ↓

    内部数据

    ↓

    HTTP API

    ↓

    外部访问

------------------------------------------------------------------------

# 6. Demo目录结构

推荐：

    windows-native-lab

    ├── docs
    │
    ├── demos
    │
    │   ├── PROC
    │   │   └── PROC-001-create-process
    │   │
    │   ├── MEM
    │   │   └── MEM-001-virtual-alloc
    │   │
    │   └── NET
    │       └── NET-001-socket-client
    │
    └── reports

------------------------------------------------------------------------

# 7. 单个Demo标准结构

例如：

    MEM-001-virtual-alloc

    ├── README.md
    ├── source
    │   └── main.cpp
    ├── bin
    │   └── demo.exe
    └── analysis.md

------------------------------------------------------------------------

# 8. README标准

内容：

    实验目标

    背景知识

    涉及API

    模拟场景

    运行方式

    观察目标

    预期结果

------------------------------------------------------------------------

# 9. analysis.md标准

内容：

    实验名称

    环境

    断点位置

    调用流程

    寄存器变化

    内存变化

    线程变化

    异常信息

    结论

------------------------------------------------------------------------

# 10. 完成标准

一个Demo完成：

-   程序运行成功
-   x64dbg成功观察
-   理解调用流程
-   完成分析记录
-   关联Knowledge Map

------------------------------------------------------------------------

# 11. 最终目标

形成：

    Windows API Knowledge Map

            +

    Demo实验库

            +

    x64dbg分析报告


    =

    Windows用户态程序观察、分析和实验能力
