# x64dbg 常用指令与模式速查

本文面向 Windows x64 + x64dbg 的初期分析场景。

目标不是学习完整 x86/x64 指令集，而是建立一套实用的“看到汇编 → 快速理解大概意图”的能力。

建议优先掌握：

```text
参数准备
    ↓
call
    ↓
返回值
    ↓
test / cmp
    ↓
je / jne / jmp
```

---

## 1. 第一阶段先记住的寄存器

Windows x64 调用约定中，前四个整数或指针参数通常通过：

| 参数 | 寄存器 |
| --- | --- |
| 第 1 个参数 | RCX |
| 第 2 个参数 | RDX |
| 第 3 个参数 | R8 |
| 第 4 个参数 | R9 |

除此之外，初期重点记住：

| 寄存器 | 初期理解 |
| --- | --- |
| RAX / EAX | 常见函数返回值 |
| RSP | 当前栈顶 |
| RIP | 当前正在执行的指令地址 |

例如：

```asm
call ReadFile
```

调用前：

```text
RCX = hFile
RDX = lpBuffer
R8  = nNumberOfBytesToRead
R9  = lpNumberOfBytesRead
```

调用后：

```text
EAX / RAX
```

通常用于观察函数返回结果。

---

## 2. mov

```asm
mov destination, source
```

最简单理解：

> 把 source 的值放到 destination。

例如：

```asm
mov edx, 80000000h
```

可以理解为：

```text
EDX = 0x80000000
```

如果当前正在准备 `CreateFileA` 参数：

```text
RDX = GENERIC_READ
```

常见用途：

- 参数准备
- 保存返回值
- 保存指针
- 在寄存器之间搬运数据
- 写入栈或内存

---

## 3. lea

```asm
lea register, [address expression]
```

`lea` 的名字是 Load Effective Address。

初期最重要的理解：

> 计算一个地址，并把这个地址放进寄存器。

例如：

```asm
lea rdx, [some_string]
```

在 API 调用附近，可以先理解为：

```text
RDX = 字符串地址
```

### 注意

`lea` 不一定只是“取变量地址”，它也经常被编译器用来做普通地址计算甚至简单算术。

初期在 x64dbg 中遇到：

```asm
lea rcx, [...]
lea rdx, [...]
lea r8,  [...]
```

优先结合后面的 `call` 判断这些是不是正在准备 API 参数。

---

## 4. call

```asm
call target
```

表示调用函数。

例如：

```asm
call ReadFile
```

或者：

```asm
call qword ptr ds:[<ReadFile>]
```

可以先理解成：

```cpp
ReadFile(...);
```

遇到关键 `call` 时优先看：

```text
call 之前：
RCX
RDX
R8
R9
栈参数

call 之后：
RAX / EAX
内存是否发生变化
```

---

## 5. ret

```asm
ret
```

表示当前函数返回。

典型模式：

```asm
xor eax, eax
ret
```

经常可以先理解为：

```cpp
return 0;
```

但具体含义仍应结合上下文确认。

---

## 6. cmp

```asm
cmp a, b
```

可以理解为：

> 比较 a 和 b，并更新 CPU 标志位。

`cmp` 本身通常不会完成分支，后面往往紧跟条件跳转。

例如：

```asm
cmp eax, 3
jne failed
```

初期可以理解为：

```cpp
if (eax != 3) {
    goto failed;
}
```

---

## 7. test

```asm
test a, b
```

`test` 会进行按位 AND，并根据结果更新标志位，但不会保存 AND 的结果。

最常见模式：

```asm
test eax, eax
```

因为：

```text
x AND x = x
```

所以：

```asm
test eax, eax
```

非常常见地用于：

> 检查 EAX 整体是否为 0。

例如：

```asm
call ReadFile
test eax, eax
je failed
```

可以直接先翻译成：

```cpp
if (!ReadFile(...)) {
    goto failed;
}
```

### `test eax, eax` 与检查某一位的区别

```asm
test eax, eax
```

检查：

```text
EAX 整体是否为 0
```

而：

```asm
test eax, 4
```

则可以用于检查 mask 指定的位是否置位。

因此可以先记：

```text
test eax, eax
    → 检查 EAX 是否为 0

test eax, mask
    → 检查指定 bit / flag
```

---

## 8. je / jz

```asm
je target
```

和：

```asm
jz target
```

在实际使用中可以先理解为同一种条件：

> Zero Flag = 1 时跳转。

常见于：

```asm
test eax, eax
je failed
```

可以理解为：

```cpp
if (eax == 0) {
    goto failed;
}
```

---

## 9. jne / jnz

```asm
jne target
```

和：

```asm
jnz target
```

初期可以理解为：

> Zero Flag = 0 时跳转。

例如：

```asm
test eax, eax
jne success
```

大概对应：

```cpp
if (eax != 0) {
    goto success;
}
```

---

## 10. jmp

```asm
jmp target
```

无条件跳转。

可以先理解成：

```cpp
goto target;
```

---

## 11. xor

```asm
xor eax, eax
```

这是非常常见的清零方式。

因为：

```text
x XOR x = 0
```

所以：

```asm
xor eax, eax
```

等价效果：

```text
EAX = 0
```

---

## 12. add / sub

```asm
add a, b
sub a, b
```

分别表示加法和减法。

例如：

```asm
sub rsp, 40h
```

在函数开头非常常见，可以先理解为：

> 在栈上为当前函数预留空间。

函数结束附近可能看到：

```asm
add rsp, 40h
ret
```

即回收栈空间后返回。

---

## 13. push / pop

```asm
push rbx
pop rbx
```

常用于保存和恢复寄存器。

在 Windows x64 编译器输出中，不应把 `push` / `pop` 简单理解为普通参数传递。

Windows x64 的普通函数参数首先使用：

```text
RCX
RDX
R8
R9
```

以及栈上的后续参数。

---

## 14. inc / dec

```asm
inc eax
dec eax
```

分别表示：

```text
+1
-1
```

常见于循环计数、索引变化和引用计数。

---

## 15. and / or

```asm
and eax, mask
or  eax, mask
```

常用于 bit mask、Flags、状态字段和权限字段。

初期只需知道：

```text
AND → 保留/检查某些位
OR  → 设置某些位
```

---

## 16. shl / shr

```asm
shl eax, 1
shr eax, 1
```

分别是左移和右移。

初期可以理解为：

```text
左移 1 位 ≈ 乘以 2
右移 1 位 ≈ 除以 2（无符号场景）
```

---

# 17. 高频组合模式

## 17.1 Win32 BOOL 返回值判断

```asm
call ReadFile
test eax, eax
je failed
```

可以先翻译成：

```cpp
if (!ReadFile(...)) {
    // failed
}
```

---

## 17.2 判断指针是否为空

```asm
test rax, rax
je null_pointer
```

可以理解为：

```cpp
if (ptr == nullptr) {
    ...
}
```

---

## 17.3 判断值是否相等

```asm
cmp eax, 3
je matched
```

大概对应：

```cpp
if (eax == 3) {
    ...
}
```

---

## 17.4 判断值是否不相等

```asm
cmp eax, 3
jne other
```

大概对应：

```cpp
if (eax != 3) {
    ...
}
```

---

## 17.5 API 参数准备

```asm
lea rcx, [fileName]
mov edx, 80000000h
mov r8d, 1
xor r9d, r9d
call CreateFileA
```

可以按：

```text
RCX → 第1参数
RDX → 第2参数
R8  → 第3参数
R9  → 第4参数
```

还原 API 参数。

---

## 17.6 超过四个参数

Windows x64 前四个参数走：

```text
RCX
RDX
R8
R9
```

第五个及之后的参数通常放在栈中。

停在 `call` 执行前时：

```text
[RSP+20] → 第5参数
[RSP+28] → 第6参数
[RSP+30] → 第7参数
```

例如：

```cpp
CreateFileA(
    fileName,
    GENERIC_READ,
    FILE_SHARE_READ,
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
);
```

可能观察到：

```text
RCX      = fileName
RDX      = 0x80000000
R8       = 1
R9       = 0

[RSP+20] = 3
[RSP+28] = 0x80
[RSP+30] = 0
```

---

# 18. x64dbg 初期分析方法

遇到大量汇编时，不需要逐条翻译。

优先寻找：

```text
String Reference
        ↓
关键 call
        ↓
API 参数
        ↓
返回值
        ↓
test / cmp
        ↓
条件跳转
```

例如 FILE-001：

```text
"config.ini"
    ↓
String Reference
    ↓
CreateFileA
    ↓
ReadFile
    ↓
Buffer 内容变化
```

分析 `ReadFile` 时：

### call 之前

观察：

```text
RCX = HANDLE
RDX = Buffer
R8  = Length
R9  = BytesRead address
```

### call 之后

观察：

```text
EAX = BOOL result
```

并重新查看调用前记录的 Buffer 地址：

```text
Before ReadFile:
Buffer 尚未包含配置内容

After ReadFile:
Buffer 包含 config.ini 内容
```

---

# 19. 初期不要做的事

暂时不需要：

- 背完整 x86/x64 指令集
- 理解每一个寄存器的所有 ABI 规则
- 逐条解释整个函数
- 研究每个 CPU Flag
- 深入 SIMD / AVX 指令
- 深入浮点指令
- 一看到陌生指令就中断整个分析

推荐原则：

> 以行为和 API 为锚点，需要什么指令就补什么指令。

---

# 20. 当前应熟悉的最小集合

第一阶段真正需要做到“看到就基本认识”的：

```text
mov
lea

call
ret

cmp
test

je / jz
jne / jnz
jmp

xor

add
sub

push
pop
```

寄存器：

```text
RCX
RDX
R8
R9

RAX / EAX
RSP
RIP
```

高频模式：

```asm
test eax, eax
je ...

test rax, rax
je ...

cmp ..., ...
je / jne ...

call API
```

当这些内容逐渐熟悉以后，再根据实际 Demo 增加新的指令，而不是一次性学习完整指令集。

---

## 官方资料

### Intel

Intel® 64 and IA-32 Architectures Software Developer’s Manual

重点参考：

```text
Volume 2: Instruction Set Reference
```

用于查询：

```text
MOV
LEA
TEST
CMP
Jcc
CALL
RET
...
```

### Microsoft

Windows x64 Calling Convention

用于理解：

```text
RCX
RDX
R8
R9
Shadow Space
Stack Arguments
Return Value
```

Win32 API 文档用于查询：

```text
CreateFileA
ReadFile
WriteFile
Sleep
...
```

三类资料的职责可以简单记成：

```text
CPU 指令
    → Intel Manual

Windows x64 ABI
    → Microsoft Calling Convention

Win32 API
    → Microsoft Win32 Documentation
```
