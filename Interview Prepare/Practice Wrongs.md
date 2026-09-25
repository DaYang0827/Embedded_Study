# 1 Errors
## 1.1 `(*p)++`（❌）

```
int a[] = {5, 10, 15};

int *p = a;

printf("%d\n", (*p)++);
printf("%d\n", *p);
printf("%d\n", a[0]);
```

问三个输出分别是多少。

然后解释：

```
*p++
```

和

```
(*p)++
```

到底有什么区别。
```text
printf("%d\n", (*p)++);   5
printf("%d\n", *p);    6
printf("%d\n", a[0]);   6

*p++是*(p++) 但是是先取出地址中的内容，指针再自增
(*p)++       取出地址中的内容，再把对应的内容加1
```


## 1.2 数组下标本质（❌）

```
int a[] = {11, 22, 33, 44};

int *p = a;

printf("%d\n", p[2]);
printf("%d\n", *(p + 2));
printf("%d\n", a[2]);
```

问：

三个输出是否相同？

并解释为什么：

```
p[2]
```

本质上可以写成什么？

```text
add                 
0x1000     0x0B    
0x1001     0x00
0x1002     0x00
0x1003     0x00

add
0x1004     0x16
0x1005     0x00
0x1006     0x00
0x1007     0x00

printf("%d\n", p[2]);   33
printf("%d\n", *(p + 2));    33
printf("%d\n", a[2]);    33
```


## 1.3 函数里修改指针本身（❗）

```
void change(int *p)
{
    int b = 20;
    p = &b;
}

int main()
{
    int a = 10;
    int *p = &a;

    change(p);

    printf("%d\n", *p);
}
```

输出：

```
?
```

最重要的问题：

为什么：

```
p = &b;
```

没有修改 `main()` 里的 `p`？
```text
输出20    ？有点疑问

==输出应该为10==

```

## 1.4 带数据的 UART callback（❌）

现在改成：

```c
void (*uart_callback)(uint8_t data) = NULL;
```

要求：

UART 每收到一个字节：

```c
UART_IRQHandler()
```

就调用 callback，并把收到的数据传出去。

请补全：

```c
uint8_t UART_DR;

void UART_RegisterCallback(____________________)
{
    uart_callback = cb;
}

void UART_IRQHandler(void)
{
    uint8_t data = UART_DR;

    if (uart_callback != NULL)
    {
        ____________________;
    }
}

void app_uart_receive(uint8_t data)
{
    printf("%c\n", data);
}
```

然后在 `main()` 中注册：

```
_____________________________;
```

```c
uint8_t UART_DR;

void (*uart_callback)(uint8_t data) = NULL;

void UART_RegisterCallback(void (*cb)(uint8_t)) //注意这里的问题
{
    uart_callback = cb;
}

void UART_IRQHandler(void)
{
    uint8_t data = UART_DR;

    if (uart_callback != NULL)
    {
        uart_callback(data);
    }
}

void app_uart_receive(uint8_t data)
{
    printf("%c\n", data);
}

int main(void)
{
    UART_RegisterCallback(app_uart_receive);
}
```
## 1.5 typedef callback（❗）

解释下面代码：

```c
typedef void (*UART_Callback_t)(uint8_t data);
```

然后：

```c
UART_Callback_t uart_callback;
```

回答：

1. `UART_Callback_t` 是什么？
2. 它是不是一个函数？
3. 它是不是一个指针变量？
4. `uart_callback` 又是什么？
5. 为什么工程中更喜欢这种写法，而不是每次写：

```
void (*uart_callback)(uint8_t data);
```

```text
1. `UART_Callback_t` 是什么？

函数指针类型的别名

2. 它是不是函数？

不是。

3. 它是不是一个指针变量？

也不是。它是**类型**。

4. `uart_callback` 是什么？

这里：UART_Callback_t uart_callback;才是在定义变量。

所以：
UART_Callback_t
↓
类型

uart_callback
↓
变量

而且这个变量类型恰好是：函数指针类型   因此：uart_callback 是函数指针变量

5.为什么工程中使用 typedef？

核心不是“多次赋值”，而是**声明简单、可读性更好、维护方便**。

比如没有 typedef：
void RegisterA(void (*cb)(uint8_t));
void RegisterB(void (*cb)(uint8_t));
void RegisterC(void (*cb)(uint8_t));
很乱。

typedef 后：
typedef void (*UART_Callback_t)(uint8_t);

就可以：
void RegisterA(UART_Callback_t cb);
void RegisterB(UART_Callback_t cb);
void RegisterC(UART_Callback_t cb);
明显清楚很多
```


## 1.6 手写一个 callback 系统(❌)

不看前面的代码，自己写一个程序，实现：

有一个函数：

```c
button_register_callback(...)
```

用于注册按键 callback。

然后有：

```c
button_irq_handler()
```

模拟按键中断。

应用层有：

```c
void button_pressed(void)
{
    printf("Button Pressed!\n");
}
```

最终：

```c
int main(void)
{
    button_register_callback(button_pressed);

    button_irq_handler();

    return 0;
}
```

要求你自己定义：

- callback 类型
- callback 变量
- register 函数
- IRQ handler
- NULL 判断

这题能独立写出来，说明 callback 基本入门了。
```c
#include <stdio.h>

typedef void (*Callback_t)(void);

Callback_t button_callback = NULL;

void button_register_callback(Callback_t cb)
{
    button_callback = cb;
}

void button_irq_handler(void)
{
    if (button_callback != NULL)   //这里需要进行NULL判断
    {
        button_callback();    //需要注意，在调用函数的时候后面需要加()
    }
}

void button_pressed(void)
{
    printf("Button Pressed!\n");
}

int main(void)
{
    button_register_callback(button_pressed);

    button_irq_handler();

    return 0;
}
```

## 1.7 区分Callback_t的相关命名（❗）

```c
typedef void (*Callback_t)(int);

void hello(int x)
{
    printf("%d\n", x);
}

Callback_t callback;
```

回答：

```
Callback_t 是什么？

callback 是什么？

hello 是什么？

callback 里面能够保存什么？
```

然后执行：

```
callback = hello;
```

此时：

```
callback 里面保存什么？
```

```text
Callback_t 是对函数指针的重命名    在使用中可以看做是int*

`callback` 是：**函数指针变量**   ‼️

hello 是定义的具体的函数    输入参数是int   没有输出
callback 里面能够保存输入参数是int   没有输出参数的函数名   也就是函数对应的地址
```

完整关系：

```
typedef void (*Callback_t)(int);
```

表示：

```
Callback_t
=
void (*)(int)
=
一种函数指针类型
```

然后：

```
Callback_t callback;
```

表示：

```
callback
=
一个函数指针变量
```

而：

```
void hello(int x)
```

这里：

```
hello
=
函数
```

所以：

```
callback = hello;
```

之后：

```
callback
    ↓ 保存
hello 函数的地址
```

这一点你要非常严格：

```
hello     → 函数

callback  → 函数指针变量

Callback_t → 函数指针类型
```

## 1.8 写头插（❌）

补全：

```
Node *insert_head(Node *head, int data)
{
    Node *new_node = malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return head;
    }

    new_node->data = data;

    ______________________;

    ______________________;

}
```

目标：

原来：

```
10 → 20 → NULL
```

插入5后：

```
5 → 10 → 20 → NULL
```

```text
错误写法
Node *insert_head(Node *head, int data)
{
    Node *new_node = malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return head;
    }

    new_node->data = data;

    new -> next = head -> next;

    head = new_node;
}
```

```c
正确写法：
Node *insert_head(Node *head, int data)
{
    Node *new_node = malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return head;
    }

    new_node->data = data;
    new_node->next = head;
    head = new_node;

    return head;
}
```

## 1.9 二级指针头插（❌）

```
void insert_head(Node **head, int data)
{
    Node *new_node = malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return;
    }

    new_node->data = data;

    new_node->next = ________;

    ________ = new_node;
}
```

主函数：

```
Node *head = NULL;

insert_head(&head, 10);
```

补全两个空。

然后解释：

为什么调用的时候是：

```
&head
```

而不是：

```
head
```

？

```text
（❌）
new_node->data = data;

    new_node->next = head -> next;

    **head = new_node;

调用的时候用的是&head   因为在函数中想要改变谁就要传入谁的地址    这里想要改变的是head所保存的地址   所以应该传入head本身的地址
```

```text
函数参数是：Node **head  因此：

- `head`是主函数中`head`变量的地址
- `*head`才是主函数中的头指针
- `**head`才是头节点本身

正确填法：
new_node->next = *head;
*head = new_node;

完整代码：
void insert_head(Node **head, int data)
{
    Node *new_node = malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return;
    }

    new_node->data = data;
    new_node->next = *head;
    *head = new_node;
}
```

## 1.10 数组名与取地址（❌）

```
int arr[5] = {1, 2, 3, 4, 5};
```

分别说明下面表达式的：

- 类型
- 表示的含义
- 数值上是否相同

```
arr
&arr[0]
&arr
arr + 1
&arr + 1
```

重点说明：为什么 `arr + 1` 和 `&arr + 1` 移动的字节数不同？
```text
arr   是数组名    在函数中可以退化成数组第一个元素的地址
&arr[0]   是数组第一个元素的地址
&arr   是数组地址     和&arr[0]是一样的   （❌）
arr + 1    对应的是2    即数组第二个元素
&arr + 1   如果arr的起始地址是0x1000    那这个语句结束之后地址应该是0x1001   而arr + 1对应的饿是0x1004     因为arr+1对应的是数组的移动     而&arr + 1对应的是地址数加一

```
 
 `&arr` 这个非常重要。类型不是 `int *`，而是：`int (*)[5]`读作： 指向“5 个 int 数组”的指针。
 
## 1.11 函数内 static（❌）

```
void parser(void)
{
    static uint8_t index = 0;

    index++;
}
```

回答：

`index` 的：

- 作用域是什么？
- 生命周期是什么？
- 一般存在哪个区域？
```text
作用域还是当前的parser函数范围内
生命周期是整个程序运行范围内
因为初始值为0   所以放在.bss里面
```

## 1.12 MAP 文件

MAP 文件是在

```
编译阶段
链接阶段
烧录阶段
```

中的哪个阶段产生？

为什么它能告诉你：

```
Reset_Handler 在哪里
main 在哪里
.data/.bss 在哪里
Stack 在哪里
```

**MAP 是链接阶段的产物**。

```
把整个流程记成：

.c
↓ 编译
.o

↓ 链接

.axf / .elf
.map
.bin / .hex

↓ 烧录

MCU Flash
```

所以 MAP 能告诉你：

```
Reset_Handler → 最终被链接到哪里
main → 最终被链接到哪里
.data → RAM什么地方
.bss → RAM什么地方
```

因为这些地址正是**链接器决定的**。


## 1.13 Bootloader流程排列（❌）

```
A. 设置 APP MSP
B. 检查 Reset_Handler
C. 读取 APP Vector Table
D. 设置 VTOR
E. 跳转 Reset_Handler
F. 检查 MSP
```

你认为正确顺序是什么？

```text
C. 读取 APP Vector Table
↓
F. 检查 MSP
↓
B. 检查 Reset_Handler
↓
D. 设置 VTOR
↓
A. 设置 APP MSP
↓
E. 跳转 Reset_Handler
```

## 1.14 `.data` 和 `.bss` 有什么区别？（❌）

请按嵌入式角度回答，不只说“一个初始化，一个没初始化”。
```text
.data：
运行地址在 RAM
初始值存储在 Flash
启动时 C runtime 从 Flash copy 到 RAM

.bss：
运行地址在 RAM
Flash 不需要保存一堆 0
启动时 C runtime 清零
```

## 1.15 Heap和 Stack 最大区别是什么？（❗）
```text
Stack：
自动管理
函数调用自动分配/释放
速度快
生命周期跟调用关系密切

Heap：
动态分配
malloc/free
程序员负责管理
生命周期可以跨函数
可能碎片化
```

## 1.16 MSP和PSP（❗）

在 Cortex-M 里分别通常在什么情况下使用？

```text
MSP = Main Stack Pointer
PSP = Process Stack Pointer

MSP：
复位后默认使用
异常/中断 Handler mode 使用
裸机程序通常主要使用 MSP

PSP：
通常用于 Thread mode
RTOS 常给不同任务使用 PSP
```

## 1.17 为什么 DMA 不能完全代替 RingBuffer？（❌）
```text
真正区别是：

DMA解决：
谁负责把数据从 USART DR 搬到 RAM？

RingBuffer解决：
生产速度和消费速度不一致时，数据怎么缓存和管理？

所以：

USART
↓
DMA负责搬到 dma_buffer
↓
CPU把新增区域推进 RingBuffer
↓
Parser慢慢消费
```

DMA 是**传输机制**。

RingBuffer 是**软件缓冲数据结构**。

它们不在一个层次，所以不能互相替代。

