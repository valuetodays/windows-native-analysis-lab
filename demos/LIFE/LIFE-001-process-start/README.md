# LIFE-001-process-start

## 实验目标

观察 Windows 用户态程序启动流程。

## API选择

使用 MessageBoxW 而不是 MessageBox：

- Windows 内部原生使用 Unicode
- MessageBox 是宏封装
- MessageBoxA 是 ANSI 兼容接口


## 验证内容

- PE EntryPoint
- Module加载
- CRT初始化
- main调用

## 涉及知识

- PE Format
- Process Lifecycle

## 观察点

x64dbg:

- EntryPoint
- main
- ExitProcess