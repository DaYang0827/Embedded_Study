# 嵌入式软件工程师秋招八股背诵稿
## ——严格根据个人简历定制

---

# 第一章 C语言与嵌入式编程基础

## 1. `static` 有什么作用？ ★★★★★

`static` 在 C 语言中主要有两个核心作用：**延长变量生命周期**和**限制符号作用域**。

函数内部的 `static` 局部变量只初始化一次，生命周期持续到整个程序结束，但作用域仍然只在当前函数内部。

文件作用域下的 `static` 全局变量和 `static` 函数只能在当前 `.c` 文件中访问，可以减少不同模块之间的命名冲突，实现模块内部封装。

例如：

```c
static uint8_t rx_buffer[256];

static void protocol_reset(void)
{
}
```

这里 `rx_buffer` 和 `protocol_reset()` 都只供当前模块内部使用。

**项目联系：**

在 Bootloader、串口协议、Ring Buffer 等驱动模块中，我会把只在本模块使用的辅助函数声明成 `static`，避免向其他模块暴露不必要的接口。

---

## 2. `extern` 有什么作用？ ★★★★★

`extern` 用于声明一个变量或者函数的定义位于其他位置。

例如：

```c
// uart.h
extern volatile uint8_t rx_flag;

// uart.c
volatile uint8_t rx_flag = 0;
```

头文件中的 `extern` 只是告诉编译器：

> 这个变量存在，但真正的存储空间在其他 `.c` 文件中分配。

所以多文件工程通常采用：

```text
.h
负责声明接口

.c
负责真正定义和实现
```

这样可以实现模块化编程。

---

## 3. 声明和定义有什么区别？ ★★★★

声明的作用是告诉编译器：

> 某个变量或者函数存在，它的类型是什么。

定义则是真正创建变量或者实现函数。

例如：

```c
extern int count;
```

是声明。

```c
int count = 0;
```

是定义。

函数：

```c
void uart_init(void);
```

是声明。

```c
void uart_init(void)
{
    ...
}
```

是定义。

如果使用一个没有声明的变量或函数，通常在**编译阶段**报错。

如果只有声明但最终找不到定义，通常在**链接阶段**出现 Undefined Symbol 一类错误。

---

# 4. `const` 有什么作用？ ★★★★★

`const` 表示对象通过当前接口不能被修改。

最常见的是三个指针形式。

### 第一种

```c
const int *p;
```

或者：

```c
int const *p;
```

表示：

> 可以修改 `p` 的指向，但是不能通过 `p` 修改它所指向的数据。

### 第二种

```c
int * const p = &a;
```

表示：

> 指针 `p` 本身不能再指向其他位置，但是可以修改 `*p`。

### 第三种

```c
const int * const p = &a;
```

表示：

> 指针本身不能改变，通过这个指针也不能修改数据。

简单记忆：

```text
const 在 * 左边
→ 数据不能改

const 在 * 右边
→ 指针不能改
```

---

# 5. `const` 和 `#define` 有什么区别？ ★★★★

`#define` 本质是**预处理阶段的文本替换**，没有真正的数据类型。

例如：

```c
#define BUFFER_SIZE 256
```

编译前会直接进行替换。

而：

```c
const uint16_t buffer_size = 256;
```

具有明确的数据类型，可以参与编译器的类型检查。

所以一般来说：

```text
#define
→ 宏替换

const
→ 有类型的只读对象
```

在能够使用 `const` 表达语义时，通常类型安全性和可读性更好。

---

# 6. `volatile` 是什么？ ★★★★★

`volatile` 告诉编译器：

> 这个对象的值可能被当前代码流程之外的因素修改，因此每次访问都必须保留真实的访问行为，不能简单假设之前读到的值一直有效。

嵌入式中最常见的两个场景：

### 第一种：硬件寄存器

硬件可能随时改变寄存器值。

### 第二种：中断和主程序共享变量

例如：

```c
volatile uint8_t rx_done = 0;
```

中断：

```c
void USART1_IRQHandler(void)
{
    rx_done = 1;
}
```

主循环：

```c
while(1)
{
    if(rx_done)
    {
        ...
    }
}
```

如果没有 `volatile`，编译器可能对读取进行优化。

但是要特别注意：

> `volatile` 不等于线程安全，也不能保证复杂操作的原子性。

---

# 7. `volatile` 和原子变量有什么区别？ ★★★★★

`volatile` 主要解决的是：

> 编译器优化和访问可见性问题。

它并不能保证：

```c
value++;
```

这种“读取—修改—写回”的复合操作不会被其他执行上下文打断。

原子变量解决的是：

> 某个读改写操作在执行过程中不可被拆开观察，从而避免简单共享状态的竞争问题。

所以：

```text
volatile
→ 不要乱优化

atomic
→ 操作不可分割
```

我在 nRF52840 项目中使用 `atomic_t` 管理异步 BLE 回调与主程序之间的共享状态，就是为了避免并发修改造成状态不一致。

---

# 8. 指针是什么？ ★★★★★

指针本质上也是一个变量。

普通变量保存的是：

```text
数据
```

而指针变量保存的是：

```text
地址
```

例如：

```c
int a = 10;
int *p = &a;
```

那么：

```text
p
保存 a 的地址

*p
访问这个地址中的数据
```

所以：

```c
*p = 20;
```

实际上修改的是 `a`。

---

# 9. `void *` 是什么？ ★★★★

`void *` 是一种通用指针类型。

它可以保存任意类型对象的地址，但是因为它没有具体的数据类型信息，所以在解引用前通常需要转换为具体指针类型。

例如：

```c
void func(void *arg)
{
    LED_TypeDef *led = (LED_TypeDef *)arg;
}
```

RTOS、回调函数和通用驱动接口中经常使用 `void *` 来传递不同类型的参数。

---

# 10. 数组和指针有什么区别？ ★★★★★

数组和指针虽然在很多表达式中表现相似，但本质并不是同一个东西。

例如：

```c
int a[10];
```

在大多数表达式中：

```c
a
```

会转换成：

```text
指向 a[0] 的指针
```

所以：

```c
a + 1
```

指向：

```c
a[1]
```

但是：

```c
sizeof(a)
```

得到的是整个数组占用的大小。

如果：

```c
int *p = a;
```

那么：

```c
sizeof(p)
```

得到的是指针本身的大小。

---

# 11. `a` 和 `&a` 有什么区别？ ★★★★

假设：

```c
int a[10];
```

`a` 在表达式中通常表示：

```text
首元素地址
类型近似 int *
```

而：

```c
&a
```

表示：

```text
整个数组的地址
类型 int (*)[10]
```

它们的数值地址可能相同，但指针类型不同。

因此：

```c
a + 1
```

移动一个 `int`。

而：

```c
&a + 1
```

会移动整个 `int[10]` 数组的大小。

---

# 12. 函数指针是什么？ ★★★★★

函数编译完成之后，本身也存放在 Flash 的某个代码地址。

函数指针就是：

> 保存函数入口地址的指针。

例如：

```c
typedef void (*app_func_t)(void);
```

定义：

```c
app_func_t app_entry;
```

然后：

```c
app_entry = (app_func_t)reset_handler_addr;
app_entry();
```

CPU 就会跳转到这个函数地址执行。

**项目联系：**

Bootloader 跳转 APP 时，我会读取 APP 向量表中的 Reset Handler 地址，然后转换成函数指针并执行。

---

# 13. 什么是回调函数？ ★★★★★

回调的核心思想是：

> 把函数地址交给另一个模块，由另一个模块在某个事件发生时调用。

例如：

```c
void register_callback(void (*callback)(void));
```

调用：

```c
register_callback(data_ready_handler);
```

以后数据就绪时：

```text
底层模块
↓
调用 callback
↓
进入 data_ready_handler()
```

回调可以降低模块之间的直接依赖，实现事件驱动和模块解耦。

---

# 14. 什么是结构体？为什么嵌入式经常使用？ ★★★★★

结构体可以把多个不同类型但逻辑上相关的数据组合在一起。

例如：

```c
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t pin;
    uint32_t clock;
} LED_TypeDef;
```

这样可以把：

```text
GPIO端口
Pin
Clock
```

封装成一个逻辑对象。

在驱动设计中，结构体非常适合保存：

```text
外设配置
设备状态
协议字段
Handle
上下文信息
```

从而提高模块化程度。

---

# 15. 什么是结构体内存对齐？ ★★★★★

CPU 对不同大小的数据通常有一定的地址对齐要求。

因此编译器为了提高访问效率，可能在结构体成员之间插入 Padding。

例如：

```c
struct A
{
    char a;
    int b;
};
```

不能简单认为：

```text
sizeof(A) = 1 + 4 = 5
```

因为 `int` 通常需要满足一定对齐要求，所以可能变成：

```text
char
padding
padding
padding
int
```

最终可能占 8 Byte。

所以：

> `sizeof(struct)` 不一定等于每个成员大小的简单相加。

---

# 16. `packed` 有什么作用？ ★★★★

`packed` 的作用是要求编译器减少或者取消结构体成员之间的 Padding。

这样可以减少存储空间，也可以让结构体内存布局严格匹配某些通信协议。

但是代价是：

> 某些非对齐访问可能效率下降，在特定架构或指令下还可能存在访问限制。

所以通信协议中不能看到结构体就随便强转，必须同时考虑：

```text
Padding
字节序
字段宽度
对齐
```

---

# 17. 什么是大小端？ ★★★★★

大小端只讨论：

> 一个多字节数值拆成多个 Byte 后，字节如何排列。

例如：

```text
0x12345678
```

大端：

```text
低地址
12 34 56 78
高地址
```

小端：

```text
低地址
78 56 34 12
高地址
```

STM32 Cortex-M 日常开发通常按照小端方式理解。

要注意：

> 大小端不改变数值本身，只改变多字节数据的字节排列。

---

# 18. C 程序主要有哪些内存区域？ ★★★★★

对于 MCU 固件，可以重点理解：

```text
.text
代码

.rodata
只读常量

.data
已初始化的全局/static变量

.bss
未初始化或零初始化的全局/static变量

Heap
动态内存

Stack
局部数据、函数调用现场等
```

其中 `.data` 和 `.bss` 运行时通常位于 SRAM。

`.data` 的初始值需要存放在 Flash 固件镜像中，启动时复制到 SRAM。

`.bss` 在启动时被清零。

---

# 19. Stack 和 Heap 有什么区别？ ★★★★★

Stack 通常由编译器和运行环境自动管理。

函数调用过程中会使用 Stack 保存：

```text
部分局部变量
需要保存的寄存器
函数调用现场
```

函数退出后，相应栈空间自动回收。

Heap 用于动态内存分配：

```c
malloc()
free()
```

Heap 更灵活，但要考虑：

```text
内存泄漏
碎片
分配失败
运行时间不确定
```

因此在资源受限、强调确定性的 MCU 中，动态内存通常需要谨慎使用。

---

# 20. 为什么不建议在函数里定义特别大的局部数组？ ★★★★

因为局部数组通常会占用当前任务或者主程序的 Stack。

例如：

```c
void func(void)
{
    uint8_t buffer[20000];
}
```

如果 Stack 本身没有这么大，就可能造成：

```text
Stack Overflow
```

进一步破坏其他内存区域，引起 HardFault 或异常行为。

---

# 21. 什么是内存泄漏？ ★★★★

动态申请的内存使用完成后没有正确释放，并且程序以后无法继续正常管理这块内存，就形成内存泄漏。

例如：

```c
uint8_t *p = malloc(100);
```

使用结束后如果一直不：

```c
free(p);
```

内存就可能一直被占用。

嵌入式设备可能运行几个月甚至几年，所以即使每次只泄漏少量内存，长期运行也可能最终耗尽 Heap。

---

# 22. 什么是野指针？ ★★★★

野指针就是：

> 指向无效或者未知内存区域的指针。

常见原因：

```text
指针没有初始化

free之后继续使用

返回局部变量地址

数组越界导致地址错误
```

防止方式：

```c
uint8_t *p = NULL;
```

动态申请后检查：

```c
if(p != NULL)
```

释放后：

```c
free(p);
p = NULL;
```

---

# 23. `memcpy` 和 `memmove` 有什么区别？ ★★★★

两者都可以进行内存复制。

`memcpy()` 适用于：

```text
源区域和目标区域不重叠
```

而 `memmove()` 会正确处理：

```text
源区域与目标区域发生重叠
```

所以：

> 如果两块内存可能发生重叠，应该使用 `memmove()`。

---

# 24. 常见位操作怎么写？ ★★★★★

某位置 1：

```c
reg |= (1U << n);
```

某位清 0：

```c
reg &= ~(1U << n);
```

某位翻转：

```c
reg ^= (1U << n);
```

判断某位：

```c
if(reg & (1U << n))
{
}
```

一句话：

```text
|   置1
&~  清0
^   翻转
&   判断
```

---

# 25. 如何访问一个绝对地址的硬件寄存器？ ★★★★★

例如：

```c
#define REG (*(volatile uint32_t *)0x40000000)
```

理解过程：

```text
0x40000000
↓
转换成 uint32_t *
↓
volatile
↓
解引用 *
↓
得到地址中的寄存器内容
```

所以：

```c
REG = 1;
```

就是向这个寄存器写入数据。

---

# 26. 什么是状态机？ ★★★★★

状态机就是：

> 把复杂流程拆分成多个明确状态，并根据事件决定状态之间如何切换。

典型形式：

```c
switch(state)
{
    case WAIT_HEAD:
        break;

    case READ_CMD:
        break;

    case READ_LEN:
        break;

    case READ_DATA:
        break;

    case READ_CRC:
        break;
}
```

状态机特别适合：

```text
串口协议解析
通信流程
升级流程
设备控制
```

它的优点是：

> 状态明确、流程清晰、异常后容易恢复。

---

# 27. 什么是模块化编程？ ★★★★★

模块化编程就是：

> 把不同功能拆成相对独立的软件模块，每个模块只暴露必要接口。

例如：

```text
uart.c / uart.h
flash.c / flash.h
ring_buffer.c / ring_buffer.h
bootloader.c / bootloader.h
protocol.c / protocol.h
```

`.h` 负责接口。

`.c` 负责实现。

模块内部辅助函数用：

```c
static
```

隐藏。

这样可以降低耦合，提高维护性和复用性。

---

# 第二章 STM32F407 与 Cortex-M 基础

# 28. STM32F407 使用什么内核？ ★★★★★

STM32F407 使用 ARM Cortex-M4 内核，并带有浮点运算单元和 DSP 相关指令支持。

Cortex-M4 面向 MCU 和实时控制场景。

它具有：

```text
NVIC
SysTick
中断向量表
MSP / PSP
Thumb/Thumb-2指令
```

等典型 Cortex-M 机制。

---

# 29. MCU 从上电到 `main()` 经历什么？ ★★★★★

整体过程：

```text
上电 / Reset
↓
读取向量表第0项
↓
设置初始 MSP
↓
读取向量表第1项
↓
得到 Reset_Handler
↓
执行启动代码
↓
初始化 .data
↓
清零 .bss
↓
SystemInit
↓
进入 C 运行环境
↓
main()
```

所以：

> `main()` 并不是 MCU 上电后执行的第一段程序。

---

# 30. 中断向量表是什么？ ★★★★★

中断向量表本质上是一张：

> 异常和中断入口地址表。

Cortex-M 向量表前两个表项比较特殊。

第 0 项：

```text
Initial MSP
```

第 1 项：

```text
Reset_Handler 地址
```

后面的表项才是：

```text
NMI
HardFault
SysTick
USART
EXTI
...
```

对应 Handler 的入口地址。

---

# 31. MSP 是什么？ ★★★★★

MSP：

```text
Main Stack Pointer
主栈指针
```

MCU Reset 后默认使用 MSP。

它保存当前主栈顶部的位置。

启动时：

```text
Vector Table[0]
↓
Initial MSP
↓
写入 MSP
```

中断 Handler 模式通常使用 MSP。

---

# 32. MSP 和 PSP 有什么区别？ ★★★★

Cortex-M 有两个栈指针：

```text
MSP
Main Stack Pointer

PSP
Process Stack Pointer
```

复位启动默认使用 MSP。

在 RTOS 中，经常会让：

```text
任务线程
使用 PSP

异常 / 中断 / 内核
使用 MSP
```

这样可以把任务栈和系统栈进行隔离。

---

# 33. SP、LR、PC 分别是什么？ ★★★★★

### SP

```text
Stack Pointer
栈指针
R13
```

指向当前栈的位置。

### LR

```text
Link Register
连接寄存器
R14
```

普通函数使用 `BL` 调用时，LR 用来保存返回位置。

### PC

```text
Program Counter
程序计数器
R15
```

决定 CPU 当前代码执行位置。

简单记忆：

```text
SP
管 Stack

LR
管返回

PC
管执行位置
```

---

# 34. `Reset_Handler` 做什么？ ★★★★★

`Reset_Handler` 是芯片复位后开始执行的启动程序。

它主要负责建立 C 程序运行环境，例如：

```text
初始化 .data
清零 .bss
调用系统初始化代码
进入 C 运行环境
最终进入 main()
```

所以 Bootloader 跳 APP 时一般跳转到：

```text
APP Reset_Handler
```

而不是直接调用 APP 的 `main()`。

---

# 35. 为什么 Bootloader 不直接跳 `main()`？ ★★★★★

因为 `main()` 前还有启动过程。

例如：

```text
.data 初始化
.bss 清零
SystemInit
C运行环境初始化
```

如果 Bootloader 直接进入 APP `main()`：

> APP 可能处于一个没有正确完成启动初始化的环境。

因此正确做法通常是：

```text
进入 APP 自己的 Reset_Handler
```

让 APP 按正常启动流程运行。

---

# 36. NVIC 是什么？ ★★★★★

NVIC：

```text
Nested Vectored Interrupt Controller
嵌套向量中断控制器
```

主要负责：

```text
中断使能和关闭
中断优先级
中断挂起
中断嵌套
中断响应管理
```

当外设产生中断请求后：

```text
外设
↓
NVIC
↓
CPU
↓
向量表
↓
对应 Handler
```

---

# 37. MCU 中断处理流程是什么？ ★★★★★

典型流程：

```text
正常程序运行
↓
外设产生中断请求
↓
NVIC判断使能和优先级
↓
CPU进入异常
↓
硬件保存必要现场
↓
根据向量表进入 ISR
↓
执行中断服务程序
↓
异常返回
↓
恢复现场
↓
继续原程序
```

Cortex-M 异常进入时，硬件会自动保存一部分核心寄存器现场。

---

# 38. 什么是中断嵌套？ ★★★★★

如果 CPU 正在执行一个较低优先级的 ISR，此时产生满足抢占条件的更高优先级中断，那么高优先级中断可以打断当前 ISR。

流程：

```text
低优先级ISR
↓
高优先级中断到来
↓
暂停低优先级ISR
↓
执行高优先级ISR
↓
返回
↓
继续低优先级ISR
```

这就是中断嵌套。

---

# 39. 为什么 ISR 要“快进快出”？ ★★★★★

因为 ISR 会打断正常程序执行。

如果 ISR 里面执行大量：

```text
Flash写入
复杂协议解析
printf
长时间循环
```

会造成：

```text
中断响应延迟
其他中断得不到及时处理
主程序长期不能运行
实时性下降
```

所以 ISR 通常只做：

```text
读取数据
清除标志
保存必要信息
通知主程序或线程
快速退出
```

复杂工作放到主循环或线程里执行。

---

# 40. 为什么中断里通常不建议 `printf()`？ ★★★★

因为 `printf()`：

```text
执行时间可能很长
格式化过程复杂
可能依赖串口
可能涉及锁或共享资源
```

导致 ISR 时间不可控。

因此调试 ISR 时，更推荐：

```text
设置状态
记录简单数据
GPIO翻转
事后日志分析
```

而不是大量 `printf()`。

---

# 41. SysTick 是什么？ ★★★★

SysTick 是 Cortex-M 内核提供的系统定时器。

它可以周期性产生 SysTick 异常。

常见用途：

```text
系统节拍
软件定时
RTOS调度时间基准
```

Bootloader 跳转 APP 前如果 Bootloader 使用了 SysTick，一般需要停止或者清理相关状态，避免影响 APP。

---

# 42. VTOR 是什么？ ★★★★★

VTOR：

```text
Vector Table Offset Register
向量表偏移寄存器
```

它告诉 Cortex-M：

> 当前异常和中断应该去哪里查向量表。

例如：

```text
Bootloader Vector Table
位于 Bootloader Flash 区

APP Vector Table
位于 APP Flash 区
```

进入 APP 后：

```c
SCB->VTOR = APP_ADDRESS;
```

这样 APP 产生中断时就会进入 APP 自己的 Handler。

---

# 43. 为什么 Bootloader 和 APP 要有两张向量表？ ★★★★★

因为 Bootloader 和 APP 是两个独立编译的程序。

它们内部：

```text
USART_Handler
SysTick_Handler
HardFault_Handler
...
```

的实际 Flash 地址可能完全不同。

所以：

```text
Bootloader
需要自己的向量表

APP
也需要自己的向量表
```

进入 APP 后必须让 CPU 使用 APP 的向量表。

---

# 第三章 GPIO、USART、I2C、SPI、DMA、PWM、ADC

# 44. GPIO 有哪些常见模式？ ★★★★★

GPIO 常见功能包括：

```text
输入
输出
复用功能
模拟模式
```

输出还经常涉及：

```text
Push-Pull
推挽

Open-Drain
开漏
```

输入端还可以配置：

```text
上拉
下拉
浮空
```

具体配置取决于外设和电气要求。

---

# 45. 推挽输出和开漏输出有什么区别？ ★★★★

推挽输出可以主动输出：

```text
高电平
低电平
```

驱动能力比较强。

开漏输出通常只能主动拉低：

```text
Low
```

高电平依靠外部或者内部上拉产生。

I2C 的 SDA 和 SCL 通常使用：

```text
开漏 + 上拉
```

这样多个器件可以共享总线而不会直接发生高低电平驱动冲突。

---

# 46. UART 和 USART 有什么区别？ ★★★★★

UART：

```text
Universal Asynchronous Receiver Transmitter
```

主要进行异步串行通信。

USART：

```text
Universal Synchronous/Asynchronous Receiver Transmitter
```

理论上既可以支持同步模式，也可以支持异步模式。

STM32 项目中 USART 非常常见，但实际使用时大多数还是：

```text
异步 UART 模式
```

---

# 47. UART 为什么叫异步通信？ ★★★★★

因为 UART 通信双方之间没有单独共享的时钟线。

所以双方必须提前约定：

```text
Baud Rate
Data Bits
Parity
Stop Bits
```

接收方根据约定的波特率进行采样。

---

# 48. UART 一帧数据是什么结构？ ★★★★★

典型 UART 帧：

```text
空闲状态
↓
Start Bit
↓
Data Bits
↓
可选 Parity
↓
Stop Bit
```

常见配置：

```text
8N1
```

表示：

```text
8个数据位
No Parity
1个停止位
```

---

# 49. 波特率不一致会发生什么？ ★★★★

因为 UART 没有独立同步时钟。

如果发送端和接收端波特率偏差过大，接收端采样位置会逐渐偏离正确 Bit 中心。

最终可能产生：

```text
乱码
帧错误
校验错误
丢数据
```

---

# 50. UART 轮询、中断和 DMA 有什么区别？ ★★★★★

### 轮询

CPU 一直检查寄存器。

优点：

```text
简单
```

缺点：

```text
占用CPU
```

### 中断

数据到来时通知 CPU。

优点：

```text
CPU不用一直等待
```

缺点：

> 数据频率很高时，中断次数会很多。

### DMA

由硬件批量搬运数据。

优点：

```text
大幅减少CPU逐字节搬运
适合大量数据
```

---

# 51. I2C 使用哪两根线？ ★★★★★

I2C 使用：

```text
SCL
Serial Clock

SDA
Serial Data
```

I2C 是：

```text
同步
串行
支持多设备寻址
```

的总线。

---

# 52. I2C 一次基本通信流程是什么？ ★★★★★

典型写操作：

```text
START
↓
Slave Address + Write
↓
ACK
↓
Data
↓
ACK
↓
...
↓
STOP
```

读取流程通常包含：

```text
START
地址
ACK
寄存器地址
Repeated START
地址 + Read
数据
NACK
STOP
```

具体流程由器件协议决定。

---

# 53. I2C 中 ACK 和 NACK 是什么？ ★★★★★

发送方发送完一个 Byte 后，接收方在第 9 个时钟周期给出应答。

ACK 表示：

> 数据已经正确接收，可以继续。

NACK 表示：

> 不再继续接收，或者当前传输存在问题。

在主机读取多个 Byte 时，主机通常对前面的 Byte 返回 ACK，最后一个 Byte 返回 NACK，然后 STOP。

---

# 54. 什么是 Repeated START？ ★★★★

Repeated START 就是在不释放总线的情况下再次产生 START。

常用于：

```text
先写寄存器地址
↓
不STOP
↓
Repeated START
↓
切换成读取
```

这样整个访问过程保持为一次连续事务。

---

# 55. SPI 有哪几根线？ ★★★★★

典型 SPI：

```text
SCK
时钟

MOSI
Master Out Slave In

MISO
Master In Slave Out

CS / NSS
片选
```

SPI 是：

```text
同步
全双工
主从式
```

通信接口。

---

# 56. SPI 中 CPOL 和 CPHA 是什么？ ★★★★★

CPOL 决定：

> 时钟空闲时是高电平还是低电平。

CPHA 决定：

> 数据在哪一个时钟边沿采样。

二者组合形成：

```text
Mode 0
Mode 1
Mode 2
Mode 3
```

主机和从机必须配置一致。

否则可能出现：

```text
数据错位
读到错误Bit
通信失败
```

---

# 57. SPI 的 CS 有什么作用？ ★★★★★

CS：

```text
Chip Select
```

用于选择具体从设备。

例如一条 SPI 总线上连接多个器件：

```text
Flash
LCD
ADC
```

可以共享：

```text
SCK
MOSI
MISO
```

但每个设备通常有独立 CS。

主机把某个设备 CS 拉到有效状态后，只有这个设备响应当前通信。

---

# 58. UART、I2C、SPI 怎么比较？ ★★★★★

UART：

```text
异步
通常TX/RX
简单
点对点常见
```

I2C：

```text
同步
两根线
具有地址机制
一条总线可以连接多个设备
```

SPI：

```text
同步
通常四根线
全双工
速度高
使用CS选择从设备
```

所以没有哪一个绝对最好，需要根据：

```text
速度
引脚数量
设备数量
距离
协议复杂度
```

来选择。

---

# 59. DMA 是什么？ ★★★★★

DMA：

```text
Direct Memory Access
直接内存访问
```

DMA 的作用是：

> 在 CPU 不需要逐字节参与搬运的情况下，由 DMA 硬件完成外设和内存之间的数据传输。

例如：

```text
USART RX
↓
DMA
↓
RAM Buffer
```

CPU 只需要：

```text
提前配置DMA
↓
等待完成/半完成事件
↓
处理数据
```

可以明显降低 CPU 数据搬运负担。

---

# 60. DMA 通常需要配置哪些内容？ ★★★★★

主要包括：

```text
数据源地址

目标地址

传输方向

传输数量

源地址是否自增

目标地址是否自增

数据宽度

优先级

Normal / Circular 模式

中断
```

如果是：

```text
USART RX → RAM
```

通常：

```text
外设地址固定
RAM地址递增
```

---

# 61. DMA Normal 和 Circular 模式有什么区别？ ★★★★

Normal：

> 完成指定数量传输后停止。

适合：

```text
一次性发送
一次性接收
```

Circular：

> 到达 Buffer 尾部后自动重新从起始位置继续。

适合：

```text
ADC连续采样
UART连续数据流
音频数据
```

---

# 62. DMA 能完全代替 CPU 吗？ ★★★★

不能。

DMA 主要负责：

```text
数据搬运
```

CPU 仍然负责：

```text
配置DMA

解析协议

处理业务

判断数据是否完整

处理异常

响应DMA完成事件
```

所以 DMA 是：

> 降低 CPU 的搬运工作，而不是替代 CPU 处理业务。

---

# 63. PWM 是什么？ ★★★★

PWM：

```text
Pulse Width Modulation
脉宽调制
```

通过改变一个周期内高电平所占比例来控制平均输出效果。

主要参数：

```text
Frequency
频率

Duty Cycle
占空比
```

例如：

```text
舵机控制
电机调速
LED调光
```

---

# 64. STM32 PWM 频率由什么决定？ ★★★★

可以基本理解为：

```text
定时器时钟
↓
PSC分频
↓
Counter
↓
ARR决定周期
```

公式：

```text
f_counter = f_timer / (PSC + 1)

f_PWM = f_counter / (ARR + 1)
```

CCR 决定比较位置，从而影响占空比。

---

# 65. ADC 是什么？ ★★★★

ADC：

```text
Analog to Digital Converter
模数转换器
```

把连续模拟电压转换成数字量。

例如 N 位 ADC：

```text
数字范围
0 ~ 2^N - 1
```

理想情况下：

```text
ADC_Value
≈
Vin / Vref × (2^N - 1)
```

常见用途：

```text
电压
温度
光敏
传感器
```

采集。

---

# 第四章 Ring Buffer、数据帧与协议解析

# 66. 什么是 Buffer？ ★★★★★

Buffer 就是：

> 数据的临时缓冲区域。

通信中经常出现：

```text
接收速度
≠
处理速度
```

因此先：

```text
收到数据
↓
放Buffer
↓
主程序慢慢处理
```

可以解耦数据接收和业务处理。

---

# 67. 什么是 Ring Buffer？ ★★★★★

Ring Buffer：

```text
环形缓冲区
```

本质上仍然是一段普通数组，但是通过软件把数组尾部和头部逻辑连接起来。

通常有两个索引：

```text
Head
写位置

Tail
读位置
```

写数据：

```text
buffer[head] = data
head移动
```

读数据：

```text
data = buffer[tail]
tail移动
```

到数组尾部以后重新回到 0。

所以：

> 数据尽量不搬，只移动 Head 和 Tail。

---

# 68. Ring Buffer 为什么比普通线性 Buffer 更适合串口？ ★★★★★

普通线性 Buffer 使用到末尾后，如果前面已经有空间，可能需要移动数据。

Ring Buffer 不需要整体搬移数据。

它通过：

```text
Head回绕
Tail回绕
```

重复利用数组空间。

所以非常适合：

```text
UART连续接收
异步数据流
生产者-消费者模型
```

---

# 69. 如何判断 Ring Buffer 空和满？ ★★★★★

具体实现有多种。

一种常见方式是：

```text
head == tail
→ Empty
```

为了区分 Full 和 Empty，预留一个空位置。

例如：

```text
next_head == tail
→ Full
```

其中：

```c
next_head = (head + 1) % BUFFER_SIZE;
```

也可以额外维护：

```text
count
```

来区分空和满。

---

# 70. 为什么你的项目 ISR 只把数据放进 Ring Buffer？ ★★★★★

因为 Flash 写入和完整协议解析都比较耗时。

如果直接在 USART ISR 中：

```text
解析整个数据包
↓
计算CRC
↓
擦写Flash
```

ISR 时间会非常长。

我的架构是：

```text
USART ISR
↓
快速把Byte放进Ring Buffer
↓
退出

主循环
↓
从Ring Buffer取数据
↓
状态机解析协议
↓
CRC
↓
Flash写入
```

这样能够降低中断处理时间，并解耦：

```text
串口接收速率
和
固件处理速率
```

---

# 71. 什么是数据帧？ ★★★★★

串口底层只会连续收到 Byte。

它不知道：

```text
哪里开始
哪里结束
是什么命令
数据有多长
有没有错误
```

所以需要人为定义数据帧。

我项目的帧结构可以概括为：

```text
Frame Header
+
Opcode
+
Length
+
Payload
+
CRC16
```

这样接收端就能按照协议恢复一条完整消息。

---

# 72. 帧头有什么作用？ ★★★★★

帧头用于：

> 帮助接收端找到一条新数据帧的起始位置。

如果通信中出现：

```text
丢Byte
错误Byte
上电时随机数据
```

Parser 可以重新寻找合法帧头，从而恢复同步。

---

# 73. Length 有什么作用？ ★★★★★

Length 告诉 Parser：

> 后面的 Payload 有多少 Byte。

所以：

```text
读到 Length
↓
知道还需要接收多少Payload
↓
知道什么时候进入CRC阶段
```

同时 Length 也需要做边界检查。

例如：

```text
Length > 最大允许Payload
```

就应该认为数据非法，避免 Buffer 越界。

---

# 74. CRC16 和 CRC32 在你的项目中分别干什么？ ★★★★★

我的设计里使用两个不同层次的 CRC。

### CRC16

用于：

```text
单个通信数据帧
```

目的：

> 判断串口传输过程中这一帧是否损坏。

### CRC32

用于：

```text
整个APP固件
```

目的：

> 判断最终写入 Flash 的完整固件是否和预期一致。

所以可以理解成：

```text
CRC16
保护单帧运输

CRC32
验证完整固件
```

---

# 75. CRC 的原理是什么？ ★★★★★

CRC：

```text
Cyclic Redundancy Check
循环冗余校验
```

发送端根据原始数据和约定的生成多项式计算 CRC。

接收端对收到的数据重新计算。

如果：

```text
计算CRC == 帧中CRC
```

说明数据大概率没有发生传输误码。

如果不同：

```text
数据损坏
↓
丢弃或重传
```

---

# 76. CRC 能防止恶意篡改吗？ ★★★★★

不能。

CRC 主要解决：

```text
随机误码
数据损坏
传输完整性
```

因为攻击者如果知道 CRC 算法，也可以修改数据后重新计算 CRC。

真正需要安全认证时应该使用：

```text
数字签名
MAC
密码学Hash
安全启动
```

所以：

> CRC 是完整性错误检测，不等于密码学安全。

---

# 77. Magic Number 有什么作用？ ★★★★★

Magic Number 相当于固件的：

> 固定身份标识。

Bootloader 读取固件头后先检查：

```text
Magic Number
```

如果不符合预期，可以快速判断：

```text
这里不是合法APP固件
```

但是 Magic Number 本身不能证明固件内容完整。

所以我的项目使用：

```text
Magic Number
+
Firmware Length
+
CRC32
```

进行分级校验。

---

# 78. 为什么还需要 Firmware Length？ ★★★★

固件长度可以用于：

```text
确定CRC计算范围

确定Flash写入范围

检查是否超出APP分区

避免读取无效Flash区域
```

所以一个固件 Header 至少可以包含：

```text
Magic
Length
CRC
```

这比单纯判断某个 Flash 地址是否不是 `0xFFFFFFFF` 更可靠。

---

# 79. 你的五状态协议状态机是什么思路？ ★★★★★

我的串口协议一帧包含：

```text
帧头
操作码
数据长度
Payload
CRC
```

所以解析逻辑可以按五个阶段理解：

```text
等待帧头
↓
读取操作码
↓
读取长度
↓
接收Payload
↓
接收并验证CRC
```

每个 Byte 到来时只根据当前状态决定：

```text
这个Byte是什么
下一步进入哪个状态
```

如果出现错误：

```text
长度非法
CRC错误
超时
```

就丢弃当前帧并恢复初始状态。

---

# 80. 为什么使用状态机，而不是一次性解析？ ★★★★★

因为串口数据是：

> 一个 Byte 一个 Byte 异步到来的。

一次函数调用时不一定已经得到完整帧。

状态机可以保存：

```text
当前解析到哪里
还需要几个Byte
当前Frame状态
```

等下一批数据到来后继续处理。

这样：

```text
不需要阻塞等待
适合异步串口
异常恢复容易
```

---

# 81. 为什么设置 20 ms 字节接收超时？ ★★★★★

正常情况下，一帧中的 Byte 应该连续到达。

如果已经进入某个数据帧，但是长时间没有收到下一个 Byte，可能说明：

```text
通信中断
丢Byte
上位机异常
帧不完整
```

所以超过设定时间后：

```text
丢弃当前未完成帧
↓
Parser Reset
↓
重新寻找下一帧
```

防止 Parser 永远卡在半包状态。

---

# 第五章 Flash、Bootloader 与 IAP

# 82. 什么是 Bootloader？ ★★★★★

Bootloader 是 MCU 上电后优先运行的一段独立程序。

在我的项目中主要负责：

```text
检查是否进入升级模式

验证APP是否合法

接收新固件

擦除和写入Flash

验证新固件

跳转APP
```

Bootloader 和 APP 位于不同的 Flash 区域。

---

# 83. 什么是 IAP？ ★★★★★

IAP：

```text
In-Application Programming
在应用编程
```

核心思想是：

> MCU 自己运行一段程序，通过串口、网络等方式接收新固件，然后修改自身 Flash 中的 Application。

典型结构：

```text
Bootloader
+
APP
```

Bootloader 固定保留。

APP 可以升级。

---

# 84. ICP、ISP、IAP、OTA 有什么区别？ ★★★★

### ICP

通过：

```text
ST-Link
J-Link
SWD/JTAG
```

由外部烧录工具直接写 Flash。

### ISP

使用芯片厂商已经固化好的系统 Bootloader，通过 UART、USB 等接口烧录。

### IAP

使用：

```text
自己开发的Bootloader
```

实现在线升级。

### OTA

强调：

```text
Over The Air
无线传输新固件
```

OTA 往往可以使用 IAP 作为最终 Flash 更新机制。

---

# 85. STM32 Flash 为什么需要先擦除再写？ ★★★★★

Flash 的编程特性决定了：

> 编程通常只能把存储单元从擦除状态向另一个方向改变，而要恢复到原始擦除状态必须执行 Erase。

所以更新 Flash 的典型流程是：

```text
Unlock
↓
Erase Sector
↓
Program
↓
Verify
↓
Lock
```

不能像 SRAM 一样随便覆盖修改。

---

# 86. 为什么 Flash 要按 Sector 分区？ ★★★★★

Flash 的擦除最小单位通常不是单个 Byte，而是：

```text
Sector
```

所以如果 Bootloader、Metadata 和 APP 混在同一个需要擦除的区域：

> 升级 APP 时可能把 Bootloader 或元数据一起擦掉。

因此必须根据芯片 Flash Sector 布局进行合理分区。

---

# 87. 你的 Flash 为什么分 Bootloader、Metadata 和 APP？ ★★★★★

我的设计是：

```text
Bootloader区
负责升级和启动

Metadata区
保存Magic Number、Firmware Length、CRC32

APP区
保存真正的应用固件
```

这样把：

```text
升级程序
固件状态
应用程序
```

进行物理隔离。

更新 APP 时不会破坏 Bootloader。

---

# 88. APP 合法性怎么判断？ ★★★★★

我不会只判断 APP 地址是不是非空，而是分级检查。

首先：

```text
Magic Number
```

判断是否像一个合法固件。

然后：

```text
Firmware Length
```

检查长度是否合法、是否超出 APP 分区。

再计算：

```text
CRC32
```

判断整个固件是否完整。

跳转前还会检查：

```text
Initial MSP
Reset Handler
```

是否落在合理的 SRAM 和 APP Flash 地址范围。

---

# 89. 为什么要检查 APP 初始 MSP？ ★★★★★

APP 向量表第 0 项是：

```text
Initial MSP
```

正常情况下它应该指向合法 SRAM 区域。

如果这个值：

```text
明显不在RAM范围
```

说明：

```text
APP可能不存在
固件损坏
向量表错误
```

此时 Bootloader 不应该跳转。

---

# 90. 为什么要检查 APP Reset Handler 地址？ ★★★★★

APP 向量表第 1 项保存：

```text
Reset_Handler入口
```

正常情况下它应该指向 APP 自己合法的代码 Flash 区域，并符合 Cortex-M Thumb 函数入口要求。

如果地址完全不合法：

> 不能直接让 PC 跳过去，否则可能马上产生 Fault。

---

# 91. Bootloader 如何跳转 APP？ ★★★★★

整体流程可以背成：

```text
验证APP
↓
关闭或屏蔽相关中断
↓
停止SysTick
↓
清理Bootloader使用过的外设状态
↓
清除必要的Pending中断
↓
设置APP向量表位置
↓
读取APP Initial MSP
↓
读取APP Reset Handler
↓
设置MSP
↓
通过函数指针跳转Reset_Handler
```

之后：

```text
APP自己的启动代码
↓
main()
```

---

# 92. 为什么跳 APP 前要关闭中断？ ★★★★★

因为 Bootloader 使用的中断可能仍然处于：

```text
Enabled
Pending
```

状态。

如果刚刚跳进 APP 就发生 Bootloader 遗留下来的中断：

> CPU 可能在硬件尚未初始化完成时进入错误的 Handler。

所以交接前要尽量给 APP 一个干净的系统状态。

---

# 93. 为什么跳转前停止 SysTick？ ★★★★★

如果 Bootloader 开启了 SysTick：

> 即使已经开始跳转 APP，SysTick 仍然可能产生周期异常。

这可能在 APP 向量表、时钟或 RTOS 尚未完全建立之前进入异常。

因此跳转前通常停止 Bootloader 的 SysTick。

---

# 94. 为什么需要修改 VTOR？ ★★★★★

因为：

```text
Bootloader Vector Table
和
APP Vector Table
```

不在同一个地址。

进入 APP 后必须让 Cortex-M 使用：

```text
APP Vector Table
```

否则 APP 中发生：

```text
USART
SysTick
EXTI
```

中断时还可能进入 Bootloader Handler。

---

# 95. APP 工程为什么必须按照 APP 地址链接？ ★★★★★

程序编译链接后，很多：

```text
函数地址
常量地址
向量表地址
跳转目标
```

都基于链接地址生成。

如果 APP 实际存放在：

```text
某个Flash偏移地址
```

却仍按照：

```text
0x08000000
```

链接，那么程序中的地址关系可能错误。

因此 APP 的链接地址必须和实际 Flash 分区匹配。

---

# 96. 意外断电为什么危险？ ★★★★★

如果升级过程中：

```text
Flash擦除了
↓
只写了一半
↓
突然断电
```

APP 区域会处于：

```text
不完整固件
```

状态。

如果下次启动直接跳 APP，就可能死机。

所以我的 Bootloader 启动时会重新：

```text
Magic
Length
CRC32
```

验证。

验证失败：

```text
留在Bootloader
```

而不是跳转损坏 APP。

---

# 97. 你当前 Bootloader 还可以怎样进一步增强？ ★★★★

如果做成更完整的工业方案，我会进一步增加：

```text
A/B双分区

升级断点续传

版本管理

Rollback

数字签名

固件加密

升级包Hash

掉电保护状态机
```

当前项目主要解决：

```text
Bootloader/IAP基础流程
通信可靠性
完整性校验
安全跳转
异常恢复
```

---

# 第六章 RTOS 与 Zephyr

# 98. RTOS 是什么？ ★★★★★

RTOS：

```text
Real-Time Operating System
实时操作系统
```

核心不是：

> “运行速度一定特别快”

而是：

> 对事件响应和任务调度具有更强的确定性。

RTOS 可以把应用拆成多个独立 Thread：

```text
通信线程
控制线程
传感器线程
UI线程
```

再由调度器决定当前 CPU 执行哪个线程。

---

# 99. 一个 CPU 能真正同时运行多个线程吗？ ★★★★★

如果只有一个 CPU Core：

> 某一时刻实际上只能执行一个 Thread。

之所以看起来同时运行，是因为 RTOS 快速进行：

```text
Thread A
↓
Context Switch
↓
Thread B
↓
Context Switch
↓
Thread C
```

多个线程交替获得 CPU。

---

# 100. 什么是线程调度？ ★★★★★

调度器维护多个线程状态。

典型状态包括：

```text
Ready
准备运行

Running
正在运行

Blocked / Waiting
等待事件

Suspended
暂停
```

调度器从 Ready Thread 中按照：

```text
优先级
调度策略
```

选择下一个运行线程。

---

# 101. 什么是抢占式调度？ ★★★★★

如果一个高优先级线程进入 Ready 状态：

> 调度器可以暂停当前较低优先级线程，让高优先级线程先运行。

这就是抢占。

它适合：

```text
实时控制
通信响应
重要事件处理
```

但优先级设计不合理也可能导致：

```text
低优先级任务长期得不到运行
```

---

# 102. 什么是上下文切换？ ★★★★★

线程 A 正在运行时，CPU 中保存着：

```text
PC
SP
寄存器
状态信息
```

如果切换到 Thread B：

```text
保存A上下文
↓
加载B上下文
↓
恢复B的PC/SP/寄存器
↓
B继续运行
```

这就是：

```text
Context Switch
```

RTOS 多任务的核心之一就是：

> 保存和恢复不同线程的执行现场。

---

# 103. 什么是原子变量？ ★★★★★

原子变量用于进行：

> 不可被其他执行上下文观察到“中间状态”的基本操作。

例如两个执行上下文同时修改某个 Flag。

普通变量可能出现竞争。

使用：

```text
atomic_t
```

配合原子操作可以保证某些：

```text
Set
Clear
Test
Exchange
```

操作是不可分割的。

---

# 104. 为什么你的 BLE 项目用 `atomic_t`？ ★★★★★

因为 BLE 数据接收是异步发生的。

一边是：

```text
BLE Callback
```

可能修改颜色状态。

另一边是：

```text
主程序/线程
```

读取这个状态并决定后续动作。

如果同时访问共享状态，就存在：

```text
Race Condition
```

所以我使用 `atomic_t` 管理简单共享状态。

---

# 105. Atomic 能代替 Mutex 吗？ ★★★★★

不能完全代替。

Atomic 很适合：

```text
Flag
Counter
简单状态
```

但是如果要保护：

```text
复杂结构体
多个变量之间的一致关系
一大段临界区代码
```

通常还需要：

```text
Mutex
Spinlock
临界区
消息队列
```

等机制。

---

# 106. Semaphore 和 Mutex 有什么区别？ ★★★★

Semaphore 主要用于：

```text
事件同步
资源计数
```

Mutex 主要用于：

> 对一个共享资源进行互斥访问。

Mutex 通常具有更明确的：

```text
Owner
```

概念。

简单记：

```text
Semaphore
更像信号

Mutex
更像锁
```

---

# 107. 什么是看门狗？ ★★★★★

Watchdog 是一个独立运行的计时机制。

程序正常运行时需要定期：

```text
Feed / Reload Watchdog
```

如果软件：

```text
死循环
线程阻塞
通信异常导致系统卡死
```

长时间没有喂狗：

```text
Watchdog Timeout
↓
硬件Reset
```

从而提高设备长期运行可靠性。

---

# 108. 为什么不能无脑喂狗？ ★★★★★

如果在一个完全独立的定时中断里无条件喂狗：

> 即使业务线程已经死掉，看门狗仍然一直被喂，就失去了检测系统故障的意义。

更合理的方案是：

```text
检查关键任务是否正常
↓
全部正常
↓
Feed Watchdog
```

所以看门狗应该反映真正系统健康状态。

---

# 109. Zephyr Device Tree 是什么？ ★★★★★

Device Tree 用来描述：

```text
板级硬件
外设地址
GPIO
IRQ
时钟
设备连接关系
```

核心思想是：

> 把“硬件是什么、连在哪里”从驱动业务代码中分离出来。

例如驱动代码不需要写死：

```text
某一个固定GPIO编号
```

而可以根据 Device Tree 获取硬件配置。

Zephyr 会在构建阶段根据 Device Tree 生成相应的编译期硬件描述信息。

---

# 110. Device Tree 有什么好处？ ★★★★

主要是：

```text
硬件和软件解耦

同一个Driver适配不同Board

减少硬编码

统一管理外设资源
```

换硬件时：

> 很多时候只需要修改 DTS/Overlay，而不是大量修改 Driver 源代码。

---

# 111. Zephyr Shell 是什么？ ★★★★★

Zephyr Shell 是一个命令行调试框架。

可以注册：

```text
自定义命令
```

然后通过串口等终端执行。

例如我的项目实现：

```text
color R
color G
color B
color U
```

直接模拟颜色识别结果。

这样可以：

> 不依赖摄像头识别模块，独立测试 BLE 通信和下游控制逻辑。

---

# 第七章 BLE、HTTP 与机器视觉项目

# 112. BLE 和经典蓝牙有什么区别？ ★★★★★

BLE：

```text
Bluetooth Low Energy
低功耗蓝牙
```

主要面向：

```text
低功耗
小数据量
传感器
IoT
可穿戴设备
```

BLE 支持：

```text
Advertising
Scanning
Connection
GATT
```

等机制。

我的项目主要利用：

```text
Advertising + Scanning
```

传输颜色控制指令。

---

# 113. 什么是 BLE Advertising？ ★★★★★

Advertising 是 BLE 设备主动广播数据的一种机制。

它不要求一定先建立连接。

广播端：

```text
周期性发送Advertising Packet
```

扫描端：

```text
Scan
↓
收到广播
↓
解析Advertising Data
```

这种方式适合：

```text
数据量小
低延迟发现
无需持续连接
```

的状态通知。

---

# 114. 为什么你的项目使用广播，而不是连接？ ★★★★★

我的数据只有：

```text
R
G
B
U
```

这种很小的颜色状态指令。

不需要大量双向数据，也不需要保持持续连接。

所以广播方式：

```text
结构简单
无需维护连接状态
适合周期状态广播
```

对于这个项目更合适。

---

# 115. BLE 广播一定可靠到达吗？ ★★★★★

不一定。

广播属于：

```text
Connectionless
```

传输。

可能受到：

```text
无线干扰
扫描窗口
信道冲突
接收时机
```

影响。

所以同一个广播内容通常会重复发送。

重复发送又产生另一个问题：

> 接收端可能多次执行同一个命令。

因此我的项目又增加了序列号进行去重。

---

# 116. Manufacturer Specific Data 是什么？ ★★★★★

BLE Advertising Data 可以包含多个不同类型的数据字段。

Manufacturer Specific Data 用于：

> 厂商或者项目自己定义广播 Payload。

我的项目自定义了：

```text
Manufacturer ID

Project ID

Message Type

Color Command

Sequence Number
```

扫描端收到广播后根据这些字段：

```text
过滤是不是自己的设备
↓
确认消息类型
↓
获取颜色命令
```

---

# 117. 为什么还需要 Project ID？ ★★★★

因为只根据 Manufacturer ID 过滤范围可能太大。

增加：

```text
Project ID
```

可以进一步确认：

> 这条广播是否属于当前项目和当前协议。

这样可以避免把其他设备广播误认为自己的控制数据。

---

# 118. 递增序列号有什么作用？ ★★★★★

BLE 广播可能：

```text
同一内容重复发送
```

如果接收端每收到一次都控制舵机：

> 同一个颜色命令可能执行多次。

所以发送端加入：

```text
Sequence Number
```

接收端记录：

```text
last_seq
```

如果：

```text
new_seq == last_seq
```

说明很可能是重复广播。

直接忽略。

如果不同：

```text
认为是新的控制命令
```

---

# 119. Sequence Number 和 CRC 是一回事吗？ ★★★★

不是。

CRC：

> 检测数据是否损坏。

Sequence Number：

> 判断数据是不是新的，或者检测重复和顺序。

所以：

```text
CRC
数据完整性

SEQ
数据新旧和顺序
```

解决的是不同问题。

---

# 120. HTTP 是什么？ ★★★★

HTTP：

```text
HyperText Transfer Protocol
```

是应用层协议。

典型工作模式：

```text
Client Request
↓
Server Response
```

HTTP 通常运行在 TCP 之上。

我在 ESP32-CAM 项目里使用 HTTP 搭建实时图像流接口，主要作用是：

> 方便在浏览器观察摄像头图像，并实时调整 HSV 阈值和验证识别效果。

---

# 121. 为什么调试摄像头时搭建 HTTP 图像流？ ★★★★

因为单纯输出最终：

```text
R/G/B/U
```

无法直观看出算法为什么判断错误。

有实时图像以后，可以观察：

```text
光照变化
颜色区域
摄像头角度
曝光
背景干扰
```

再结合 HSV 阈值进行调试。

这样比只看日志更加高效。

---

# 122. RGB 和 HSV 有什么区别？ ★★★★★

RGB 从：

```text
Red
Green
Blue
```

三个颜色通道描述颜色。

但是颜色和亮度耦合比较明显。

HSV 使用：

```text
H
Hue 色相

S
Saturation 饱和度

V
Value 明度
```

描述颜色。

在颜色识别中：

> H 更直接表示“是什么颜色”，S 和 V 可以帮助排除灰色、过暗、过亮区域。

因此 HSV 通常比直接在 RGB 上设置固定阈值更方便进行颜色分割。

---

# 123. 为什么红色需要两个 Hue 区间？ ★★★★★

Hue 本质上是一个环。

红色正好位于 Hue 环的首尾交界位置。

所以红色可能同时分布在：

```text
Hue低端
和
Hue高端
```

因此如果只使用一个连续区间，会漏掉另一侧红色。

所以我的算法使用两个红色 Hue 阈值区间，再把结果合并。

---

# 124. 为什么使用像素区域统计？ ★★★★

单个像素很容易受到：

```text
噪声
反光
背景
压缩
传感器误差
```

影响。

所以我的方法不是：

> 只看到一个红色像素就判断为红色。

而是：

```text
统计满足HSV条件的像素数量/区域
↓
超过阈值
↓
判断目标颜色
```

能够减少孤立噪点引起的误判。

---

# 125. 你的视觉项目中为什么需要看门狗？ ★★★★★

整个系统包含：

```text
摄像头
图像处理
BLE
RTOS
舵机控制
```

属于长期连续运行系统。

如果出现：

```text
程序阻塞
通信异常
任务死锁
未知软件异常
```

系统可能停止工作。

所以配置约 4 秒硬件看门狗：

> 如果程序长期不能进入正常喂狗流程，硬件自动 Reset，提高连续运行能力。

---

# 第八章 调试工具

# 126. 示波器和逻辑分析仪有什么区别？ ★★★★★

示波器主要观察：

```text
真实模拟电压随时间变化
```

适合看：

```text
电压幅值
上升沿
下降沿
过冲
噪声
信号完整性
```

逻辑分析仪主要把信号理解成：

```text
0 / 1
```

特别适合：

```text
UART
SPI
I2C
PWM
```

的数字时序分析和协议解码。

简单记：

```text
示波器
看电气波形

逻辑分析仪
看数字时序和协议
```

---

# 127. UART 不通信你怎么排查？ ★★★★★

我的排查顺序通常是：

```text
1.检查供电和GND

2.检查TX/RX是否交叉

3.检查GPIO复用

4.检查USART时钟

5.检查波特率和8N1等配置

6.逻辑分析仪查看TX是否真的发送

7.检查RX是否收到波形

8.检查RXNE/中断/DMA配置

9.检查Buffer和协议Parser

10.使用日志进一步定位
```

应该先区分：

```text
硬件没有波形
还是
有波形但是软件没解析
```

---

# 128. I2C 不通信怎么排查？ ★★★★★

首先用逻辑分析仪看：

```text
START是否存在

SCL是否正常

Slave Address是否正确

R/W位是否正确

ACK是否出现
```

如果地址后没有 ACK，优先检查：

```text
设备地址

7-bit和8-bit地址混淆

供电

上拉电阻

接线

设备是否初始化
```

如果前面 ACK 正常而后面失败，再检查：

```text
寄存器地址
Repeated START
读写时序
```

---

# 129. SPI 不通信怎么排查？ ★★★★★

重点看：

```text
CS有没有正确拉低

SCK有没有波形

MOSI有没有输出

MISO有没有响应

CPOL / CPHA是否一致

数据位宽

MSB/LSB First
```

逻辑分析仪可以直接解码 SPI，快速判断：

> MCU 发出去的数据对不对，以及从机有没有返回。

---

# 130. 为什么日志调试很重要？ ★★★★

很多问题不是硬件完全不工作，而是：

```text
状态错误
时序错误
数据格式错误
状态机走错
异步事件顺序错误
```

日志可以记录：

```text
当前状态
错误码
时间
收到的数据
任务运行情况
```

帮助把“偶发问题”转换成可分析的数据。

---

# 第九章 了解 C++——只背基础，不主动展开

# 131. C 和 C++ 最大区别是什么？ ★★★

C 更偏：

```text
过程式编程
```

C++ 在 C 的基础上增加了：

```text
类
对象
封装
继承
多态
模板
STL
RAII
```

嵌入式中两者都可以使用。

对资源非常有限或者底层驱动场景，C 仍然非常常见。

---

# 132. `malloc/free` 和 `new/delete` 有什么区别？ ★★★

`malloc/free` 是 C 风格的动态内存管理函数。

`malloc()`：

```text
按Byte申请
返回void*
不调用构造函数
```

`new/delete` 是 C++ 运算符。

`new`：

```text
根据类型申请
返回对应类型指针
对象类型会执行构造过程
```

`delete` 对象时会执行析构过程。

因为我的主要嵌入式项目使用 C，所以这一部分只需要掌握基本区别。

---

# 133. 指针和引用有什么区别？ ★★★

指针本身是一个保存地址的变量：

```c
int *p = &a;
```

引用是已有对象的别名：

```cpp
int &r = a;
```

指针：

```text
可以为空
可以修改指向
访问对象需要解引用
```

引用：

```text
声明时必须绑定对象
一般不能再改绑其他对象
使用方式和原对象接近
```

---

# 第十章 两个项目必须会说的综合题

# 134. 用一分钟介绍你的 Bootloader 项目。 ★★★★★

我的项目是基于 STM32F407 实现的一套 Bootloader 与 IAP 在线升级系统。

首先，我按照内部 Flash 的 Sector 对空间进行分区，把 Bootloader、固件 Metadata 和 APP 隔离。升级数据通过 USART 接收，我没有在中断中直接解析和写 Flash，而是使用 USART 接收中断加 Ring Buffer 构建异步接收结构，中断只负责数据入队，主循环负责状态机解析和 Flash 写入。

通信协议采用类 Modbus RTU 的自定义帧格式，包括帧头、操作码、数据长度、Payload 和 CRC16，并使用五状态解析状态机以及 20 ms 字节超时进行异常恢复。

固件下载完成后，通过 Magic Number、固件长度和 CRC32 验证 APP 完整性。正常启动时还会校验 APP 的初始 MSP 和 Reset Handler，跳转前关闭中断和 SysTick、清理外设状态、重定位向量表，然后设置 MSP 并进入 APP Reset Handler。

项目还支持 APP 校验失败、物理按键以及 3 秒上电窗口三种升级触发方式。

---

# 135. 这个项目最核心的设计点是什么？ ★★★★★

我认为主要有四个：

```text
第一
Flash分区
保证Bootloader和APP隔离

第二
ISR + Ring Buffer
实现异步接收

第三
状态机 + CRC16 + Timeout
保证通信协议健壮性

第四
Magic + Length + CRC32 + MSP/Reset Vector校验
保证不会轻易跳入损坏APP
```

最后再通过：

```text
VTOR + MSP + Reset Handler
```

完成可靠的软件交接。

---

# 136. 为什么不用中断里直接写 Flash？ ★★★★★

Flash 写入甚至擦除耗时远高于普通 RAM 操作。

如果在串口 ISR 中执行：

```text
协议解析
CRC
Flash Erase
Flash Program
```

ISR 会长时间占用 CPU。

可能造成：

```text
后续UART数据丢失
其他中断延迟
实时性变差
```

所以我让 ISR 只负责快速入 Ring Buffer，复杂操作放到主循环。

---

# 137. 你的 Bootloader 项目最可能出现什么 Bug？ ★★★★★

我重点考虑的异常包括：

```text
串口丢Byte

错误帧头

非法Length

半包

CRC错误

Ring Buffer溢出

升级过程中断电

APP损坏

Bootloader遗留中断

VTOR没有重定位
```

对应措施分别包括：

```text
状态机

边界检查

20ms超时

CRC16

Ring Buffer

CRC32

APP地址校验

中断清理

VTOR重定位
```

---

# 138. 用一分钟介绍机器视觉分拣项目。 ★★★★★

项目面向不同颜色物体自动分拣。

整个系统分成两个节点。

ESP32-CAM 负责摄像头初始化、图像采集和 HSV 颜色识别，我根据像素区域统计判断红、绿、蓝以及未知状态，同时搭建 HTTP 实时图像流用于观察识别效果和调节阈值。

识别结果通过 BLE Manufacturer Specific Data 广播，其中包括 Manufacturer ID、Project ID、Message Type、颜色命令和递增序列号。

nRF52840 使用 Zephyr RTOS 扫描 BLE 广播并过滤目标设备，解析 Manufacturer Specific Data 后控制后续执行逻辑。

为了防止广播重复导致同一个动作重复执行，我加入递增序列号去重；异步 BLE 回调和主程序之间的简单共享状态使用 `atomic_t` 管理，同时配置硬件看门狗提高长期运行可靠性，并使用 Zephyr Shell 实现独立模块调试。

---

# 139. 你在视觉项目中主要负责什么？ ★★★★★

我主要负责：

```text
ESP32-CAM摄像头初始化

HTTP图像流

HSV颜色识别算法

BLE自定义数据帧设计

ESP32-CAM广播发送

nRF52840扫描过滤与解析

atomic_t共享状态

Watchdog

Zephyr Shell调试

系统联调
```

机械部分的双轴云台和舵机驱动主要由团队成员负责。

---

# 第十一章 科研项目防追问

# 140. NRZ 和 QPSK 有什么区别？ ★★★

NRZ 是一种比较简单的强度调制数字传输方式，实现结构相对简单。

QPSK：

```text
Quadrature Phase Shift Keying
```

利用多个相位状态携带信息，一个 Symbol 可以表达更多 Bit 信息。

因此在较高传输速率和较复杂链路中，QPSK 通常具有更好的频谱利用能力和系统性能，但实现复杂度也更高。

---

# 141. BER 是什么？ ★★★

BER：

```text
Bit Error Rate
误码率
```

表示：

```text
错误Bit数量
/
总传输Bit数量
```

BER 越低，说明数字通信链路质量越好。

---

# 142. Q Factor 是什么？ ★★★

Q Factor 可以用于衡量接收数字信号中：

> 逻辑 1 和逻辑 0 的可区分程度。

一般来说：

```text
Q越高
↓
两个电平分布越容易区分
↓
BER通常越低
```

所以它是评价光通信系统性能的重要指标之一。

---

# 143. 眼图怎么看？ ★★★

眼图是把很多数字信号码元叠加到一个时间窗口中。

通常：

```text
眼睛越开
↓
噪声和码间干扰越小
↓
系统质量越好
```

如果眼图逐渐闭合，通常说明：

```text
色散
噪声
带宽限制
ISI
```

等问题在增加。

---

# 144. 为什么距离增加性能变差？ ★★★

光纤距离增加以后：

```text
光纤损耗增加
↓
接收功率下降
```

同时：

```text
色散累积
↓
脉冲展宽
↓
码间干扰增加
```

因此：

```text
BER上升
Q Factor下降
眼图闭合
```

---

# 145. 为什么 QPSK 在中长距离表现更好？ ★★★

在达到相同比特率时，QPSK 每个 Symbol 可以携带更多信息，因此所需 Symbol Rate 可以更低。

Symbol Rate 降低后：

> 单个符号持续时间更长，对色散引起的脉冲展宽通常具有更高容忍度。

因此在我的仿真条件下，QPSK 在中长距离和较高比特率场景中表现出更好的抗色散能力。

---

# 第十二章 面试前最后必须记住的主线

如果面试官从你的简历开始追问，最可能形成下面几条路线。

## 主线一：C语言

```text
static
↓
extern
↓
const
↓
volatile
↓
指针
↓
函数指针
↓
回调
↓
结构体
↓
内存对齐
↓
Stack / Heap
↓
.data / .bss
```

## 主线二：STM32

```text
STM32F407
↓
Cortex-M4
↓
SP / LR / PC
↓
Vector Table
↓
Reset_Handler
↓
NVIC
↓
Interrupt
↓
SysTick
↓
VTOR
```

## 主线三：外设

```text
GPIO
↓
USART
↓
UART Frame
↓
I2C ACK/NACK
↓
SPI CPOL/CPHA
↓
DMA
↓
PWM
↓
ADC
```

## 主线四：Bootloader

```text
Flash
↓
Sector
↓
Bootloader / APP
↓
IAP
↓
Ring Buffer
↓
State Machine
↓
CRC16
↓
Magic Number
↓
CRC32
↓
MSP
↓
Reset Handler
↓
VTOR
↓
APP Jump
```

## 主线五：Zephyr / BLE

```text
RTOS
↓
Thread
↓
Scheduler
↓
Context Switch
↓
Atomic
↓
Watchdog
↓
Device Tree
↓
Shell
↓
BLE Advertising
↓
Manufacturer Specific Data
↓
Sequence Number
```

---

# 暂时不作为主背内容

根据当前简历，下面这些知识不要投入和 STM32/Bootloader 同等级别的时间：

```text
Linux进程和线程源码级原理

fork / exec

select / poll / epoll

Linux字符设备驱动

U-Boot源码

虚拟内存页表

MESI Cache一致性

C++虚函数表

复杂继承

STL内部实现

智能指针源码

右值引用和完美转发

TCP拥塞控制

TCP TIME_WAIT

红黑树

复杂动态规划
```

不是说这些永远不会考，而是：

> 当前简历没有把面试官主动引向这些领域。

秋招有限时间里，应优先把自己简历上的每一个技术关键词做到能够连续追问 3～5 层。
