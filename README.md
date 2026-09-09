# Windows Native Analysis Lab

Windows 用户态程序观察、分析与实验能力建设项目。

A personal laboratory for exploring Windows user-mode programs.


通过 C++ 实验程序、x64dbg 调试分析以及 Windows API 知识地图，
逐步建立 Windows 用户态程序的观察、分析和实验能力。


## 项目目标

- 理解 Windows 用户态程序运行机制
- 建立面向实际场景的 Win32 API 知识体系
- 使用 x64dbg 分析程序行为
- 掌握调试与逆向分析基础能力
- 探索 DLL、Hook 等程序扩展技术

## 开发环境

当前实验基准环境使用 w64devkit 提供的 MinGW-w64 GCC 工具链。

当前推荐开发环境：

| 项目 | 版本/工具 |
| --- | --- |
| 操作系统 | Windows 10 / Windows 11 |
| 目标架构 | x86-64 |
| 编程语言 | C++ |
| 编译器 | MinGW-w64 (GCC) |
| 工具链 | [w64devkit](https://github.com/skeeto/w64devkit/releases) 2.9.1 |
| 调试工具 | [x64dbg](https://github.com/x64dbg/x64dbg/releases) |
| 编辑器 | 任意文本编辑器（推荐 Notepad++/VS Code） |

其它资源

- https://github.com/therealdreg/DbgChild/releases  
- https://github.com/NationalSecurityAgency/ghidra/releases

## 编译器说明

项目初期不依赖 Visual Studio 工程文件。

实验程序采用：

-   标准 C++ 源文件
-   命令行编译
-   独立 Demo 目录

原因：

-   保持实验程序简单
-   减少 IDE 隐藏行为
-   方便观察最终生成的 PE 文件
-   更贴近 Windows 程序分析流程

## 编译方式

示例：

``` bash
g++ main.cpp -g -O0 -Wall -o demo.exe
```

参数说明：

  参数      说明
  --------- --------------------------------
  `-g`      生成调试信息，方便 x64dbg 分析
  `-O0`     关闭优化，保持代码结构清晰
  `-Wall`   开启编译警告
  `-o`      指定输出文件

## 编译验证

查看编译器版本：

``` bash
g++ --version
```

查看生成文件：

``` bash
file demo.exe
```

Windows 下也可以使用：

``` bash
objdump -f demo.exe
objdump -p demo.exe
```

查看 PE 信息。

## 编译原则

学习阶段遵循：

    源码

    ↓

    命令行编译

    ↓

    生成 EXE

    ↓

    x64dbg 分析

    ↓

    记录实验报告

暂不引入：

-   CMake
-   vcpkg
-   Visual Studio 工程管理

后续进入大型 Demo 或 DLL / Hook 阶段时再考虑。


## 文档

详细设计文档：

- [项目总体规划](docs/01-project-plan.md)
- [Windows API 实战地图](docs/02-windows-api-map.md)
- [实验 Demo 体系](docs/03-demo-system.md)
- [Demo 开发流程](docs/04-demo-development-workflow.md)
- [分析报告模板](docs/05-analysis-report-template.md)


## 路线规划

### Phase 1：基础能力建设

- Windows API Knowledge Map
- 基础 Win32 实验程序
- x64dbg 动态分析


### Phase 2：程序结构与异常分析

- PE 文件分析
- Exception 分析
- DLL 模块分析


### Phase 3：程序扩展能力

- DLL Injection
- API Hook
- Agent 架构


## 项目状态

🚧 实验性 / 学习项目
