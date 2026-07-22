# 1 C 程序与基础类型
## 1.1 C语言基本数据类型以及大小

|    类型名称     |   描述    |     通常大小 (字节)     |
| :---------: | :-----: | :---------------: |
|    char     |   字符型   |         1         |
|    short    |   短整型   |         2         |
|     int     |   整型    |    4 (32位/64位)    |
|    long     |   长整型   | 4 (32位) / 8 (64位) |
|  long long  |  长长整型   |         8         |
|    float    | 单精度浮点数  |         4         |
|   double    | 双精度浮点数  |         8         |
| long double | 扩展精度浮点数 |    8或16 (平台相关)    |

<img width="682" height="345" alt="image" src="https://github.com/user-attachments/assets/32853df2-7874-4b9c-b2f0-18ebe4813761" />

## 1.2 uint_16 和 int

uint_16代表unsigned 16即无符号的16位（2字节）

int 是个“变色龙” (可移植性问题)

这是最大的坑。**在 C 语言标准中，int 的长度是不固定的，它取决于编译器和 CPU 的位数**。在 51 单片机 / Arduino Uno (8位/16位处理器) 上：int 通常是 16位 (2字节)。在 STM32 (32位 ARM 处理器) 上：int 通常是 32位 (4字节)。在 电脑 (64位处理器) 上：int 也是 32位，但在某些古老或特殊的系统上可能不同。

后果： 如果你写了 int a; 并且假设它是 16 位的，当你把代码从 Arduino 移植到 STM32 时，内存占用突然翻倍了；反之，如果从 STM32 移植到 8 位机，原本能存 100000 的变量突然溢出了（因为 16 位最大只能存 32767）。

相比之下 **uint16_t： 不管你在什么 CPU 上，它永远、绝对、一定是 16 位 (2字节)**。这给底层驱动开发提供了绝对的安全感。

| 特性  |      int       |   uint16_t   |      评价       |
| :-: | :------------: | :----------: | :-----------: |
| 大小  | 看心情 (2字节或4字节)  |   固定 (2字节)   | uint16_t 更可靠  |
| 符号  |   有符号 (包含负数)   |  无符号 (全是正数)  |  硬件参数通常不需要负数  |
| 最大值 | 32767 (如果是16位) |    65535     | uint16_t 容量更大 |
| 用途  |  算个加减法，做个循环变量  | 配置寄存器，定义硬件接口 |     专业对口      |

## 1.3 Keyword（关键字）

关键字（keyword）是由 **C 语言标准预先规定、具有特殊语法含义的单词，不能作为变量名、函数名或结构体成员名**

在C90中有32个对应的关键字，之后还有C99以及C11的新增关键字
```text
auto        break      case        char        const       continue    default
dodouble    else       enum        extern      float       for         goto
if          int        long        register    return      short       signed
sizeof     static      struct      switch      typedef     union       unsigned
void       volatile    while
```

1. 数据相关的关键字
char、 int、 short、 long、 sighed、 unsigned、 float、 double、  void、 bool 、 complex（复数）、 imaginary（虚数）

2. 流程控制关键字
if、 else、 case 、switch、 default、 for、 while、 do、 break、 continue、 goto、 return

3. 自定义数据结构关键字
Struct、 union、 enum

4. 存储类别关键字
auto、 register、 static、 extern、 typedef

5. 类型限定相关关键字
const、 volatile、 restrict、 automic

## 1.4 Static

`static`在不同位置含义不同，主要有三种用途：

1. 局部静态变量：**延长生命周期**；

2. 文件级静态变量：**限制在当前 `.c` 文件**；

3. 静态函数：**限制函数只能在当前 `.c` 文件调用**。

### 1.4.1 局部变量中的static

普通局部变量：

```c
void test(void)
{    
	int count = 0;    
	count++;    
	printf("%d\n", count);
}
```

多次调用：

```c
test();   // 1
test();   // 1
test();   // 1
```

因为普通局部变量每次进入函数时都会重新创建和初始化。

改成：

```c
void test(void)
{    
	static int count = 0;    
	count++;    
	printf("%d\n", count);
}
```

多次调用：

```c
test();   // 1
test();   // 2
test();   // 3
```

此时 `count`：

- 只初始化一次；
- 函数结束后不会销毁；
- 下次调用继续保留原来的值；
- 只能在 `test()` 函数内部通过名字访问

对应的作用域和生命周期是不一样的

- **作用域**：只在 `test()` 函数内部；
- **生命周期**：整个程序运行期间。

也就是说，它一直存在，但并不是整个程序都能直接访问它

### 1.4.2 文件中的static

假设在 `uart.c` 中写：

```c
static uint8_t rx_buffer[128];
```

这里的 `static` 表示：

 `rx_buffer` 只属于 `uart.c` 这个编译单元，**其他 `.c` 文件不能通过 `extern` 正常访问它**。

这叫作**内部链接**，其他的文件不可以直接使用这个变量，这种设计更安全，因为其他模块无法随意修改 UART 模块内部状态。

### 1.4.3 静态函数
```c
static void uart_gpio_init(void){}
```

表示这个函数只能在当前 `.c` 文件中使用。

如：

```c
// uart.c
static void uart_enable_clock(void)
{

}
static void uart_gpio_init(void)
{

}
void uart_init(void)
{    
	uart_enable_clock();    
	uart_gpio_init();
}
```

在这里：

```c
void uart_init(void);
```

是给外部模块使用的公共接口。

而：

```c
static void uart_gpio_init(void);
```

只是 `uart.c` 内部的辅助函数，不应该暴露出去。这样做可以防止其他文件错误调用，避免不同文件中同名函数发生冲突，让代码接口更加清晰


## 1.5 Const
`const`表示：**不允许通过当前变量、表达式或指针修改该对象**。

基本写法：

```c
const int value = 10;
```

下面不允许：

```c
value = 20;   // 编译错误
```

但也不是绝对不可以变化，

例如：

```c
const volatile uint32_t status;
```

程序不能修改 `status`，**但硬件可能改变它**。所以 `const`准确来说表示：**通过当前 C 语言表达式没有修改权限**。它不一定表示物理内存永远不变化。

### 1.5.1 const与指针
1.指向常量的指针
```c
const int *ptr;
```
表示：
- `ptr`可以指向其他地址；
- 不能通过 `ptr`修改它指向的数据
```c
int a = 10;  
int b = 20;  
  
const int *ptr = &a;  
  
ptr = &b; // 可以  
*ptr = 30; // 不可以
```
2.常量指针
```c
int * const ptr = &a;
```

表示：

- `ptr`保存的地址不能改变；
- 可以修改它指向的数据。

```c
*ptr = 30;   // 可以
ptr = &b;    // 不可以
```
3.指向常量的常量指针
```c
const int * const ptr = &a;
```

表示：

- 指针不能改变地址；
- 也不能通过指针修改数据。

```c
ptr = &b;    // 不可以
*ptr = 30;   // 不可以
```

|声明|指针能否换地址|能否通过指针修改数据|
|:---:|:---:|:---:|
|`int *ptr`|可以|可以|
|`const int *ptr`|可以|不可以|
|`int * const ptr`|不可以|可以|
|`const int * const ptr`|不可以|不可以|

判断方法是先找到变量名：

```c

const int * const ptr;
```

从 `ptr` 开始看：

```
ptr 是 const
ptr 是一个指针
指向 const int
```

## 1.6 Volatile

volatile表示：**该对象的值可能在当前代码无法直接观察到的地方发生变化，编译器不能假设它保持不变**。

典型修改来源：

- 硬件外设；
- 中断服务函数；
- DMA；
- 另一个CPU核心；
- 某些操作系统或底层执行环境。

volatile应用：

1.硬件寄存器

STM32外设寄存器的值会被硬件改变。

例如：

```c
#define USART1_SR \   
 (*(volatile uint32_t *)0x40011000U)
```

轮询发送寄存器空标志：

```c
while ((USART1_SR & USART_SR_TXE) == 0U){}
```

如果没有 `volatile`，编译器可能认为：

```c
USART1_SR
```

的值在循环中不会改变，从而只读取一次。但实际上是 UART 硬件在改变寄存器状态。

2.中断与主程序共享变量

```c
static volatile uint8_t rx_complete = 0;

void USART1_IRQHandler(void)
{    
	rx_complete = 1;
}
int main(void)
{    
	while (rx_complete == 0)    
	{    
	
	}    
	process_data();
}
```

`rx_complete`必须考虑使用 `volatile`，因为：

- 主程序读取；
- 中断异步修改；
- 主程序不能预测中断何时发生。

环形缓冲区示例

```c
static uint8_t rx_buffer[128];

static volatile uint16_t rx_head = 0;

static volatile uint16_t rx_tail = 0;
```

中断修改：

```c
void USART1_IRQHandler(void)
{    
	uint8_t data = (uint8_t)USART1->DR;    
	rx_buffer[rx_head] = data;    
	rx_head =  (rx_head + 1U) % sizeof(rx_buffer);
}
```

主程序读取：

```c
bool uart_read_byte(uint8_t *data)
{    
	if (rx_head == rx_tail)    
	{       
		 return false;    
	}    
	*data = rx_buffer[rx_tail];    
	rx_tail =  (rx_tail + 1U) % sizeof(rx_buffer);    
	return true;
}
```

这里：

- `rx_head`会被中断修改；
- `rx_tail`可能由主程序修改；
- 两者在不同执行上下文中被访问。

但需要注意，仅仅使用 `volatile`还不能自动保证整个算法线程安全。

3.DMA

```c
static volatile uint8_t dma_complete = 0;
```

DMA完成中断：

```c
void DMA2_Stream0_IRQHandler(void)
{    
	dma_complete = 1;
}
```

主程序：

```c
while (dma_complete == 0)
{

}
```

如果DMA直接修改一个缓冲区：

```c
uint8_t dma_buffer[512];
```

是否需要给整个缓冲区加 `volatile`，取决于访问方式和驱动设计。

更重要的是：

> `volatile`不能解决Cache一致性问题。

例如 STM32H7 使用 D-Cache 时，DMA写入RAM后，CPU可能仍然读到Cache里的旧数据。

这时需要：

- Cache clean；
- Cache invalidate；
- 合适的内存区域；
- 内存屏障；
- MPU配置。

仅仅写：

```c
volatile uint8_t dma_buffer[512];
```

不能自动解决Cache问题。

总结：
```c
static
```

控制的是：

> **存多久，以及能被哪些文件看到**。

```c
const
```

控制的是：

> **当前代码有没有修改权限**。

```c
volatile
```

控制的是：

> **编译器能不能假设这个值不会自行变化**。

最常见嵌入式组合：

```c
static volatile uint8_t flag;
```

当前**驱动文件私有、长期存在、可能被中断修改**。

```c
static const uint16_t table[];
```

当前**驱动文件私有、长期存在、只读查找表**。

```c
volatile const uint32_t status_reg;
```

软件只读，但硬件可能随时修改。

## 1.7 Enum （enumeration）（枚举）

### 1.7.1 定义
<img width="692" height="248" alt="image" src="https://github.com/user-attachments/assets/9cfeccf3-d08b-4929-ba9e-a7ceffd1709d" />

注意枚举变量中使用，进行分割的，并且最后一个变量末尾不用加，

示例：

```c
enum 
{
	monday = 1, 
	tuesday = 2, 
	wednesday = 3
} week;

week = monday;
```

意思是变量week只能取Monday，Tuesday，Wednesday不能取别的值，与typedef嵌套使用就是
```c
typedef enum
{
	monday = 1,
	tuesday = 2,
	wednesday = 3
} week_t;

week _t week ;

week = monday ;
```

`enum`（枚举）里写的那些名字，虽然看起来像“字符串”，但它们在被编译器编译之后，**在单片机的世界里本质上全部都是【整数】**

它的物理规律是：**从第一个名字开始，默认自动对应整数 `0`，往后依次递增 `1`**

标准定义语法：

```c
// 定义一个名叫 State 的枚举类型
enum State 
{
    STATE_IDLE,     // 👈 编译器自动把它替换成 0
    STATE_RUNNING,  // 👈 编译器自动把它替换成 1
    STATE_ERROR     // 👈 编译器自动把它替换成 2
};
```

如果不想从 0 开始，甚至可以手动给它指定数字：

```c
enum Speed 
{
    LOW_SPEED = 10,   // 手动指定为 10
    MID_SPEED = 20,   // 手动指定为 20
    HIGH_SPEED = 30   // 手动指定为 30
};
```

在`switch` 里面可以直接对它进行判断， **`switch` 语句在底层只认整数（`int`、`char` 等）**，它根本不认识任何字符串！在代码里看到的那些名字，在编译（Compilation）阶段，编译器就已经把它们全部“脱马甲”，露出了整数的真面目。

❌ 你眼睛看到的源代码：

```c
switch (current_state) 
{
    case STATE_IDLE:    // 看起来像字符串
        Do_Nothing();
        break;
    
    case STATE_RUNNING: // 看起来像字符串
        Do_Running();
        break;
}
```

⚙️ 单片机真正执行的物理代码（被编译器翻译后的真相）：

```c
switch (current_state) 
{
    case 0:    // 👈 真正的整数 0！
        Do_Nothing();
        break;
    case 1:    // 👈 真正的整数 1！
        Do_Running();
        break;
}
```

`switch` 在执行的时候，实际上只是拿 `current_state` 这个变量里的数字（比如 `1`），去跟下面 `case` 后面的数字（`0` 和 `1`）做快捷的数学比对。因为全都是纯整数的对比，所以 `switch` 的执行速度极快，甚至比一堆 `if-else` 还要快。

在定义的时候使用`,`进行分隔，**“逗号”是元素并列，“分号”是独立语句**

`enum` 的本质是**并列的列表元素**

在编译器的眼里，`enum`（枚举）内部是一组**紧密关联、同属一个家族、彼此并列的符号常量**。

- **逗号 `,` 的物理意义**：在 C 语言中，逗号专门用来**分隔同一个集合里的并列元素**。
- 这和我们定义数组、或者给函数传参是一样的逻辑。比如定义数组：`int a[] = {1, 2, 3};` 元素之间必须用逗号。
```c
enum Color {
    RED,    // 元素1,
    GREEN,  // 元素2,
    BLUE    // 元素3（最后这个逗号可写可不写）
};
```

### 1.7.2 在enum中赋值

比如 **W25Q128 Flash 芯片**，它手册里写死了各种控制指令的十六进制数字。  如果想用枚举来管理这些指令，就必须手动赋值，让名字和芯片硬件的寄存器指令**一一物理对齐**。

```c
enum W25Q128_CMD {
    CMD_WRITE_ENABLE      = 0x06,   // 👈 手动赋值：硬件规定 0x06 才是写使能
    CMD_WRITE_DISABLE     = 0x04,   // 👈 手动赋值：硬件规定 0x04 是写禁止
    CMD_READ_STATUS_REG1  = 0x05,   // 👈 手动赋值
    CMD_JEDEC_ID          = 0x9F    // 👈 手动赋值：刚才你代码里发的就是这个 0x9F！
};
```

- **如果不赋值会怎样？**  如果你不写 `= 0x06`，编译器就会把 `CMD_WRITE_ENABLE` 当成 `0` 编译发给 Flash。 Flash 收到 `0` 根本不知道你要干嘛，通信直接失败。

在 `enum` 里，**“不赋值”** 是让编译器帮你自动数数（省心）；**“赋值”** 是你拿着厂商的手册、或者带着特定的数学目的，去**强行绑架这个名字对应的物理底层数字**

在底层效果上，它们极其相似：无论是 `enum` 里的名字，还是 `#define` 定义的宏，编译后在单片机眼里**通通都会变成没有马甲的纯数字**。但它们在**物理管理、编译安全和调试便利性**上，有着本质的区别。简单来说：**`#define` 是无情的“文本复印机”（单打独斗），而 `enum` 是有组织的“编制管理所”（团队作战）**

在现代嵌入式开发中，行业内有一条不成文的规范：

1. **什么时候用 `enum`？** 只要数字之间有**关联性、属于同一个归类**（比如：状态机的状态、通信协议的错误码、星期几、车载屏幕的菜单项），**一律强制使用 `enum`**。
2. **什么时候用 `#define`？** 只有当数字是**孤立的、全局的常量配置**时才用（比如：定义板子的晶振频率 `#define CPU_FREQ 168000000`，或者定义一个数组的大小 `#define BUFFER_SIZE 1024`）。

## 1.8 int main (void) 函数

**这是 C 语言程序的入口**，也是整个程序的起跑线。简单来说：单片机上电复位后，干完必要的杂活（启动文件），就会直奔这里，开始执行你写的代码。

### 1.8.1 main：主角（函数名）
   
含义：**这是“主函数”的名字**。

地位：它是特殊的、唯一的。编译器在编译代码时，会满世界找叫 main 的函数。`main()`是C程序的主要入口，但通常不是芯片复位后执行的第一条代码。

注意：你不能随便给它改名（比如改成 start 或 begin），否则编译器会报错，因为它找不到“带头大哥”了。

对于函数，C 语言规定：**函数名本身，就直接代表它在内存中的首地址！**

典型 Cortex-M 单片机复位后的执行过程是：

```text
芯片复位
→ 从中断向量表读取初始栈指针
→ 执行 Reset_Handler
→ 初始化 .data 段
→ 清零 .bss 段
→ 执行系统和运行库初始化
→ 调用 main()
```

因此：

- `Reset_Handler` 通常是硬件复位后的入口；
- `main()` 是启动代码最终调用的 C 程序入口；
- 编译器不是简单地“把 main 设成芯片的第一条指令”。

### 1.8.2 void：参数列表

位置：括号里的 void。

含义：表示这个函数不接受任何参数。

通俗理解：这就像老板（系统）命令这个主函数去干活，但不需要给它任何额外的文件或指令。它启动就是了。

对比：在电脑编程（Windows/Linux）中，你可能见过 int main(int argc, char * argv[ ])，那是为了接收命令行参数（比如你运行程序时输入的命令）。但在嵌入式（STM32）里，通常没人给它传参，所以是 void。

### 1.8.3 int：返回类型

位置：最前面的 int。

含义：表示这个函数执行结束后，会返回一个整数 (integer) 给调用它的系统。

标准写法：**通常在函数的最后写一行 return 0;**

0：通常代表“程序成功执行，完美结束”。

**非0：代表“程序出错了”**。

### 1.8.4 举例

场景 A：电脑软件 (Windows/Linux)

流程：你双击软件 -> 软件运行 -> 你点击关闭 -> main 函数执行到最后 -> return 0; -> 告诉 Windows 系统“我关掉了，一切正常”。

int 的作用：真的有作用，用来给操作系统汇报工作状态。

场景 B：嵌入式系统 (STM32/Keil)

流程：单片机上电 -> 跳到 main -> 进入 while(1) 死循环 -> 永远在循环里跑。

int 的作用：完全是个摆设！因为单片机程序是不能“退出”的（退出了单片机就瘫痪了）。程序永远跑不到最后的 return 0; 那一行。

那为什么还要写 int？ 主要是为了符合 C 语言的标准语法规范。如果不写 int 而写 void main()，有些严格的编译器会发出警告（Warning），看着心烦，所以大家约定俗成写 int。

## 1.9 `,`和`;` 的使用区别

分号 `;` 是“断句符”，**用来终结一条独立的、需要 CPU 执行或编译器处理的语句**

逗号 `,` 是“连接符”，**用来把多个平级、并列的元素串联在同一条语句内**

### 1.9.1 分号 `;` 

真面目：无情的语句终结者

在编译器的底层逻辑中，分号就是**句号**。编译器在阅读你的代码时，是不见分号不回头的。只要没看到分号，它就会认为这一行和下一行是连在一起的。

核心使用场景：

1.**表达式语句的结尾**：任何计算、赋值、函数调用，只要是一个完整的动作，必须用分号结尾。
```c
current_state = STATE_RUNNING; // 👈 动作：赋值，结束
Control_LED(LED_ON);           // 👈 动作：调用函数，结束
```
2.**变量声明的结尾**：告诉编译器，关于这个（或这组）变量的类型和名字的划分，到此正式结束。

```c
int age;                       // 👈 声明结束，编译器在此开辟 4 字节空间
```

3.**结构体/联合体/枚举定义的末尾大括号后**：这是新手最容易漏掉的地方。大括号外面的分号，代表整个**类型的声明图纸设计完毕**。

```c
struct Point { int x; int y; };  // 👈 必须有分号，证明图纸画完了
```

### 1.9.2 逗号 `,` 
真面目：平级元素的粘合剂

逗号本身不需要被翻译成 CPU 的特定机器码，它在编译期主要用来告诉编译器：**“左右两边的东西是同属于一个家族、地位平等的元素，请一口气打包处理。”**

核心使用场景：

1.**同一类型下，声明多个并列变量**：

 ```c
 int a, b, c; // 👈 它们全都是 int 类型，用逗号隔开，最后用分号总收尾
 ```
 
2.**函数定义或调用时的“参数列表”**：
```c
// 定义函数时：参数1 和 参数2 是并列传入的
void Set_Motor(uint8_t speed, uint8_t direction); 
```
3.**大括号内部的“数据/常量初始化列表”**：  
当大括号内装的是具体的**数字、字符、符号常量**时，属于数据列表，必须用逗号。
```c
int arr[] = {10, 20, 30}; // 👈 数组数据列表
enum Color { RED, GREEN, BLUE }; // 👈 枚举常量列表
```

| 维度          | 分号 `;`（Statement Terminator）        | 逗号 `,`（Separator / Operator）            |
| :---: | :---: | :---:|
| **底层语意**    | “这件事情**做完了**，请推进到下一步。”              | “这两（多）个东西是**一伙的**，请一起看。”                |
| **对编译器的指令** | 截断当前语法树，开始解析新语句。                    | 继续在当前语句的上下文中，向后读取并列元素。                  |
| **书写数量限制**  | 一条有效语句的末尾**有且只能有一个**分号。             | 同一条语句内部可以有**无数个**逗号。                    |
| **经典翻车案例**  | 在 `if(a > b);` 后面误加分号，导致 `if` 直接失效。 | 在结构体内部定义 `int a, float b` 误用逗号导致编译直接崩溃。 |

# 2 指针与复合数据类型

## 2.1 Pointer 指针

### 2.1.1 指针(Pointer)

指针是C语言的一个重要知识点，其使用灵活、功能强大，是C语言的灵魂。**指针与底层硬件联系紧密，使用指针可操作数据的地址，实现数据的间接访问**。

指针即指针变量，**用于存放其他数据单元（变量/数组/结构体/函数等）的首地址**。若指针存放了某个数据单元的首地址，则这个指针指向了这个数据单元，若指针存放的值是0，则这个指针为空指针。

指针就是一张写着地址的小纸条，想象内存就是一个巨大的大酒店，里面有成千上万个房间。

变量 (int a = 100;)： 你在酒店开了一间房，房间里住着一个叫 100 的客人。为了方便，你给这个房间起个别名叫 a。地址 (&a)： 虽然你叫它 a，但在酒店管理系统（操作系统）里，它真正的名字是 房间号（比如 0x7ffee000 这种奇怪的十六进制数字）。

**指针变量，就是用来专门记房间号的笔记本**。

普通变量 (int a)：存的是数据（比如 100）。

指针变量 (int* p)：存的是地址（比如 0x7ffee000）。

比喻：a 是房间，p 是房卡（或者写着房间号的纸条）。

### 2.1.2 定义一个指针变量

|数据类型|	指向该数据类型的指针|
|:---:|:---:|
|(unsigned) char	1字节	|(unsigned) char *	x字节|
|(unsigned) short	2字节	|(unsigned) short *	x字节|
|(unsigned) int	4字节	|(unsigned) int *	x字节|
|(unsigned) long	4字节|	(unsigned) long *	x字节|
|float	4字节	|float *	x字节|
|double	8字节	|double *	x字节|

16位系统：x=2，32位系统：x=4，64位系统：x=8

**一个字节包含8个比特，在计算机存储中一个字节占一位的地址**

<img width="303" height="373" alt="image" src="https://github.com/user-attachments/assets/1bcdf8c3-17e3-49e9-809b-ba50b4a91c0f" />


### 2.1.3 指针的内存和步长

1)内存：

指针大小取决于目标平台。  
  
常见16位系统：2字节  

常见32位系统：4字节  

常见64位系统：8字节  

STM32F407：通常为4字节，

里面存储地址，因为现在的内存太大了，导致“门牌号”数字变得超级长，4 个字节写不下了。8 个字节，仅仅是为了完整地写下那一个“首地址”的编号。

地址本质上就是一个“编号”。想象一下，内存里的每一个字节都有一个编号（从 0 开始）。

如果你的内存很小（比如只有 256 字节）： 地址编号就是 0 到 255。 只需要 1 个字节（8 bit）就能存下任意一个地址。如果你的内存比较大（比如 4GB）： 地址编号可能高达 4,294,967,295。 这就需要 4 个字节（32 bit）才能存下这个数字。

现在的电脑内存非常大（比如 16GB, 32GB 甚至更多）： 内存地址的编号已经大到了天文数字，比如 17,179,869,184。 原来的 4 个字节已经不够用了(4 字节最大只能表示到 4GB 左右的地址)。 **所以，为了能覆盖到所有内存，我们被迫把地址长度加倍，变成了 8 个字节（64 bit）**。

结论： **那 8 个字节里，存的不是“8 个数据”，而是 “1 个超级长的数字”**。

假设有一个内存地址是 0x123456789ABCDEF0（这是一个超大的 16 进制数，刚好 8 个字节）当你把这个地址赋给指针 p 时，计算机不得不把它切成 8 段，分别塞进 8 个小格子里（内存单元）：（在排序的时候是倒着放进内存中的）

|内存偏移|	+0|		+1|		+2	|	+3|		+4	|	+5	|	+6|		+7|	
|:---: |:---: |	:---: |	:---: |	:---: |	:---: |	:---: |	:---: |	:---: |	
|存的数据|	F0|	DE	|BC|	9A|	78|	56|	34	|12|

取的时候：**合八为一**

当你使用 * p 或者 p++ 时，CPU 会发起一次读操作：

指令： “我要读指针 p 里的值。”

动作： CPU 伸出 8 只手，同时把这 8 个格子里的小数据抓出来。

拼装： CPU 内部的电路会瞬间把这 8 个小数据拼回去，还原成那个完整的数字：0x123456789ABCDEF0。

寻址： CPU 拿着在这个拼好的、巨大的数字，去内存海洋里找到了唯一的那个位置。

2)步长：

指针设计的初衷，不是为了按字节数数，而是为了按“个”数数。当写 p + 1 时，意思是：“我要找下一个数据”，而不是“我要找下一字节内存”

比喻：

char* (蚂蚁观察员)： 蚂蚁只有 1 厘米长。**他说“看下一个”，只需要往前挪 1 厘米**。

int* (青蛙观察员)： 青蛙有 4 厘米长。他说“看下一个”，如果不挪 4 厘米，就会只看到半只青蛙。所以他必须一步跨 4 厘米。

技术原理：**编译器在处理 p + 1 时，其实在背后做了一个乘法运算：目标地址 = 当前地址 + (1 ×该类型的大小)**

|   指针类型    | 自身大小(占位) |  含义  | p+1的结果(步长) |             解释             |
| :-------: | :------: | :--: | :--------: | :------------------------: |
|  char* p  |   8 字节   | 指向字符 | 1001 (+1)  |  因为字符只占 1 坑，跨 1 步就到下一个了。   |
|  int* p   |   8 字节   | 指向整数 | 1004 (+4)  | 因为整数占 4 坑，必须跨 4 步才能跳过当前整数。 |
| double* p |   8 字节   | 指向浮点 | 1008 (+8)  |       双精度很大，必须跨 8 步。       |

举例：
``` c
char name[] ="Da_Yang";
char* p = name;
int age = 21;
int i;
printf("名字:");
for (i = 0; i < 8; i++)
{
  printf ("%c"，*p);
  p++;
}
```

 `char* p = name`，**数组名等于第0个元素的地址**，赋给指针之后，指针的内部就会存储第0个元素的地址，`*p`就代表`D`
 
` p++` 代表指针增加一个步长，因为是char类型，所以代表当前地址+1，对应的 `* p`就应该表示`a`

### 2.1.4 指针的操作

若已定义：
```c
	int a;		//定义一个int型的数据
	int *p;		//定义一个指向int型数据的指针
```
则对指针p有如下的操作方式

| 操作方式 |     举例     |      解释       |
| :--: | :--------: | :-----------: |
| 取地址  |  P = &a;   | 将数据a的首地址赋值给p  |
| 取内容  |    * p;    |  取出指针指向的数据单元  |
|  加   |   P ++;    | 使指针向下移动1个数据宽度 |
|      | P = p + 5; | 使指针向下移动5个数据宽度 |
|  减   |   P --;    | 使指针向上移动1个数据宽度 |
|      | P = p - 5; | 使指针向上移动5个数据宽度 |

* * 在C语言中有三个功能
 
(1)A * B 代表两个数的相乘

(2)Int p  * 出现在变量类型后面（int char等），表示是int类型，代表是一个int类型的指针

(3) * p  在的前面没有变量类型，表示取内容，取出指针指向的数据单元

### 2.1.5 例子解析
```c
char a = 0x66;
char *p;

p = &a;

printf("%d\n",a);
printf("%d\n",p);
printf("%d\n",*p);
```

定义了两个char变量a和p，在计算机的存储中是

|     地址     | 内存  |
| :--------: | :-: |
| 0x147bf524 | 66  |
|            | 24  |
|            | F5  |
|            | 7b  |
|            | 14  |
|            |  0  |
|            |  0  |
|            |  0  |

char a=0x66;  表示定义了一个char 类型的数据a，a的值为66，同时他的地址为147bf524

char *p; 表示定义了一个char*类型的数据p，char*会占用8个字节的空间

p = &a;  表示将a的地址付给p，所以p的8个字节会按照低端的方式存入a的地址147bf524

printf("%x\n",a);  表示将a的值显示出来

printf("%x\n", p);  表示将p的值显示出来

printf("%x\n", p);  表示将p的值做为地址，找到这个地址对应的存储信息并显示出来

### 2.1.6 数组与指针

数组是一些相同数据类型的变量组成的集合，其数组名即为指向该数据类型的指针。数组的定义等效于申请内存、定义指针和初始化

<img width="515" height="197" alt="image" src="https://github.com/user-attachments/assets/8094735f-051b-4018-9880-9a4fcb8c8454" />


当你定义一个数组 int arr[5]; 时，内存里发生了两件事：1.分配空间： **系统划出了一块连续的内存（能存5个整数）**。2.生成标签： **arr 这个名字，其实就代表了这块内存首元素的地址**。数学公式：arr≡&arr[0](数组名等于第0个元素的地址)

平时用的 arr[2]，其实是写给人类看的简化写法。编译器在编译时，会把它无情地翻译成指针运算。翻译公式：arr[i]⇔∗(arr+i).arr:首地址（起点）。+ i：向后跳 i 步（回忆一下，因为是 int 数组，每一步跳 4 字节）。* (...)：解引用，取出那个地方的值。

这也是为什么数组下标从 0 开始：arr[0]→∗(arr+0) 起点往后跳 0 步 → 就是起点本身。如果从 1 开始，电脑就需要多做一次减法运算 * (arr + i - 1)，效率就低了。

### 2.1.7 注意事项

在对指针取内容之前，一定要确保指针指在了合法的位置，否则将会导致程序出现不可预知的错误

同级指针之间才能相互赋值，跨级赋值将会导致编译器报错或警告

<img width="515" height="197" alt="image" src="https://github.com/user-attachments/assets/c9792c80-c975-448e-95c4-33b1b0e8f238" />

### 2.1.8 指针的应用
(1)传递参数

**使用指针传递大容量的参数**，主函数和子函数使用的是同一套数据，避免了参数传递过程中的数据复制，提高了运行效率，减少了内存占用

使用指针传递输出参数，利用主函数和子函数使用同一套数据的特性，实现数据的返回，可实现多返回值函数的设计

(2)传递返回值

将模块内的公有部分返回，让主函数持有模块的“句柄”，便于程序对指定对象的操作

(3)直接访问物理地址下的数据

**访问硬件指定内存下的数据**，如设备ID号等

将复杂格式的数据转换为字节，方便通信与存储

### 2.1.9 示例
```c
int FindMax (int* array, int count)
{
	int i;
	int max = array[0];
	for(i=1 ; i<count ;i++)
	{
        if (array[i] > max)
		max=array[i];
    }
   return max;
}
int main(void)
{
	int a[] = {4,5,8,7,9,10,88,76 };
	int max = FindMax(a，8);

	printf("Max=%d \n"，max);
```

### 2.1.10 高级表达

```c
int* p＝arr
```

等价于`int* p = &arr[0];`

等价于`int*p;  p＝&arr [0];` 因为数组名本身就等于第0个元素的地址

### 2.1.11 用指针传参


## 2.2 Function pointer（函数指针）

在纯 C 语言（尤其是底层、嵌入式、驱动开发）中，使用函数指针的核心目的只有四个字：**动态调用**。

如果不用函数指针，C 语言的函数调用都是死板、硬编码（写死）的。而有了函数指针，**程序就可以在运行时根据不同的情况，动态地决定去执行哪段代码**,主要针对回调函数使用

### 2.2.1 什么是函数指针
普通指针保存的是**数据地址**：

```c
int value = 10;
int *ptr = &value;
```

函数指针保存的是**函数入口地址**：

```c
int add(int a, int b)
{    
	return a + b;
}

int (*func_ptr)(int, int) = add;
```

这里：

```c
func_ptr
```

保存了 `add()` 函数的地址，因此可以通过它调用 `add()`：

```c
int result = func_ptr(10, 20);
```

结果：

```c
result = 30
```

函数指针的核心作用是：

> **不把要调用的函数写死，而是在运行时决定调用哪个函数**。

### 2.2.2 函数指针声明

假设有函数：

```c
int add (int a, int b)
{    
	return a + b;
}
```

对应的函数指针必须满足：

- 返回类型相同；
- 参数数量相同；
- 每个参数类型兼容。

声明：

```c
int (*func_ptr)(int, int);
```

完整理解：

```
func_ptr 是一个指针
它指向一个函数
这个函数接收两个 int 
参数这个函数返回 int
```

### 2.2.3 函数指针的赋值

1.直接使用函数名

```c
int (*func_ptr)(int, int);

func_ptr = add;
```

**函数名在多数表达式中会自动转换成函数地址**。

2.使用取地址符

也可以写：

```c
func_ptr = &add;
```

这两种写法通常等价：

```c
func_ptr = add;

func_ptr = &add;
```

一般推荐更简洁的：

```c
func_ptr = add;
```

4.通过函数指针调用函数

两种写法都可以：

```c
int result = func_ptr(10, 20);
```

或者：

```c
int result = (*func_ptr)(10, 20);
```

两者效果相同。

通常使用：

```c
func_ptr(10, 20);
```

更简洁。

完整示例：

```c
#include <stdio.h>

int add (int a, int b)
{    
	return a + b;
}

int main(void)
{    
	int (*func_ptr)(int, int);    
	
	func_ptr = add;    
	
	int result = func_ptr(10, 20);    
	
	printf("%d\n", result);    
	
	return 0;
}
```

### 2.2.4 使用 typedef 简化函数指针

函数指针声明比较复杂：

```c
int (*func_ptr)(int, int);
```

可以使用 `typedef` 定义类型别名：

```c
typedef int (*Operation) (int, int);
```

这里的：

```c
Operation
```

代表一种函数指针类型：

> **指向“接收两个 `int`、返回 `int`”函数的指针类型**。

之后可以直接写：

```c
Operation func_ptr;
```

完整示例：

```c
#include <stdio.h>
typedef int (*Operation)(int, int);

int add(int a, int b)
{    
	return a + b;
}

int subtract(int a, int b)
{    
	return a - b;
}

int main(void)
{    
	Operation operation;    
	
	operation = add;    
	
	printf("%d\n", operation(10, 5));    
	
	operation = subtract;    
	
	printf("%d\n", operation(10, 5));    
	
	return 0;
}
```

- **没有 `typedef` 时**：系统认为 `func_ptr` 是一个**具体的收音机（变量）**，你可以直接给它赋值和使用。它只能用这一次。
- **带有 `typedef` 时**：系统不再创建具体的变量，而是把 `Operation` 变成了**这张收音机的“设计图纸”（类型别名）**。以后你可以用这张图纸，批量生产出成百上千个一模一样的收音机。

有没有typedef的对比：

1.如果没有 `typedef`：

每次想用这个指针，都要当场临时向编译器描述它的长相。

- **对编译器说**：“我要创建一个变量叫 `p1`，它是一个指针，指向一个‘返回值为 int，接收两个 int 参数’的函数。”
    - **代码**：`int (*p1)(int, int);`
- **过了一会儿又要用**：“我还要创建一个变量叫 `p2`，它是一个指针，指向一个‘返回值为 int，接收两个 int 参数’的函数。”
    - **代码**：`int (*p2)(int, int);`

2.如果使用了 `typedef`：

先给这种长相的函数录入一个“官方标准”，以后直接叫名字。

- **对编译器说**：“听好了，以后只要看到 `Operation` 这个词，它就代表‘指向一个返回值为 int，接收两个 int 参数的函数’的指针类型。这是一种**新类型**。”
    - **代码**：`typedef int (*Operation)(int, int);`
- **录入成功后，想创建变量**：“来一个 `Operation` 类型的变量，名字叫 `p1`。”
    - **代码**：`Operation p1;`
- **还想再来一个**：“再来一个 `Operation` 类型的变量，名字叫 `p2`。”
    - **代码**：`Operation p2;`

### 2.2.5 使用示例

1.不用函数指针，普通函数的做法（硬编码死板应用）

假设你写了一个底层的按键驱动文件 `button.c`。

- **规则**：底层驱动的代码一旦写好，**绝对不能轻易修改**。
- **痛点**：今天产品经理要求按键按下时“LED灯闪烁”，明天要求按键按下时“屏幕显示文字”。

如果没有函数指针，底层驱动代码里必须**直接调用**具体的普通函数：

```c
// button.c（底层驱动）
void check_button_status() {
    if (read_pin(1) == LOW) { // 检测到按键按下
        // ❌ 死板：必须在这里写死具体的普通函数名字
        blink_led(); 
    }
}
```


**致命缺陷**：如果明天要改成“屏幕显示”，你必须打开 `button.c`，把 `blink_led()` 删掉，改成 `show_screen()`。这违反了软件开发“对修改关闭，对扩展开放”的核心原则。


2.函数指针的做法（真正的回调应用）

现在，在底层驱动里换上**函数指针**。底层驱动只提供一个“空插座”，具体插什么由上层决定。

```c
// button.c（底层驱动）

// 1. 定义一个函数指针变量（空插座）
void (*on_button_press)(void) = NULL;

// 2. 提供一个注册接口（让上层把函数插进来）
void register_button_callback(void (*cb)(void)) 
{
  on_button_press = cb; // 把上层函数的地址记在小本子上
}

// 3. 底层循环检测
void check_button_status() 
{
    if (read_pin(1) == LOW) 
    {
        if (on_button_press != NULL) 
        {
            // 【重点】这里看起来和普通函数没区别，但它是顺着地址去调用的！
            on_button_press(); 
        }
    }
}
```

3.上层业务（随心所欲，底层代码一字不改）

有了上面的函数指针驱动，上层应用（比如 `main.c`）就可以随意玩转了：

- **应用场景 A：需要闪烁 LED**
    ```c
    void my_led_func(void) 
    { 
	    blink_led(); 
	} // 这是一个普通函数
    
    int main() 
    {
        // 注册回调：把普通函数的地址，交给了底层的函数指针
        register_button_callback(my_led_func); 
        while(1)
        { 
	         check_button_status(); 
	    }
    }
    ```
- **应用场景 B：改成了屏幕显示（底层 `button.c` 连一个标点符号都不用改！）**
    ```c
    void my_screen_func(void) 
    { 
	    show_screen(); 
	} // 另一个普通函数
    
    int main() {
        // 只需要在这里换个地址注册即可
        register_button_callback(my_screen_func); 
        while(1) 
        { 
	        check_button_status(); 
	    }
    }
    ```

## 2.3 Struct 结构体

<img width="692" height="326" alt="image" src="https://github.com/user-attachments/assets/badca57d-718b-4d63-905b-18e16ce95b33" />

完整的定义应该为：
```c
struct A
{
	int x;
	int y;
}B;
```

关于调用的理解：

(1)要造一个新的变量（比如说 C）

格式：struct A C;

既然 struct A 是类型（像 int 一样），那可以用它去定义新变量 C。这时，C 和 B 是两个独立的变量，互不干扰。

(2)我要用刚才顺手造出来的变量 B

格式：B. 来定义里面的数据

因为 B 已经是一个实实在在存在的变量了，所以你直接用 . 号就能访问它的成员。例如：B.x = 10

**Struct本质还是一个数据类型，只是它是多个数据类型的集合**

<img width="498" height="154" alt="image" src="https://github.com/user-attachments/assets/7683fc6c-8f34-462d-a7d2-466341bc8798" />


C语言的定义变量的语法为**数据类型+变量名**

|步骤|	使用 int (系统类型)|	使用 struct name (自定义类型)|
|:---:|:---:|:---:|
|公式	|类型 变量名;|	类型 变量名;|
|代码	|int a;	|struct name a;|
|含义	|定义一个叫 a 的整型变量	|定义一个叫 a 的结构体变量|

**将不同的数据类型进行打包**，在定义上和别的变量定义是相同的比如int a，都是先写变量类型再写变量名字，在结构体中最前面多了一个struct，并且变量由{ }表示出来

struct{char x; int y; float z;} 整体是一个变量，可以对这个整体使用typedef，起一个别名进行调用

示例：
```c
struct
{
	int a;
	char b;
	float c;
} z;
z.a = 10;
z.b = 'Y';
Z.c = 1.2;
printf("%d \n%c \n%f\n"， z.a，z.b，z.c);

```

在这个例子中，并没有struct的名字，z是自变量，直接用z.来写入变量。

注意在定义结构体时，**前面是变量后面是名字，要用{  }写入变量，且每个变量之间用；分开，在最后也要加上；**。在定义结构体时用结构体的名字加.再加变量的名字在定义结构体的具体变量

在实际应用的过程中，我们可能会想再定义一个结构体，用相同的结构体变量，如果直接定义还需要重新复制粘贴出来一个新的结构体，这时候我们**可以使用typedef来重新定义这个结构体变量**

```c
typedef struct
{
	int a;
	char b;
	float c;
} StructName_t;

StructName_t q;
q.a = l;
q.b = 'A’;
q.c = 2.4;

struct { int a; char b; float c; } z;
```
通过typedef，将整个结构体变量改名为StructName_t，q是自变量，调用的时候就是直接用StructName_t+自变量来定义一个规定的结构体 

在C语言中调用struct结构体，**不能只用他的名字调用，必须在前面加上struct前缀**，C 语言觉得 Student 只是这个结构体的“内部编号”（Tag）。如果不加 struct 前缀，C 语言会以为 Student 是一个变量名或者函数名，容易搞混

<img width="582" height="230" alt="image" src="https://github.com/user-attachments/assets/73801cec-010e-4018-b80b-af18a1b16167" />

<img width="572" height="140" alt="image" src="https://github.com/user-attachments/assets/f320e47c-31da-482a-a29c-b8714a6914d1" />

所以在编写的时候，为了方便可以少写一个struct，STM32用typedef对struct重命名，就可以通过重命名直接使用struct

在定义结构体的时候有三种不同的方式

| 写法  |             代码特征              |      Student/Xiaoming 是什么？      |      怎么定义变量？      | 怎么访问成员？      |
| :-: | :---------------------------: | :-----------------------------: | :---------------: | ------------ |
|  1  |     struct Student {...};     |          Type (类型/模具)           | struct Student a; | a.age        |
|  2  |    struct {... } Xiaoming;    |        Variable (变量/实物)         |     (它自己就是变量)     | Xiaoming.age |
|  3  | typedef struct {...} Student; |        Type Alias (类型别名)        |    Student a;     | a.age        |


### 2.3.1 Structure就是一个“快递包裹”

为什么要用结构体？ 想象你要描述一个学生：如果不用结构体：你需要定义 int age1, char name1[  ], float score1... 如果有100个学生，变量名会乱成一团。如果用结构体：你可以定义一个“学生”类型，把年龄、姓名、分数全部打包在一起。

代码示例：
``` c
//定义一个结构体类型，相当于设计了一个“表格模板”
struct Student
{
  char name[20];	//姓名
  int age;           //年龄
  float score;        //分数
};

//使用模板创建一个具体的变量（实例)
struct Student xiaoming;

//往里面存数据:使用点号(.)
xiaoming.age = 18;
xiaoming.score = 95.5;
```

### 2.3.2 结构体 + 指针：嵌入式的灵魂

(1)既然有结构体，为什么还要用“结构体指针”？

这就好比你要交作业给老师：

传值（不用指针）：你把作业本上的内容全部抄写一份给老师。如果作业有100页，这太慢了，还浪费纸（内存）。

传址（使用指针）：你直接告诉老师你的作业本放在哪张桌子上（给地址）。老师走过去直接看原件。这非常快！

**在嵌入式中，寄存器配置结构体往往很大，复制它们太浪费时间，所以我们几乎永远只传地址（指针）**。

(2)最重要的符号：箭头 ->

当你手里拿着一个结构体的指针时，如何访问里面的成员？

普通结构体变量用点 . （例如 xiaoming.age）

结构体指针变量用箭头 -> （例如 p->age）

|变量类型	|访问符号	|代码示例	|含义|
|:---:|:---:|:---:|:---:|
|实体变量|	. (点)|	Config.Mode = ...	|直接打开包裹拿东西|
|指针变量|	-> (箭头)|	Config->Mode = ...	|顺着地址找到包裹，再拿东西|

### 2.3.3 ADC1 -> DR

1. 物理层面的含义：**数据的“终点站”**

ADC (Worker)：是负责干活的模数转换器。它在后台默默地把电压变成数字。

-> (Flow)：代表“写入”或“传送”的动作。

DR (Data Register)：数据寄存器。这是 ADC 模块对外展示结果的唯一窗口。

“ADC -> DR” 的意思就是： ADC 硬件内部电路转换完电压后，自动把算出来的那个数字（比如 2048），搬运并存储到 DR 寄存器里去。

形象比喻：ADC 是厨房里的厨师。DR 是厨房门口的出餐台。ADC -> DR：就是厨师把做好的菜（数据），端到了出餐台上。

CPU（你）平时进不去厨房，你看不到厨师是怎么炒菜的。你只能去出餐台 (DR) 拿菜。

2. 代码层面的含义：**结构体指针**

在 STM32 的 C 语言编程中，-> 是结构体指针成员访问符。

->箭头操作符是一个二合一的超级快捷键，在其内部就已经包含了*的功能

A−>B≡(∗A).B

'* A：表示“解引用”，即取出 A 指向的那个结构体（进大门）。

. B：表示“成员访问”，即找到结构体里的 B 成员（进房间）。

ADC1 -> DR编码器会把他翻译为 （* ADC1）. DR   * ADC1：先用钥匙打开 ADC1 这个大楼的门（完成了你想要的“取内容”操作）。.DR：然后直接走到 DR 这个房间，拿走里面的数据。结论： 因为 -> 已经帮你把“开门（取内容）”这件事做了，所以不需要再手动加一个 *。

ADC1 是一个指针，指向 ADC1 外设在内存中的基地址（大本营）。DR 是这个结构体里的一个成员（偏移地址）。

当写代码 Value = ADC1->DR; 时，在对 CPU 说： “请去 ADC1 的大本营，找到那个叫 DR 的房间，把里面的数字读出来给我。”

### 2.3.4 结构体嵌套结构体

```c
typedef struct 
{ 
	DMA_Stream_TypeDef *DMA_Streamx;    // 👈 核心卡点：这只是一个“硬件的门牌号”！ 
	uint32_t ErrorCode;                // 记录错误代码（4字节数字） 
	uint8_t State;                     // 记录当前状态（1字节数字） 
} DMA_HandleTypeDef;
```

在结构体内部的这行代码里，`DMA_Stream_TypeDef` 的后面紧紧跟着一个星号 `*`！这直接意味着，**结构体里装的根本不是另一个庞大的结构体，而仅仅是一个【用来记物理内存地址的指针变量（只有 4 个字节大小）】**！

真实运转流程：

1. 单片机里有很多个 DMA 通道（比如 DMA1_Stream0, DMA1_Stream1...）。它们在芯片内部的物理门牌号（基地址）是死固定的（比如 `0x40026000`）。
2. 当你想用这个驱动结构体去管理 **DMA1 的第 0 个通道**时，你只需要在初始化时，把门牌号赋给它：
  ```c
  DMA_HandleTypeDef my_dma_handler;
  my_dma_handler.DMA_Streamx = DMA1_Stream0; // 👈 把物理地址（如 0x40026000）传给它
  ```

3. 接下来，当你想在 App 层让这个 DMA 开启传输时，驱动函数内部就可以通过这个指针，**直接物理操控硬件的寄存器**：
   
 ```c
 // 驱动内部代码：顺着门牌号找到硬件，把它的控制寄存器（CR）的启动位（EN）给置 1！
 my_dma_handler.DMA_Streamx->CR |= DMA_SxCR_EN; 
 ```

只要看到结构体里出现了另一个大写的、长得很像类型的名字：

1. **如果带星号 `*`（如 `DMA_Stream_TypeDef *DMA_Streamx;`）**：
    - **真相**：这叫**外设句柄（Pointer to Peripheral）**。它不占空间（在 32 位单片机里只占 4 字节），它只是一个**远程遥控器的物理地址线**。
    - **作用**：让你的软件代码能够精准操控具体的物理硬件。
2. **如果不带星号（如 `GPIO_InitTypeDef Init;`）**：
    - **真相**：这才是真正的结构体嵌套（实体套实体）。
    - **作用**：这通常是一张**“配置参数大礼包（表单）”**，里面装满了诸如引脚速度、上拉下拉等具体的配置数值，用来一次性传给初始化函数。

具体的使用方式
```c
/* 1. 先定义被嵌套的底层配置图纸 */ 
typedef struct 
{ 
	uint32_t pin; // 引脚号（分号结尾，代表独立变量声明语句） 
	uint32_t mode; // 模式（分号结尾） 
} GPIO_Config_t; 

/* 2. 再定义总管结构体图纸（也就是 total_manager 的真身） */ 
typedef struct 
{ // 核心关键：嵌套的结构体指针！它是一个 4 字节的物理地址接收器 
	GPIO_Config_t *p_config; // 可以在总管里继续塞入其他的业务属性 uint8_t device_id; 
	// 设备编号 
	uint8_t status; // 设备状态（比如：0代表休眠，1代表运行） 
} Device_Manager_t; // 这一整行声明结束，用分号收尾！


int main(void) 
{ 
	// 内存里现在有两个实打实的硬件配置表（实体 A 和 实体 B） 
	GPIO_Config_t config_room_A = 
	{ 
		.pin = 1, 
		.mode = 0 
	}; 
	
	GPIO_Config_t config_room_B = 
	{ 
		.pin = 2, 
		.mode = 1 
	}; 
	
	Device_Manager_t total_manager; // 总管 
	
	// 物理动作 1：白天，让总管去管 A 房间 
	total_manager.p_config = &config_room_A; 
	total_manager.p_config->pin = 10;   // 此时修改的是 config_room_A.pin！ 
	
	// 物理动作 2：黑夜，【动态调换指向】！让总管转头去管 B 房间 
	total_manager.p_config = &config_room_B; 
	total_manager.p_config->pin = 20; // 此时修改的是 config_room_B.pin！config_room_A 毫发无损！ 
}
```


## 2.4 Typedef （定义类型）

关键字：typedef

用途：**将一个比较长的变量类型名换个名字，便于使用，只能对变量类型进行换名，名字在右边，并且在末尾有；**

基本形式：

```c
typedef 原类型 新类型名;
```

例如：

```c
typedef unsigned int uint;
```
需要注意：

> `typedef` **通常不会创造一个本质上完全不同的新类型，只是给已有类型增加一个别名**

### 2.4.1 定义变量

普通指针变量：

```c
int *ptr;
```

改成类型别名：

```c
typedef int *IntPtr;
```

以后：

```c
IntPtr ptr;
```

等价于：

```c
int *ptr;
```

### 2.4.2 定义结构体

1.不使用 `typedef`

```c
struct Student
{    
	char name[20];    
	int age;
};
```

定义变量时必须写：

```c
struct Student student1;
```

2.使用 `typedef`

```c
typedef struct Student
{    
	char name[20];    
	int age;
} Student;
```

以后可以直接写：

```c
Student student1;
```

不需要重复写：

```c
struct Student
```

这里有两个名字：

```c
struct Student
```

是结构体标签。

```c
Student
```

是 `typedef` 创建的类型别名。

两者都可以使用：

```c
struct Student student1;
Student student2;
```

3.匿名结构体

如果不需要使用结构体标签，可以写：

```c
typedef struct
{    
	char name[20];    
	int age;
} Student;
```

使用：

```c
Student student1;
```

但此时没有：

```c
struct Student
```

这个类型名称。

因此下面不合法：

```c
struct Student student2;
```

### 2.4.3 Handle 类型

例如：

```c
typedef struct
{    
	USART_TypeDef *USARTx;    
	uint32_t baud_rate;    
	uint32_t timeout;    
	uint8_t *rx_buffer;    
	uint16_t rx_length;
} UART_Handle;
```

这里：

```c
typedef
```

负责给结构体类型起名：

```c
UART_Handle
```

然后定义实际句柄对象：

```c
UART_Handle uart1;
UART_Handle uart2;
```

关系是：

```
struct      
定义数据结构typedef     
给结构体类型起别名UART_Handle 
类型名称uart1       
实际结构体对象
```

### 2.4.4 定义枚举类型

不使用 `typedef`：

```c
enum SystemState
{    
	STATE_IDLE,    
	STATE_RUNNING,    
	STATE_ERROR
};
```

定义变量：

```c
enum SystemState state;
```

使用 `typedef`：

```c
typedef enum
{    
	STATE_IDLE = 0,    
	STATE_RUNNING,    
	STATE_ERROR
} SystemState;
```

以后可以直接写：

```c
SystemState state = STATE_IDLE;
```

状态机中非常常见：

```c
switch (state)
{    
	case STATE_IDLE:        
		break;    
		
	case STATE_RUNNING:        
		break;    
		
	case STATE_ERROR:        
		break;    
		
	default:        
		state = STATE_ERROR;        
		break;
}
```

### 2.4.5 定义指针类型

基本写法：

```c
typedef int *IntPtr;
```

使用：

```c
IntPtr p1;
IntPtr p2;
```

两者都是：

```c
int *
```

等价于：

```c
int *p1;
int *p2;
```

### 2.4.6 定义数组类型

可以把完整数组定义成一种类型：

```c
typedef int IntArray10[10];
```

使用：

```c
IntArray10 array1;
IntArray10 array2;
```

等价于：

```c
int array1[10];
int array2[10];
```

初始化：

```c
IntArray10 numbers =
{    
	1, 2, 3, 4, 5,    
	6, 7, 8, 9, 10
};
```

嵌入式例子：

```c
typedef uint8_t PacketBuffer[128];

PacketBuffer rx_buffer;
PacketBuffer tx_buffer;
```

### 2.4.7 定义函数指针类型

假设函数：

```c
int add(int a, int b)
{    
	return a + b;
}
```

对应函数指针：

```c
int (*operation)(int, int);
```

用 `typedef` 简化：

```c
typedef int (*Operation)(int, int);
```

以后：

```c
Operation operation;
```

等价于：

```c
int (*operation)(int, int);
```

赋值：

```c
operation = add;
```

调用：

```c
int result = operation(10, 20);
```

### 2.4.8 回调函数类型

```c
typedef void (*UART_Callback)( uint8_t data);
```

注册：

```c
static UART_Callback rx_callback = NULL;
```

赋值：

```c
rx_callback = app_uart_rx_handler;
```

调用：

```c
if (rx_callback != NULL){ rx_callback(data);}
```

如果不使用 `typedef`，会写成：

```c
void (*rx_callback)(uint8_t data);
```

使用 `typedef` 后，可读性更高。

### 2.4.9 `typedef` 和 `#define` 的区别

`typedef`：

```c
typedef unsigned int uint;
```

**由编译器处理，是类型系统的一部分。**

`#define`

```c
#define UINT unsigned int
```

**由预处理器处理，只是文本替换。**

指针例子

使用宏：

```c
#define PINT int *PINT a, b;
```

展开：

```c
int *a, b;
```

因此只有 `a` 是指针。

使用 `typedef`：

```c
typedef int *PInt;

PInt a, b;
```

两者都是指针。

什么时候使用哪一个

类型别名：

```c
typedef uint32_t TickType;
```

常量：

```c
#define BUFFER_SIZE 128U
```

条件编译：

```c
#ifdef DEBUG

#endif
```

宏函数：

```c
#define ARRAY_SIZE(x) \
    (sizeof(x) / sizeof((x)[0]))
```

不要使用 `#define` 代替复杂类型定义。

# 3 变量、函数与流程控制
## 3.1 变量作用域

(1)局部变量（local variable）

在函数（比如 main 函数，或者 LED_Control 函数）的大括号 { ... } 里面定义的变量，叫局部变量。

规则：出了这间房（大括号），这个变量就不存在了。

生命周期：函数执行时它才出生，函数执行完它就“死”了（内存被回收）。

(2)全局变量 (Global Variable)

如果想让所有函数都能用同一个变量，需要把它定义在所有函数的外面（通常写在 .c 文件的最顶上）。这叫全局变量。

规则：文件里任何一个函数都能读、能改这个变量。

生命周期：程序一运行它就在，直到断电才消失。

传参和返回值
```c
//接收方:定义一个参数×来接数据
void LED_Control(int x)
{
	if(x== 1)  {...}
}

//发送方
void main()
{
	int apple = 1;
	LED_Control(apple); 	//把apple 的值传进去
}

//计算方
int Get_Sensor_Data()
{
	int temp = 25;
	return temp; 	//把temp的值扔出去
}

//接收方
void main()
{
	int data = Get_Sensor_Data(); //接住扔出来的值
}


```

## 3.2 `if...else`

适合：

- 范围判断；
- 浮点判断；
- 多个不同变量组成的复杂条件；
- 字符串比较结果判断。

```c
if (temperature > 80)
{
    alarm_on();
}
else if (temperature > 60)
{
    fan_on();
}
else
{
    fan_off();
}
```

在 C 中，条件表达式结果为 0 表示假，非 0 表示真。

## 3.3 `for` 条件

适合循环次数明确的场景：

```c
for (uint16_t i = 0U; i < length; i++)
{
    buffer[i] = 0U;
}
```

## 3.4 While 循环

在嵌入式开发（STM32）中，while 循环主要扮演两个截然不同的角色：“永动机”和“等待者”。

语法：

执行流程：先判断：电脑走到这就问：“条件成立吗？”

若成立 (True)：执行大括号里的代码。执行完最后一行后，立刻跳回第一步再次判断。

若不成立 (False)：直接跳过整个大括号，继续执行后面的代码。

核心用途：

1.死循环

```c
while(1)
{
	//main function
	LED_Blink();
	Key_Sacn();
}
```

在电脑软件里，死循环通常是 Bug（程序卡死了）。**但在单片机里，死循环是必须的**。单片机一上电就不能停。如果 main 函数执行到了结尾（return 0），单片机就无事可做了，这在嵌入式里是不允许的。 **所以，我们必须用 while(1) 把 CPU 困在里面，让它周而复始地去扫描按键、刷新屏幕、闪烁 LED。这是整个系统的主心骨**。

2.轮训等待

这是在配置外设时最常见的用法，用来死等某个硬件事件完成。

用法：
```c
//翻译:当(While)标志位是复位状态(RESET/0)时，就空转(分号)
while (USART_GetFlagstatus(USART1，USART_FLAG_TXE) == RESET);
//只有当硬件把标志位变成了SET(1)，上面的循环条件变成假，CPU才能从分号里逃出来，执行这个函数
USART_SendData(USART1,Data);
```
关键点：注意 while(...); **后面那个分号。这意味着循环体是空的，CPU 就在原地踏步，什么都不干，专门盯着那个寄存器看**。

<img width="482" height="211" alt="image" src="https://github.com/user-attachments/assets/a74346a9-22a3-4bbc-8031-ea666c073694" />


符合while里面的判断，就会一直等待，直到条件不满足while中的语句才会进行下一步的程序

## 3.5 Switch...case

### 3.5.1 定义
`switch...case` 是 C 语言中的**多分支选择语句**。它适合处理这种情况：

> 根据同一个变量的不同离散值，执行不同代码

例如根据命令执行不同功能：

```c
switch (command)
{    
	case 1:        
		led_on();        
		break;    
		
	case 2:        
		led_off();        
		break;    
	case 3:        
		system_reset();        
		break;
		    
	default:        
		handle_invalid_command();        
		break;
}
```

相当于：

```c
if (command == 1)
{    
	led_on();
}

else if (command == 2)
{    
	led_off();
}

else if (command == 3)
{    
	system_reset();
}

else
{    
	handle_invalid_command();
}
```

当分支很多，而且都是判断同一个变量是否等于某个固定值时，`switch...case` 通常比连续的 `if...else if` 更清楚。

### 3.5.2 基本语法

```c
switch (表达式)
{    
	case 常量1:        
		语句1;        
		break;    
		
	case 常量2:        
		语句2;        
		break;    
		
	default:        
		默认语句;        
		break;
}
```

完整执行过程：

1. 计算 `switch` 后面的表达式；
2. 将结果与每个 `case` 后面的常量比较；
3. 找到相等的 `case`；
4. 从该位置开始执行；
5. 遇到 `break` 后退出整个 `switch`；
6. 如果没有匹配项，则执行 `default`

`switch` 的表达式必须是**整数类型或可以转换成整数的类型**，常见合法类型：

```
int
char
short
long
unsigned 
int
uint8_t
uint16_t
enum
```

例如：

```c
uint8_t command = 0x01;

switch (command)
{    
	case 0x01:        
		
		break;    
		
	case 0x02:        
		
		break;    
	
	default:        
		
		break;
}
```

字符也可以：

```c
char operation = '+';

switch (operation)
{    
	case '+':        
		
		printf("Addition\n");        
		
		break;    
	
	case '-':        
		
		printf("Subtraction\n");        
		
		break;    
		
	default:        
		
		printf("Invalid operation\n");        
		
		break;
}
```

因为**字符在 C 语言中本质上也是整数编码**。

`break` 表示：

> 立即退出当前 `switch`。

如果不写 `break`，程序会继续向下执行后面的 `case`

需要注意：

> `case` 只是一个跳转入口，不会自动结束前一个分支。

### 3.5.3 `switch` 和 `if...else` 的区别

#### 3.5.3.1 适合使用 `switch` 的情况

**判断同一个变量是否等于多个固定值**：

```c

switch (command)
{    
	case 1:    
	
	case 2:    
	
	case 3:
}
```

例如：

- 命令字；
- 枚举状态；
- 菜单选项；
- 按键编号；
- 错误码；
- 字符运算符；
- 协议类型。

#### 3.5.3.2 适合使用 `if...else` 的情况

判断范围：

```c
if (temperature > 80)
{

}

else if (temperature > 60)
{

}
```

多个不同条件：

```c

if ((temperature > 50) && (voltage < 3.0f))
{

}
```

浮点数：

```c
if (voltage > 3.3f)
{

}
```

字符串：

```c
if (strcmp(command, "start") == 0)
{

}
```

对比

|情况|`switch`|`if...else`|
|:---:|:---:|:---:|
|同一变量匹配固定整数值|适合|可以|
|范围判断|不适合|适合|
|浮点判断|不支持|支持|
|字符串判断|不直接支持|支持|
|复杂逻辑条件|不适合|适合|
|枚举状态处理|非常适合|可以|
## 3.6 运算符
### 3.6.1 位运算符

& , | , ~ , ^  —— 用来修寄存器、操作硬件的。	

(1) & (按位与) —— “掩码/清零”

规则：只有两个都是 1，结果才是 1。否则是 0。

口诀：“有 0 则 0”。

STM32 用法：用来检查某一位是不是 1，或者把某一位清零。

(2) | (按位或) —— “融合/置位”

规则：只要有一个是 1，结果就是 1。

口诀：“有 1 则 1”。

STM32 用法：用来把某一位设为 1，且不影响其他位。

(3) ~ (按位取反) —— “唱反调”

规则：0 变 1，1 变 0。

STM32 用法：通常配合 & 用来清除某一位。

(4) ^ (按位异或) —— “反转/翻转”

规则：相同为 0，不同为 1。

STM32 用法：用来让 LED 闪烁（0 变 1，1 变 0）。

(5) << 和 >> (移位) —— “挪位置”

规则：把二进制数整体向左或向右移动。

STM32 用法：用来构造掩码。

例子： 1 << 5 的意思就是把 1 向左移 5 位，变成 0010 0000 (也就是 0x20)。 

### 3.6.2 逻辑运算符

&&,  ||,  ! —— 用来写 if 语句做判断的。

(1) && (逻辑与) —— “并且”

规则：全真才真。

场景：要同时满足多个条件。

(2) || (逻辑或) —— “或者”

规则：有一个真就真。

场景：满足任意一个条件就行。

(3) ! (逻辑非) —— “不”

规则：真变假，假变真。

场景：判断“没有发生”什么。

单个符号搞硬件，成双成对做判断

### 3.6.3 >>和<<

`>>`右移运算符

`<<`左移运算符  **箭头的方向代表移动的方向**

把一个数的所有二进制位，整体向左搬家

动作原理：整体左迁，右边补零

想象这还是一个字节（8位）的数据。

规则：所有的 0 和 1 集体向左移动 n 位。左边溢出的位（掉出悬崖的）直接丢弃。右边空出来的位，无脑补 0。
举个例子：假设变量 a = 3 (二进制是 0000 0011)。如果我们执行 a << 2 (左移 2 位)：

|步骤|	二进制状态|	说明|
|:---:|:---:|:---:|
|原始值|	0000 0011|	十进制的 3|
|移位中	|00 0011__|	整体向左挪了2格，右边空了2格|
|补零后|	0000 1100|	右边补上两个 0，左边超出的被切掉|

**左移（`<<`）用于“数据拼接”（从小变大/组合）；右移（`>>`）用于“数据拆分”（从大变小/提取）**

规律一：**接收数据时用左移**—— 小字节“拼”大整数（组合场景）

典型场景：SPI 接收 2 个字节，拼接成 16 位 ADC 电压值

假设 SPI 连续读到了两个字节：`high_byte = 0x12`，`low_byte = 0x34`。你想拼成 `0x1234`：

1. 先把 `high_byte` **左移 8 位**（变成 `0x1200`），把低 8 位的空位“顶出来”。
2. 再和 `low_byte`（`0x34`）进行按位或（`|`），缝合在一起变成 `0x1234`。

```c
uint16_t result = ((uint16_t)high_byte << 8) | low_byte; // 👈 经典左移拼接
```

规律二：**发送数据时用右移**—— 大整数“拆”小字节（拆分场景）

典型场景：把 32 位地址拆成 4 个 8 位字节通过 SPI 发送

```c
// 假设地址是 0x12345678
spi_send_byte(spi, (uint8_t)(addr >> 24)); // 右移24位，把 0x12 挤到最右边，切下来发走
spi_send_byte(spi, (uint8_t)(addr >> 16)); // 右移16位，把 0x34 挤到最右边，切下来发走
spi_send_byte(spi, (uint8_t)(addr >> 8));  // 右移8位， 把 0x56 挤到最右边，切下来发走
spi_send_byte(spi, (uint8_t)addr);         // 不需要右移，直接切下最右边的 0x78 发走
```

规律三：配置芯片寄存器时（设置某一个特定功能）

在操作单片机寄存器或者传感器配置命令时，左移和右移代表着**“定位”**和**“读取”**：

- **用【左移】来“写入/配置”**：你想把某个控制参数放到寄存器的第 N 位上。
    - 规律：`数据 << 位次`
    - 例子：控制寄存器的第 3 位代表开启 LED（`1` 开启）。你不需要去手算十六进制，直接写 `(1 << 3)`，编译器会自动帮你算成二进制的 `00001000`。
- **用【右移】来“读取/判断”**：你想知道寄存器第 N 位当前是 `0` 还是 `1`。
    - 规律：`(寄存器 >> 位次) & 1`
    - 例子：你想知道第 5 位是不是 `1`。把寄存器整体**右移 5 位**，让目标位跑到最右边变成第 0 位，然后 `& 1`（把旁边不相干的位全部抹零清空），直接判断结果是 `0` 还是 `1`。


# 4 模块化与嵌入式关键字

## 4.1 区分App（应用层）和Driver（驱动层）

在嵌入式和计算机软件架构中，划分**App（应用层）**和 **Driver （驱动层)** 的核心评判铁律只有一条：**看这段代码是否跟特定的“硬件芯片/物理引脚”直接绑死**

如果把系统比作一个**“智能厨房”**：

- Driver（驱动层）是那些具体的电器说明书和开关操作（比如：微波炉加温、冰箱降温、烤箱控温） 
- App（应用层）是顶层的菜谱和烹饪逻辑（比如：先用微波炉热3分钟，再用烤箱烤10分钟）。**菜谱本身绝不关心微波炉底层的加热管是用什么牌子的芯片控制的**。

### 4.1.1 核心职责的本质区别

Driver 层（驱动层）：硬件的“无情代理人”

- **本质**：把物理世界的芯片、引脚、寄存器，翻译成软件世界能看懂的“基本动作” 
- **它的世界观**：它只负责把某件事情“办到”，**完全不关心业务逻辑** 
- **例子**：一个 LED 的驱动函数 `Driver_LED_On()`，它的工作就是让单片机的 `PA0` 引脚物理输出高电平。至于为什么要开灯？是因为警报响了还是用户按了按键？驱动层一概不知，也绝不应该去管。

App 层（应用层）：产品的“灵魂操控者”

- **本质**：纯粹的用户业务逻辑、产品的核心功能实现 
- **它的世界观**：它负责决定“什么时候、以什么顺序、去干什么事”，**完全不关心硬件细节** 
- **例子**：防丢器项目里，App 层在算：`if (蓝牙断开连接时间 > 10秒)` → 应该触发报警灯闪烁。App 层只负责发出这个“开灯”的宏观指令，具体怎么控制引脚去开灯，它直接调用驱动层的接口。

### 4.1.2 通过代码对比

| 维度           | Driver 层（驱动层）代码特征                                             | App 层（应用层）代码特征                                                         |
| :------------: | :-------------------------------------------------------------: | :----------------------------------------------------------------------: |
| **包含的头文件**   | 大量包含寄存器定义、硬件库（如 `stm32f4xx_hal.h`、`esp_gpio.h`）。              | 主要包含业务结构体、纯逻辑定义、以及驱动层暴露出来的 API 头文件。                                    |
| **关键变量与命名**  | 充满物理字眼：`PIN`、`GPIO`、`SPI`、`I2C`、`BaudRate`、`Register`。        | 充满业务语义：`User_Age`、`Target_Temperature`、`Is_Connected`、`STATE_RUNNING`。 |
| **函数接口设计**   | `W25Q128_Read_Byte()`、`OLED_Draw_Point()`、`Motor_Set_Speed()` | `OTA_Process_Task()`、`UI_Refresh_Menu()`、`Battery_Management()` 。      |
| **移植性（换芯片）** | 如果把单片机从 STM32 换成 ESP32，这部分代码**基本要全部重写**。                      | 如果把单片机从 STM32 换成 ESP32，这部分代码**一行都不用改，直接编译通过**。                         |

### 4.1.3 终极思想实验

“换芯测试法”

如果你在一个函数前犹豫不决，不知道该把它归到 App 还是 Driver，请在脑子里做这个著名的行业测试：

> **“假设明天老板为了省钱，把物理屏幕从 OLED 换成了 LCD，或者把控制器芯片换了。我手上的这个函数需要修改吗？”**

- **答案 A：必须改！** 里面的时序、引脚、初始化命令都要跟着新屏幕/新芯片去重新配置 → **这绝对属于 Driver 层。**
- **答案 B：完全不用改！** 换了什么屏幕，我的菜单逻辑依然是“按一下往下走一格” → **这绝对属于 App 层。**



## 4.2 `#`
`#`是预处理器（Preprocessor） 的信号灯。当编译器真正开始编译代码之前，预处理器会先扫描一遍代码。每当它看到 `#`，它就知道：“这是给我的指令，我要在编译器接入之前处理它。”

|         符号/指令          |     名称      |       作用       |      典型场景      |
| :--------------------: | :---------: | :------------: | :------------: |
|       `#include`       |     包含      |     导入头文件      |  几乎所有 C++ 程序   |
|       `#define`        |     定义      |    文本替换/宏定义    |   常量、简单的内联逻辑   |
|       `##` (在宏中)       |    字符串化     | 将参数变为字符串 "..." |   调试日志、反射辅助    |
|     `###` (在宏中)	连接     | 将两个代码片段拼成一个 |    自动生成变量名     |                |
| `## ifndef / '#endif'` |    条件编译     |  控制代码块的开启/关闭   |  跨平台兼容、头文件保护   |
|       `#pragma`        |    杂项指令     |    编译器特有控制     | `#pragma once` |

### 4.2.1 `#Include` 包含
它的本质就是由编译器自动执行的复制粘贴

在编译器真正开始翻译代码（编译）之前，有一个叫“预处理器”的小助手会先跑一遍。**当它看到 '#include' 时，它会做以下动作：找到那个文件名（比如 stdio.h 或 led.h）；把那个文件里的所有内容复制出来；原地覆盖掉 '#include' 这一行**。

### 4.2.2 `#define` 宏定义

关键字：#define

用途：用一个字符串代替一个数字，便于理解，防止出错；**提取程序中经常出现的参数，便于快速修改,在宏定义的末端没有;**

定义宏定义：
```c
#define   ABC     12345
```

引用宏定义：
```c
int a = ABC;	//等效于int a = 12345;
```

示例：
``` c
#define GPIO_Pin_0		( (uint16_t)0x0001)
#define GPIO_Pin_1		( (uint16_t)0x0002)
#define GPIO_Pin_2		( (uint16_t)0x0004)
#define GPIO_Pin_3		( (uint16_t)0x0008)
#define GPIO_ Pin_4		( (uint16_t)0x0010)
```

用GPIO_Pin_0来代表0x0001

### 4.2.3 `#ifndef` 避免重复定义错误
'#ifndef' （if not defined）是 C/C++ 预处理器中的条件编译指令，主要用于在头文件里防止同一个文件被多次包含（避免重复定义错误）

其基本用法是：#ifndef 标识符 (如果没定义) -> '#define' 标识符 (定义它) -> '#endif' (结束)，确保头文件中的内容只被处理一次。及在.h文件中进行定义处理。

示例：
```c
#ifndef MY_HEADER_H 	// 1．如聚Y_HEADER_至没被定义过
#define MY_HEADER_H 	// 2.就定义它

//头文件内容（函数声明，结构体，故举等)
void myFunction() ;
#endif 					//3.结束条件编译按

```
工作原理：

当第一个源文件包含 my_header.h 时，MY_HEADER_H 未定义，预处理器执行 '#define' MY_HEADER_H，然后处理头文件内容。

如果第二个文件也包含 my_header.h，此时 MY_HEADER_H 已经定义，预处理器会跳过 '#define' 和头文件内容，直接到 '#endif' 之后，从而避免了重复定义。 

## 4.3 编写 .h和 .c文件

.h 文件 (头文件) = 餐厅菜单。它告诉别人：我有宫保鸡丁（函数声明）、我有鱼香肉丝（变量声明）。只给看，不给做。别人包含了你的 .h，就知道能调用什么功能。

.c 文件 (源文件) = 后厨做法。这里是实打实的炒菜过程（函数具体的代码逻辑）。只有你知道。这里的具体实现细节，外部文件不需要关心。

1.编写.h文件
```c
#ifndef  __BUZZER_H__
#define  __BUZZER_H__

void Buzzer_Init (void);
void Buzzer_ON(void);
void Buzzer_OFF (void);
void Buzzer_Turn (void);
#endif
```
.h 文件的标准结构只有三部分：防卫宏、引用依赖、对外接口。

‘#ifndef __BUZZER_H __'  

'#endif'：这叫头文件卫士。防止你如果在多个地

定义 '#define __BUZZER_H__'，导致代码重复定义报错。这是死规矩，必须写。

要注意在#ifndef和#define后都需要空一格再加两个下划线再写名称

在中间加入.c文件中定义的函数，末尾加上；

2.编写.c文件 

在.c文件中写入函数的具体代码逻辑

3.在main函数中调用函数

在外部调用的时候要先引用这个.h模块

在main中就可以直接用函数

## 4.4 Extern 声明

extern 的意思是：“请注意，这个变量（或函数）已经在别的文件里定义过了，这里只是引用它，请编译器去别处找它的真身。”

extern 是什么：**它是连接多个 C 文件的桥梁**。

什么时候用：当需要在 A.c 里使用 B.c 里定义的全局变量时。

怎么用（最佳实践）：

在 B.c 中定义变量：int count = 0;

在 B.h 中声明变量：extern int count;

在 A.c 中 ## include "B.h"。


## 4.5 Printf 输出

1.C语言输出规则主要由 printf( ) 函数控制，核心是格式控制字符串，包含普通字符（原样输出）、转义字符（如\n换行、\t制表），以及最关键的格式说明符（如%d(Decimal十进制)整数、%f浮点数、%s字符串）来指定数据类型和显示方式。输出格式支持宽度、精度、对齐方式（左对齐-、右对齐）等修饰，例如%5d输出整数占5位，%.2f输出两位小数。

2.格式：printf("格式控制", 输出列表);

- 格式控制字符串: 双引号内的部分。

- 输出列表: 用逗号分隔的变量或常量。

3.格式说明符 
- 整数:

	%d 或 %i : 十进制有符号整数。 
		
	%u: 十进制无符号整数。
		 
	%o: 八进制无符号整数。
		 
	%X: 十六进制无符号整数（小写/大写字母）。

- 浮点数:

	%f: 小数形式，默认6位小数。 
	
	%E: 指数形式。 
	
	%G: 自动选择 f 或 e 格式，不含无意义的零。 

- 字符和字符串: 

	%c （character）: 单个字符。 
	
	%s （string）: 字符串。（会将字符串完整的输出出去，一直到结束符） 

- 其他: 

	%p: 指针地址。 
	
	%: 输出百分号 % 本身。 

  
4.修饰符（放在%和类型之间）

- 宽度 (m): %m.nf，指定最小输出宽度，不足则补空格。
	
- 精度 (n): %.nf，指定小数位数，%.ns指定字符串取前n个字符。
	
- 标志:
	-: 左对齐
	
	+: 正数显示+号。
	
	0: 用前导零填充宽度。 


5.示例
``` c
#include <stdio.h>
int main()
{
	int num = 123;
	double pi = 3.1415926;
	char *str = "Hello";

	printf("整数:%din", num) ;			//输出:整数:123
	printf("两位小数:%.2f\in", pi;		//输出:两位小数:3.主4
	printf(“宽度5,左对齐:%-5din",num);	//输出:窳度5,左对齐:123(右边补空格)
	printf("宽度5,右对齐:65din", num); 	//输出:宽度5,右对齐:123（左边补空格)
	printf("字符串:%sin", str);			//输出:字符串:Hello
	printf(“字符串截断:%.3sin, str);		//输出:字符串截断:Heireturn 0:

```

## 4.6 Handle（句柄）

### 4.6.1 Handle 不是关键字

`Handle` 是命名和设计概念，不是 C 语言关键字。

真正创建类型别名的是：

```c
typedef
```

例如：

```c
typedef struct
{
    USART_TypeDef *instance;
    uint32_t baud_rate;
    uint32_t timeout;

    const uint8_t *tx_buffer;
    uint16_t tx_length;
    volatile uint16_t tx_count;

    uint8_t *rx_buffer;
    uint16_t rx_length;
    volatile uint16_t rx_count;

    volatile uint32_t error_code;
} UartHandle;
```

定义对象：

```c
UartHandle uart1;
UartHandle uart2;
```

可以理解为：

- `UartHandle`：设备管理档案的格式；
- `uart1`：USART1 的具体管理对象；
- `uart2`：USART2 的具体管理对象。

### 4.6.2 Handle 中通常保存什么

- 硬件实例地址；
- 配置参数；
- 运行状态；
- 发送和接收缓冲区；
- 当前计数；
- 错误码；
- DMA Handle；
- 回调函数；
- 用户上下文。

### 4.6.3 为什么传 Handle 指针

```c
DriverStatus uart_init(UartHandle *handle);
```

原因：

- 避免复制整个结构体；
- 函数可以修改原 Handle 的状态；
- 一套驱动函数支持多个设备实例；
- 中断、DMA 和回调可以共享设备上下文。

### 4.6.4 Handle 的生命周期

异步操作的 Handle 不能是马上失效的局部对象：

```c
void start_receive(void)
{
    UartHandle handle;       // 函数退出后生命周期结束
    uart_receive_it(&handle);
}
```

中断稍后继续使用该地址会产生悬空指针。

异步 Handle 通常使用：

```c
static UartHandle uart1;
```

或文件作用域对象。

### 4.6.5 模块私有和公共 Handle

模块私有：

```c
static UartHandle uart1;
```

通过函数提供操作，封装性更好。

公共对象：

```c
/* uart.h */
extern UartHandle uart1;

/* uart.c */
UartHandle uart1;
```

更灵活，但外部代码可能错误修改内部状态。


## 4.7 Callback（回调函数）

回调函数本质上就是：**把一个函数的地址交给另一个模块保存，在特定事件发生时，由那个模块主动调用这个函数**。

回调函数的调用是：

应用层把函数地址注册给驱动  
↓  
驱动保存函数地址  
↓  
发送完成、中断发生或错误出现  
↓  
驱动反过来调用应用层函数

基础是：**函数指针**

函数指针保存函数地址：

```c
void (*callback)(void);
```

这个声明表示：callback 是一个指针它指向一个没有参数、没有返回值的函数

### 4.7.1 函数指针
1.定义普通函数

```c
void led_on(void){    // 打开LED}
```

2.定义函数指针

```c
void (*callback)(void);
```
3.保存函数地址

```c
callback = led_on;
```

也可以写：

```c
callback = &led_on;
```

两者通常等价。

4.通过函数指针调用函数

```c
callback();
```

也可以写：

```c
(*callback)();
```

两种方式都可以，但通常写：

```c
callback();
```

完整示例：

```c
#include <stdio.h>
void led_on(void)
{    
	printf("LED ON\n");
}
int main(void)
{    
	void (*callback)(void);
	    
	callback = led_on;
	    
	callback();
	    
	return 0;
}
```

### 4.7.2 回调函数的三个基本步骤

1.定义回调函数类型

```c
typedef void (*UART_RxCallback)(uint8_t data);
```

2.注册回调函数

```c
UART_RxCallback rx_callback = NULL;

void uart_register_rx_callback(UART_RxCallback callback)
{    
	rx_callback = callback;
}
```

3.事件发生时调用

```c
void uart_receive_byte(uint8_t data)
{    
	if (rx_callback != NULL)    
	{        
		rx_callback(data);    
	}
}
```

应用层：

```c
void app_uart_rx_handler(uint8_t data)
{    
	// 应用层处理数据
}
int main(void)
{    
	uart_register_rx_callback(app_uart_rx_handler);    
	while (1)    
	{
	    
	}
}
```

4.完整的回调函数示例

 `uart.h`

```c
#ifndef __UART_H__
#define __UART_H__
#include <stdint.h>

typedef void (*UART_RxCallback)(uint8_t data);

void uart_init(void);

void uart_register_rx_callback(UART_RxCallback callback);

void uart_irq_handler(void);

#endif
```
`uart.c`

```c
#include "uart.h"

static UART_RxCallback rx_callback = NULL;

void uart_init(void)
{
	rx_callback = NULL;                          // UART硬件初始化
}

void uart_register_rx_callback(UART_RxCallback callback)
{    
	rx_callback = callback;
}

void uart_irq_handler(void)
{    
	uint8_t data;    
	
	data = (uint8_t)USART1->DR;
	   
	if (rx_callback != NULL)    
	{        
		rx_callback(data);    
	}
}
```
`main.c`

```c
#include "uart.h"
static volatile uint8_t received_data;
void app_uart_rx_callback(uint8_t data)
{    
	received_data = data;
}

int main(void)
{    
	uart_init();
	   
	uart_register_rx_callback(app_uart_rx_callback);    
	
	while (1)    
	{ 
		// 主程序处理received_data    
	}
}
```

5.真正的中断函数

```c
void USART1_IRQHandler(void)
{    
	uart_irq_handler();
}
```

运行过程：

```text
main.c注册app_uart_rx_callback              
↓
uart.c保存函数地址              
↓
UART收到数据，发生中断              
↓
USART1_IRQHandler执行              
↓
uart_irq_handler执行              
↓
rx_callback(data)              
↓
实际调用app_uart_rx_callback(data)
```

## 4.8 `' '`和`" "`的区别
**单引号 ' '：给单个字符用的（本质是数字）**。

**双引号 " "：给字符串用的（本质是内存地址/数组）**。

(1)单引号 ' ' (Character)

含义：代表一个字符 (char)。

本质：在 C 语言看来，它就是一个整数 (Integer)（对应的 ASCII 码值）。

占用空间：**1 个字节**。

举例：char = ‘A’;

单引号 ' ' (Character)

含义：代表一个字符 (char)。

本质：在 C 语言看来，它就是一个整数 (Integer)（对应的 ASCII 码值）。

(2)双引号 " " (String)

含义：**代表一个字符串** (String)。

本质：它是一个数组，或者说是一个指向内存首地址的指针。

占用空间：**字符数量 + 1 (必须要有一个看不见的结束符 \0)**。

举例：`char str[ ]="A";`

编译器看到 "A"，它会在内存里开辟两个格子：'A' (65);\0 (0) -> 这是双引号自动帮你加上的结束标志！

举例：
```C
// 1.修正类型:用char*来保存字符串
char name[] = "Da Yang" ;
char* p = name ;
int age = 21;
int i;
printf("名字:\n");
for (i = 0; i < 8; i++)
{
	printf("%c,地址=%x\n",*p,p);
	p++;
}
```

|  特性  |  'A' (单引号)   |       "A" (双引号)        |
| :--: | :----------: | :--------------------: |
|  类型  | char (字符/整数) | char* 或 char[] (地址/数组) |
| 内存样子 |    [ 65 ]    | [ 65 ] [ 0 ] (自动补了 \0) |
|  大小  |     1 字节     |          2 字节          |
| 生活类比 |     一颗子弹     |      一个只装了一颗子弹的弹夹      |
## 4.9 State Machine（状态机）

### 4.9.1 定义

它是一种**程序设计方法**，用来描述：

> 系统在不同状态下应该做什么，以及满足什么条件时切换到另一个状态。

例如一个电机可能有这些状态：

```
停止启动中运行中停止中故障
```

系统在任意时刻只处于其中一个状态。

不同状态下，程序执行不同逻辑：

```
停止状态：等待启动命令
启动状态：逐渐提高转速
运行状态：正常控制电机
停止状态：逐渐降低转速
故障状态：关闭电机并报警
```

这就是状态机。

状态机关注的主要问题是：

1. 当前是什么状态；
2. 当前状态要执行什么；
3. 什么条件下切换状态；
4. 切换到哪个状态。

### 4.9.2 状态机的四个基本组成部分

#### 4.9.2.1 状态

系统当前所处的阶段。

例如：

```c
typedef enum
{    
	STATE_IDLE,    
	STATE_RUNNING,    
	STATE_ERROR
} SystemState;
```

这里定义了三个状态：

```c
STATE_IDLE      空闲
STATE_RUNNING   运行
STATE_ERROR     错误
```

#### 4.9.2.2 事件

促使状态发生变化的事情。

例如：

```
按下启动按钮
收到串口命令
定时器超时
传感器检测到故障数据
接收完成
```

#### 4.9.2.3 转移条件

决定是否切换状态的判断条件。

例如：

```c
if (start_button_pressed())
{    
	state = STATE_RUNNING;
}
```

#### 4.9.2.4 动作

在某个状态下执行的操作。

例如：

```c
motor_start();
led_on();
uart_send_data();
```

### 4.9.3 最简单的状态机代码

```c
#include <stdint.h>
typedef enum
{    
	STATE_IDLE = 0,    
	STATE_RUNNING,    
	STATE_ERROR
} SystemState;

static SystemState current_state = STATE_IDLE;

void state_machine(void)
{    
	switch (current_state)    
	{        
		case STATE_IDLE:        
		{            
			if (start_condition())                //在start_condition()函数中会有一个return的返回值，if去判断这个返回值，如果是0就是不成立；除去0之外的任意其他数字都是成立
			{                
				current_state = STATE_RUNNING;            
			}     
			       
			break;        
		}        
			
		case STATE_RUNNING:        
		{            
			system_run();            
			if (error_condition())            
			{                
				current_state = STATE_ERROR;            
			}
			            
			break;        
		}        
		
		case STATE_ERROR:        
		{            
			system_stop();            
			
			error_handler();            
			
			break;        
		}        
		
		default:        
		{            
			current_state = STATE_ERROR;            
			
			break;        
		}    
	}
}
```

主程序：

```c
int main(void)
{    
	system_init();    
	
	while (1)    
	{        
		state_machine();    
	}
}
```

程序会不断调用：

```c
state_machine();
```

每次根据：

```c
current_state
```

执行对应状态的代码。

状态机的优点主要是：

- 程序流程清晰；
- 减少复杂嵌套判断；
- 容易增加新状态；
- 容易排查错误；
- 特别适合嵌入式实时系统；
- 适合非阻塞程序设计。

### 4.9.4 状态机和普通流程代码的区别

普通流程代码通常是从上到下执行：

```c
motor_start();
delay_ms(1000);
motor_run();
delay_ms(5000);
motor_stop();
```

这种写法的问题是：

```c
delay_ms()
```

会阻塞程序。

阻塞期间，CPU可能无法及时处理：

- 按键；
- 串口；
- 传感器；
- 其他设备；
- 错误事件。

状态机通常写成非阻塞形式：

```c
switch (motor_state)
{    
	case MOTOR_STARTING:        
		if (start_time_elapsed())        
		{            
			motor_state = MOTOR_RUNNING;        
		}        
		
		break;
}
```

程序不会一直停在某个状态等待，而是每次快速检查一次

## 4.10 阻塞与非阻塞状态

### 4.10.1 核心定义
1. 阻塞（Blocking）：死等结果，原地卡死

当程序调用一个阻塞函数时，在所等待的事件（如串口收到数据、延时到期、按键按下）发生之前，**当前线程或 CPU 核心会被操作系统或硬件强制挂起（或在 `while` 循环里死等）**。此时，程序停滞在这一行，无法向下执行任何后续代码。

2. 非阻塞（Non-blocking）：看一眼就走，异步推进

当程序调用一个非阻塞函数时，函数会立刻探测目标状态。如果数据还没准备好，它**绝不逗留，而是立刻返回一个特定的状态码**（如 `EAGAIN`、`HAL_BUSY`），告诉调用者：“现在还没好，你先去忙别的吧。”

### 4.10.2 代码对比
1. 阻塞式接收（死等标志位）

```c
// 阻塞调用：必须收满 10 个字节，或者熬过 5000 毫秒，否则死都不放手
HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, rx_buffer, 10, 5000);

// 只有上面这行代码真正执行完了，CPU 才能走到这一行
LED_Toggle(); // ❌ 如果对方一直不发数据，LED 闪烁任务会被无情卡死 5 秒！
```

** 阻塞的底层物理逻辑：**  
函数内部通常包含一个无情的 `while` 循环，不停地去读取硬件寄存器的状态标志位（Flag）：

```c
// 阻塞函数内部伪代码
while (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) == RESET) {
    if (Check_Timeout()) return HAL_TIMEOUT; // 没到时间就一直在这里打转，CPU 满载空转
}
```

2. 非阻塞式接收（状态轮询 或 中断回调）

非阻塞的优雅之处在于，它把“等待”的动作交给了**硬件自动处理**，或者通过**快速查询**来解放 CPU。

方案 A：非阻塞轮询（Polling）
```c
// 检查硬件寄存器：有数据掉进来了吗？
if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == SET) {
    rx_buffer[i++] = huart1->Instance->DR; // 到了就顺手收一个
}
// 不管有没有数据，CPU 毫无延迟地一掠而过，继续往下冲！
LED_Toggle();        //  灯照样闪烁
Refresh_OLED_UI();   //  屏幕照样流畅刷新
```

方案 B：异步非阻塞（利用中断/DMA，工业级核心）

这是最高阶的非阻塞玩法。CPU 只是“下达命令”，具体干活交给硬件。

```c
// 主动下达行政命令：开启中断/DMA接收，收满 10 个字节硬件会自动通知我，我走了！
HAL_UART_Receive_IT(&huart1, rx_buffer, 10); 

while(1) {
    Run_App_Logic(); //  CPU 在主循环里疯狂跑核心业务，一天能跑几万行代码
}

/* 物理真相：当外界物理电平信号真的把 10 个字节传输完毕，硬件触发中断，CPU 瞬间跳转到这里 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    Process_New_Data(); //  “奶茶好了”，在这里处理收到的数据
}
```

### 4.10.3 精准区分
|区分维度|阻塞（Blocking）|非阻塞（Non-blocking）|
|:---:|:---:|:---:|
|**函数参数特征**|函数参数中极大概率带有 **`Timeout`（超时时间）**，如 `5000ms`。|通常没有超时参数，或者函数名带有 **`_IT`（中断）** 或 **`_DMA`** 后缀。|
|**返回值语意**|返回值代表**“事情最终的办理结果”**（如 `HAL_OK`、`HAL_TIMEOUT`）。|返回值仅代表**“这个命令有没有成功下达”**（如 `HAL_BUSY`、`HAL_OK`）。|
|**代码组织结构**|往往**只有一行**，后面紧跟处理该数据的业务代码。|往往伴随着 **`if-else` 判断状态**，或者需要编写 **`Callback`（回调函数）**。|
|**CPU 资源占用**|事件未发生时，CPU 在大括号里**原地疯狂打转**（裸机）或**被强行挂起切换**（OS）。|事件未发生时，CPU 正在**开心地跑别的高级业务逻辑**。|

### 4.10.4 工业开发

1. 驱动层的“阻塞死等”会毒死应用层的“多任务实时性”

- **翻车惨剧**：你在编写 W25Q128 Flash 的驱动时，为了图省事，在写入数据后写了一句`HAL_Delay(10);` 阻塞死等芯片内部擦除完毕。
- **代价**：在跑 RTOS 操作系统或者 LVGL 界面时，这 10 毫秒的死等，会导致界面的触摸响应直接卡顿，或者导致主任务错过了接收倒车雷达的紧急数据。
- **标准解法**：驱动层绝不轻易死等。利用状态机，每次进函数看一眼状态，没好就退出，好拉到下一次再处理（即非阻塞）。

2. 非阻塞轮询极易导致“数据吞吐量大时的数据丢失”

- **翻车惨剧**：如果你用非阻塞轮询（方案 A）去接收 Wi-Fi 模块发来的高速大量数据，如果你的主循环 `Run_App_Logic()` 里面包含了一些复杂的数学运算（耗时 20 毫秒），那么在这 20 毫秒内，串口硬件可能已经灌进了 50 个字节，由于 CPU 没来得及看一眼，**后来的数据会直接把前面的数据物理覆盖掉（Overrun Error，溢出错误）**。
- **标准解法**：面对高频、高速数据，**必须采用【非阻塞 + 中断/DMA】** 的架构。

3. 非阻塞异步容易诱发“内存乱套（竞态条件）”

- **翻车惨剧**：你调用了非阻塞的 `HAL_UART_Transmit_DMA(&huart1, my_buffer, 100);` 发送数据。DMA 刚开始在后台偷偷摸摸地搬运。结果你在下一行应用层代码里，急急忙忙地写了一句：`my_buffer[0] = 0xFF;` 修改了数组内容。
- **代价**：DMA 还没搬完呢，你就把数据改了！导致发出往外送的数据前几位全乱了。
- **标准解法**：在使用非阻塞异步发送时，**必须建立一个信号量（Flag）**。当 DMA 没搬完时，应用层（App）绝对禁止触碰、修改该内存缓冲区。

## 4.11 `GPIO_Init(..., ...)`和 `void GPIO_Init(..., ...)`

(1) `GPIO_Init(..., ...);`

这是：函数调用 (Function Call)

含义：“现在，立刻，马上去执行这件事！”

位置：必须写在函数体（比如 main() 或 GPIO_Init()）的大括号 { } 里面。

作用：CPU 执行到这一行时，会真的去设置寄存器，把GPIO重新配置

(2)` void GPIO_Init(..., ...);`

这是：函数声明/定义

这里有个核心细节：如果你在括号里填的是 GPIOB 这种具体的值，而前面加了 void，这通常是错误的写法。

①如果在 main 函数里这样写是错误的

void 是用来定义新函数的。在 main 函数里，不需要定义这个函数（ST公司已经定义好了），你只需要使用它。加了 void，编译器以为你想在 main 里面再创造一个函数，但语法又不对，所以报错。

②在 .h 头文件或帮助文档

注意区别：这里括号里写的是 类型 (uint8_t)，而不是具体的值 (GPIOB)。

含义：这是在告诉编译器，“有一个叫 GPIOx 的函数，它不返回值 (void)，它需要两个 uint8_t 类型的参数”。

|          写法           |   比喻   |             电脑的反应             |
| :-------------------: | :----: | :---------------------------: |
| `void GPIO_Init(...)` |  一本菜谱  | “哦，我知道这就叫‘做这道菜的方法’，但我不会现在去做。” |
|   `GPIO_Init(...)`    | 给厨师下个单 |       “好的！我这就去把这道菜做出来！”       |

## 4.12 `(GPIO_TypeDef *)` 强制类型转换

STM32 的 GPIOA 硬件实际上就住在一个固定的内存地址上，比如 0x40010800。但在编译器眼里，0x40010800 只是一个枯燥的十六进制整数 (Integer)，就像 100 或 999 一样。

`(GPIO_TypeDef *)` 的作用：给数字“穿制服”

`## define GPIOA   ( (GPIO_TypeDef * ) 0x40010800 )`

0x40010800：原本只是一个单纯的数字（物理基地址）。

`(GPIO_TypeDef *)`：给这个数字穿上了“结构体指针”的制服。

结果：**现在编译器认为 GPIOA 是一个合法的指针了**。

使用：所以你才能写 GPIOA->ODR。编译器会想：“哦，GPIOA 是个指针，指向一个结构体，那我去找这个地址偏移量为 ODR 的地方写数据。”

形象比喻：

0x40010800：这就像是地图上的一个经纬度坐标 (116.40, 39.90)。

`(GPIO_TypeDef *)`：这就像是在这个坐标上插了一个牌子，上面写着 “紫禁城”（而且规定了紫禁城里有太和殿、中和殿等布局）。

转换后：你不再说“去坐标 (116.40, 39.90)”，而是说“去紫禁城的太和殿 (GPIOA->ODR)”。

公式含义：纯数字地址→具有具体功能的结构体指针

## 4.13 智能提示的快速理解

<img width="626" height="22" alt="image" src="https://github.com/user-attachments/assets/bc17be69-04d6-41c5-b748-d2d0b287d287" />

拆成三部分理解，函数名和两个参数

函数名为GPIO_ResetBits，这个函数的作用是让某个引脚输出低电平（如果是控制LED，通常是熄灭；如果是低电平触发的电路，则是开启）

第一个参数`GPIO_TypeDef *GPIOx`，是一个指针，GPIOx 是一个占位符，提示要填入端口组的名字。可以填：GPIOA, GPIOB, GPIOC 等。他代表需要输入一个指针变量，当输入GPIOA时就是一个指针GPIOA在内部的定义为
```c
// 1. 这是一个纯粹的数字（基地址）
#define GPIOA_BASE    (APB2PERIPH_BASE + 0x0800)  // 算出结果是 0x40010800
// 2. 这是一个指针（把数字强制转换成了指针）
#define GPIOA         ((GPIO_TypeDef *) GPIOA_BASE)
所以GPIOA实际上就是代表了0x40010800的数据
```

第二个参数uint16_t GPIO_Pin，**uint16_t 只是告诉你这是一个 16 位的整数**（不用深究，知道是数字就行）; GPIO_Pin 提示要填入具体的引脚号，可以填：GPIO_Pin_0, GPIO_Pin_5, GPIO_Pin_13 等（或者是它们的组合 GPIO_Pin_0 | GPIO_Pin_1）

## 4.14 注意

1. 在编写的时候，下边没有提示可以按Ctrl+Alt+空格，会出现提示

2. 在编写的时候需要在最后空出一行，防止编译器或某些工具将最后一行代码视为被截断或忽略，避免潜在的编译警告（last line of file ends without a newline）或运行时错误，保证代码的完整性和兼容性GPIO（General Purpose Input Output）

3. C语言不支持二进制，在编写时只能使用16进制编写

4. 

# 5 嵌入式内存布局、分配与运行时管理 

## 5.1 存储器

在嵌入式系统和计算机架构中，存储器主要分为两大阵营：**RAM（随机存取存储器，掉电丢失）** 和 **ROM/Flash（只读/闪存，掉电不丢失）**

### 5.1.1 存储器的两大核心分类
|              存储器大类               |            常见类型            |            特点             |                   在嵌入式系统中的典型用途                   |
| :------------------------------: | :------------------------: | :-----------------------: | :----------------------------------------------: |
|     **RAM**  <br>(运行内存，速度极快)     |      **SRAM** (静态RAM)      |      芯片内部，无需刷新，速度最快。      | 存放 **FreeRTOS 的任务堆栈**、**LVGL 的显存缓冲区（VDB）**、全局变量。 |
|                                  |  **DRAM / SDRAM**（动态RAM）   |      芯片外部扩展，容量大，需刷新。      |      在跑大型 GUI（如高分辨率 LVGL）时，用作外部大显存或扩展运行内存。       |
| **ROM / Flash**  <br>(程序存储，掉电不丢) |  **Nor Flash** (如内部Flash)  | 支持芯片内执行（XIP），读取速度快，按字节读取。 |     存放 **可执行代码（Code）**、**LVGL 的静态图片/字库常量**。      |
|                                  | **Nand Flash** (如SD卡/eMMC) |     容量极大，擦写寿命长，按块读写。      |      存放大容量文件（如 UI 音视频素材、**OTA 升级固件的暂存区**）。       |

在早期的电子产品中，程序一旦写死在 ROM 里就再也无法更改。如果要修复一个 Bug，必须把整个芯片拆下来扔掉，换一块新的芯片，这在现代电子产品（尤其是需要 **OTA 远程升级** 的产品）中是不可接受的

关于ROM是有完整的进化史的
- **真正的` ROM`（Mask ROM，掩膜只读存储器）**：在芯片出厂制造时，程序就通过光刻机“焊死”在电路里了。**完全无法修改**，只能读。
- **PROM / EPROM**：虽然可以写，但极其麻烦。PROM 只能写一次（写错芯片就废了）；EPROM 需要用**紫外线灯**照着玻璃窗口照射几十分钟才能擦除，无法在电路板上直接操作。
- **FLASH（闪存）**：可以在普通的电信号控制下，**在电路板上直接进行单字节或按块的擦除与写入（电可擦除）**。


### 5.1.2 结合` FreeRTOS、LVGL` 和 `OTA` 的实战演练

RAM（运行内存）里都在跑什么？

当系统运行时，FreeRTOS 和 LVGL 会疯狂消耗 RAM：

- **FreeRTOS 堆栈（Heap/Stack）**：每个你创建的 FreeRTOS 任务（Task），都需要在 SRAM 中开辟一块空间存放它的上下文。
- **LVGL 缓冲区（Draw Buffer）**：LVGL 刷新屏幕时，需要先在 SRAM 中把画面“画”好，再发送给 LCD。分辨率越高，消耗的 RAM 越恐怖。

Flash（程序存储）里都存了什么？

芯片烧录后，固件是静态存放在 Flash 里的：

- **代码区（RO-Code）**：FreeRTOS 的调度算法、LVGL 的控制逻辑代码。
- **只读数据区（RO-Data）**：你在 LVGL 中用到的 **图标图片、艺术字库**。它们非常占空间，通常直接固化在 Flash 中，用的时候才读取。

OTA 时的存储器是如何工作的？

OTA 的本质，就是**对 Flash 进行擦除和重新写入**的过程：

-  **下载阶段**：4G/Wi-Fi 模组收到新的固件包，由于数据量大，RAM 装不下，必须一边下载，一边写入 **Flash 的“备份区（Slot 1）”**。
-  **校验阶段**：FreeRTOS 调度算法从 Flash 备份区读取数据，在 RAM 中计算 CRC/MD5 校验码。
-  **重启阶段**：Bootloader 运行，把备份区（Slot 1）的新固件，搬移覆盖到**运行区（Slot 0）**，完成升级。

### 5.1.3 常见术语：RAM 的三大段 (Heap, Stack, Static)

在看编译报告（如 Keil 的 `.map` 文件）时，你常会看到这些专业词汇，它们统统属于 RAM：

- **Stack（栈）**：局部变量、函数跳转时的现场保护。在 FreeRTOS 中，每个任务都有独立的 Stack。
- **Heap（堆）**：动态内存分配。例如用 `malloc()` 或 FreeRTOS 的 `pvPortMalloc()` 申请的空间。**LVGL 创建动态小部件（如弹窗）时极度依赖 Heap**。
- **Static/Global（静态/全局区）**：你在代码最外层定义的全局变量。

**栈（Stack）和堆（Heap）都属于 RAM**，但它们在**内存管理方式**、**数据生命周期**和**使用场景**上有本质的区别。

如果把 RAM 比作一家**高档餐厅**：

- **栈是“旋转火锅”**：座位固定，菜品按顺序转过来，吃完自动收走。速度极快，但你不能挑选座位的长短。
- **堆是“自选包厢”**：需要服务员（`malloc`）根据你报的人数动态开辟空间，吃完必须主动退房（`free`）。空间很大，但手续繁琐。

| 维度       | 栈 (Stack)              | 堆 (Heap)                        |
| :---: | :---: | :---: |
| **分配方式** | **系统自动分配**和释放。         | **程序员手动申请**和释放。                 |
| **申请语法** | C语言中直接定义：`int a = 10;` | C语言中动态申请：`malloc(100);`         |
| **空间大小** | **非常小**。单片机中通常只有几 KB。  | **非常大**。可以占满除去栈和全局变量后剩下的所有 RAM。 |
| **管理结构** | 先进后出（FILO），像一叠盘子。      | 链表/内存块管理，像一堆凌乱的储物柜。             |
| **分配速度** | 极快。CPU 专门有指令寄存器（SP）控制。 | 较慢。需要在内存中寻找足够大的空闲碎片。            |
| **释放风险** | 无风险。函数执行完，系统自动回收。      | 高风险。不手动释放会导致**内存泄漏**。           |

在很多芯片的底层内存布局中，为了最大化利用 RAM，栈和堆是**对向生长**的：

```text
[ 低地址 ] -----------------------------------------------> [ 高地址 ]

| 全局变量区 | ---> 堆 (Heap) 向上生长 ... <--- 栈 (Stack) 向下生长 |
```

- **堆（Heap）** 从低地址往高地址分配。
- **栈（Stack）** 从高地址往低地址压入。

如果你的程序疯狂 `malloc` 不释放（堆满了），同时函数嵌套太深或者局部变量太大（栈满了），它们就会在中间**迎头相撞**，这叫**堆栈冲突**，后果同样是系统崩溃。

## 5.2 MCU 的物理内存空间

在嵌入式系统中，内存不是一个抽象概念，而是真实存在于芯片地址空间中的物理区域。

以 STM32F407 为例，常见内存区域包括：

```text
Flash        ：存放程序代码、只读常量、初始化数据的原始值
SRAM         ：存放运行时变量、栈、堆
CCM SRAM     ：Core Coupled Memory，速度快，但通常不能被 DMA 访问
外设寄存器区  ：GPIO、USART、SPI、I2C、ADC、DMA 等外设寄存器映射区域
系统存储区    ：Bootloader、系统 ROM 等
```

典型地址示意：

```text
Flash       ：0x0800 0000 起
SRAM        ：0x2000 0000 起
CCM SRAM    ：0x1000 0000 起
外设寄存器区 ：0x4000 0000 起
```

需要注意：

> C 语言中的变量、数组、结构体、函数，最终都会被链接器安排到某一块具体的物理内存区域中。

例如：

```c
uint32_t global_value = 10;
```

这个变量在代码中看起来只是一个名字，但编译链接后，它会被安排到某个 SRAM 地址上。

## 5.3 编译出来的固件到底占用了哪里的内存

在 Keil 或 STM32 工程中，经常能看到类似信息：

```c
Program Size: Code=12000 RO-data=800 RW-data=300 ZI-data=5000
```

这些字段反映了程序最终占用了多少 Flash 和 SRAM

### 5.3.1 Code：程序机器码

`Code` 表示程序编译后生成的机器指令，通常存放在 Flash 中。

例如：

```c
void led_on(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
}
```

这段 C 代码会被编译器翻译成 ARM 机器指令，这些指令最终存放在 Flash 中。

特点：

```text
存放位置：Flash
内容：CPU真正执行的机器码
是否掉电丢失：不会
是否运行时修改：通常不会
```
### 5.3.2 RO-data：只读数据

`RO-data` 是 Read Only Data，只读数据。

常见内容包括：

```c
const uint8_t table[] = {1, 2, 3, 4};

const char message[] = "Hello STM32";
```

这些数据通常存放在 Flash 中，因为它们运行时不需要被修改。

需要注意：

```c
const char *str = "Hello";
```

这里要分开看：

```text
"Hello" 字符串字面量：通常在只读区
str 指针变量本身：取决于它定义在哪里
```

如果写在函数内部：

```c
void test(void)
{
    const char *str = "Hello";
}
```

`str` 这个指针变量本身通常在栈上，而 `"Hello"` 字符串内容通常在只读区。

如果写成全局变量：

```c
const char *str = "Hello";
```

指针变量 `str` 本身可能占用 RW-data 或 RO-data，具体取决于修饰方式和编译器安排。

### 5.3.3 RW-data：已初始化的全局变量和静态变量

`RW-data` 是 Read Write Data，表示有初始值、运行时可以修改的全局变量或静态变量。

例如：

```c
int global_value = 10;

static uint8_t mode = 1;
```

这些变量运行时需要放在 SRAM 中，因为程序可能会修改它们。

但是它们的初始值又必须保存在 Flash 里，否则单片机断电后就不知道开机时应该初始化成什么值。

所以 RW-data 有一个很重要的特点：

```text
初始值存放在 Flash
运行时变量实体在 SRAM
启动时由启动文件从 Flash 复制到 SRAM
```

例如：

```c
int global_value = 10;
```

启动时大致发生：

```txet
Flash 中保存 10
        ↓
Reset_Handler 启动时复制
        ↓
SRAM 中的 global_value = 10
```

之后程序修改：

```c
global_value = 20;
```

修改的是 SRAM 中的运行时变量，不会改 Flash 里的初始值。

### 5.3.4 ZI-data：未初始化或初始化为0的全局/静态变

`ZI-data` 是 Zero Initialized Data，表示启动时被清零的变量。

例如：

```c
int global_count;

static uint8_t rx_buffer[128];

uint32_t flag = 0;
```

这些变量都属于运行时需要放在 SRAM 中的数据。

特点：

```text
存放位置：SRAM
初始值：启动时清零
是否占用 Flash：通常不占用实际初始化数据空间
```

例如：

```c
uint8_t buffer[1024];
```

如果它是全局变量且没有显式初始化：

```c
uint8_t buffer[1024];
```

它通常属于 ZI-data。

启动时，启动代码会把这块 SRAM 清成 0。

如果写成：

```c
uint8_t buffer[1024] = {1};
```

那么它就不再是纯 ZI-data，而会涉及 RW-data，因为初始值不是全0。

### 5.3.5 Flash 和 SRAM 的实际占用计算
在 Keil 中通常可以这样理解：

```text
Flash 占用 ≈ Code + RO-data + RW-data
SRAM  占用 ≈ RW-data + ZI-data + Stack + Heap
```

为什么 Flash 中也要包含 RW-data？

因为 RW-data 的初始值要保存在 Flash 中，开机后再复制到 SRAM。

例如：

```text
Code     = 12000
RO-data  = 800
RW-data  = 300
ZI-data  = 5000
```

大致表示：

```text
Flash 至少占用：12000 + 800 + 300 = 13100 字节
SRAM 至少占用 ：300 + 5000 + Stack + Heap
```

其中 Stack 和 Heap 的大小通常由启动文件、链接脚本或工程配置决定，不一定直接显示在 `RW-data + ZI-data` 中。

## 5.4 启动过程中的内存初始化

STM32 上电复位后，并不是直接执行 `main()`。

典型流程是：

```text
芯片复位
    ↓
从中断向量表读取初始栈顶地址
    ↓
进入 Reset_Handler
    ↓
复制 .data 段：Flash → SRAM
    ↓
清零 .bss 段：SRAM 清零
    ↓
系统时钟等底层初始化
    ↓
调用 main()
```

这解释了几个重要现象。

### 5.4.1 为什么全局变量默认是0
例如：

```c
int count;
```

虽然你没有写：

```c
int count = 0;
```

但它启动后仍然是 0。

原因是：

> 启动文件在进入 `main()` 之前，会把 `.bss` 或 ZI-data 区域清零。

### 5.4.2 为什么静态局部变量不会随函数退出销毁

例如：

```c
void test(void)
{
    static int count = 0;

    count++;
}
```

`count` 虽然写在函数内部，但它不是普通栈变量，而是静态存储期变量。

它通常位于：

```text
.data 或 .bss / ZI-data
```

因此函数退出后它不会销毁，下次进入函数时仍然保留上一次的值。

### 5.4.3 为什么有初值的全局变量既占 Flash 又占 SRAM

例如：

```c
uint32_t config_value = 0x12345678;
```

它运行时需要在 SRAM 中被修改，所以占 SRAM。

但开机时它要知道初始值是 `0x12345678`，所以这个初始值还要存放在 Flash 中。

因此：

```text
Flash：保存初始值
SRAM ：保存运行时变量
```
## 5.5 栈 Stack

栈是函数调用时使用的一块内存区域。

它主要保存：

```text
局部变量
函数参数的一部分
返回地址
临时寄存器保存值
中断现场
```


### 5.5.1 局部变量通常在栈上

例如：

```c
void function(void)
{
    int value = 10;
    uint8_t buffer[32];
}
```

`value` 和 `buffer` 通常会在函数进入时创建，函数退出时释放。

流程：

```text
进入 function()
    ↓
栈上分配 value 和 buffer
    ↓
执行函数内容
    ↓
函数返回
    ↓
value 和 buffer 所在栈空间失效
```

所以不能返回局部变量的地址：

```c
uint8_t *get_buffer(void)
{
    uint8_t buffer[32];

    return buffer;   // 错误
}
```

函数返回后，`buffer` 已经失效。

### 5.5.2 栈通常向低地址方向增长

很多 ARM Cortex-M 系统中，栈从高地址向低地址增长。

示意：

```text
高地址
│
│  初始栈顶
│
↓  函数调用、中断进入时栈向下增长
│
│
低地址
```

每次函数调用或中断进入，都会消耗一部分栈空间。

函数返回或中断退出后，栈空间被释放。

### 5.5.3 压栈

压栈（Push）在单片机的物理世界里，本质上就是一套“硬件自动开启的备忘录/寄存器快照机制”。

在 CPU 内部，用来干活的通用寄存器（如 R0~R12）数量是非常有限的。当单片机正在开心地跑 A 任务（比如算算法），突然来了一个紧急的 B 任务（比如物理按键触发了中断，或者调用了一个新函数）时，CPU 必须立刻腾出所有的寄存器去跑 B 任务。

那 A 任务算到一半的数据怎么办？  
CPU 就会启动 **“压栈”** 动作：强行把当前寄存器里的所有数字，像叠盘子一样，物理复制、堆叠到内存（SRAM）中一块专属的、名叫“栈（Stack）”的连续格子里。

等 B 任务跑完了，CPU 再通过**出栈（Pop）**，把刚才按顺序叠在 SRAM 里的数据原封不动地搬回寄存器，A 任务就能像什么都没发生过一样，继续往下算。

在嵌入式开发中，作为高层 C 语言程序员，**几乎不需要亲手去写任何一行“压栈”的汇编指令（如 `PUSH`），所有的压栈动作，都是由【C语言代码隐式驱动】并由【单片机硬件/内核自动完成】的**

场景 1：你在 C 语言里“调用了另一个函数”（软件主动触发）

```c
void Calc_Logic(void) 
{
    int a = 5;        // 👈 此时数字 5 存在 CPU 的 R0 寄存器里
    Delay_Ms(10);     // 👈 核心卡点：调用了另一个函数！
    a = a + 1;
}
```

**如何操作出来的**：当 C 语言编译器看到你调用了 `Delay_Ms` 时，它在编译生成的机器码里，会自动在你跳转的一瞬间插入压栈指令。单片机硬件会**自动把当前 R0 寄存器（里面的 5）以及当前代码运行到的物理地址（PC指针）强行压入栈区**。等到延时函数跑完，再自动出栈还原。

场景 2：外部物理引脚触发了“硬件中断”（硬件被迫触发）

这是单片机和高级内核（如 Cortex-M 内核）最硬核的物理行为。

- **如何操作出来的**：你正在跑 App 层的状态机，突然用户物理按下了复位按键，触发了 GPIO 中断。**此时，内核内部的 NVIC 中断控制器会直接启动纯硬件强行压栈！**
- CPU 甚至不需要执行任何软件代码，硬件会自动把 `R0~R3`、`R12`、`LR`（返回地址）、`PC`（当前程序指针）和 `xPSR`（状态寄存器）等 8 个核心寄存器的快照，一瞬间啪的一下全部强行挤进物理 SRAM 的栈顶空间。这在行业内有一个专有名词，叫**硬件自动压栈（Auto-stacking）**

场景 3：在函数内部定义了“局部变量”

```c
void My_Task(void) 
{
    uint8_t buffer[64];       // 👈 这 64 个字节在内存里的开辟，本质上就是压栈动作！
}
```

- **如何操作出来的**：当函数被调用时，单片机里的**栈指针寄存器（SP，Stack Pointer）** 会自动物理向前挪动 64 个字节的刻度（减去 64）。这就等于强行在栈区为你圈出了 64 个连续的内存格子用来当临时仓库。当函数退出时，SP 指针又会自动往后退 64 个字节（出栈），这 64 个格子物理上瞬间被宣告销毁。

### 5.5.4 压栈的底层物理真相

单片机内部有一个专门用来记录“栈顶到了哪里”的内核寄存器，叫 **SP（`Stack Pointer`，栈指针）**。栈的物理生长方向通常是**向下生长（从高内存地址往低内存地址灌数据）。**

```text
【 物理 SRAM 内存中的栈区（Stack）空间 】

 高地址 (例如 0x20002000)
    │   ┌───────────────────────────┐
    │   │      之前函数压入的数据   │
    │   ├───────────────────────────┤
    │   │      之前函数压入的数据   │
    ▼   ├───────────────────────────┤ ◄─── 压栈前 SP 指针指在这里！
        │  【准备压入：寄存器 R0】  │
        ├───────────────────────────┤
        │  【准备压入：返回地址 PC】│
 Low地址 └───────────────────────────┘ ◄─── 压栈后 SP
```

压栈两步走（硬件指令的内部纯物理动作）：

1. **挪动指针**：SP 指针地址自动减 4（在 32 位系统里，一个寄存器占 4 字节，指针向低地址挪一步）。
2. **强灌物资**：内核把寄存器里的数字（比如 `R0` 里的 `5`），通过系统内部的高速通道，强行写进当前 SP 指针所指向的那个物理 SRAM 内存格子里。

### 5.5.5 函数调用时的压栈

当函数调用另一个函数时，CPU和编译器可能需要保存：

```text
返回地址
部分寄存器
局部变量
临时数据
```

例如：

```c
void function_a(void)
{
    function_b();
}
```

大致流程：

```text
function_a 正在运行
    ↓
调用 function_b 前保存现场
    ↓
跳转到 function_b
    ↓
function_b 执行完成
    ↓
恢复现场
    ↓
回到 function_a
```

### 5.5.6 中断发生时的压栈

在 Cortex-M 中，中断发生时，硬件会自动保存一部分现场。

典型会压入：

```
R0
R1
R2
R3
R12
LR
PC
xPSR
```

如果使用 FPU，还可能涉及更多浮点寄存器保存。

这意味着：

> 中断不是免费的，中断越深、局部变量越大、调用函数越多，栈消耗越大。

### 5.5.7 什么是栈溢出

**栈溢出就是栈空间不够用了**。

常见原因：

```text
函数中定义过大的局部数组
递归调用过深
中断嵌套过多
任务栈分配过小
函数调用链太深
```

例如：

```c
void bad_function(void)
{
    uint8_t buffer[10000];
}
```

在 PC 上可能没什么，但在只有几十 KB SRAM 的 MCU 上，这种局部数组可能直接导致栈溢出。

栈溢出的后果包括：

```text
覆盖全局变量
覆盖堆
进入 HardFault
程序莫名复位
变量值莫名改变
任务异常卡死
```
### 5.5.8 为什么驱动和中断中不应该定义过大局部数组

中断函数应尽量短小。

不推荐：

```c
void USART1_IRQHandler(void)
{
    uint8_t temp_buffer[1024];

    process_data(temp_buffer);
}
```

原因：

```text
中断栈空间有限
中断执行时间变长
可能造成栈溢出
影响其他中断响应
```

更推荐：

```c
static uint8_t temp_buffer[1024];
```

或者把缓冲区放在模块级：

```c
static uint8_t usart_rx_buffer[1024];
```

但这样也要注意：

```text
static 缓冲区长期占用 SRAM
多上下文访问需要同步保护
DMA访问需要考虑对齐和Cache一致性
```

所以最终选择要看实际需求。

## 5.6 堆 Heap 与动态内存分配

堆是运行时用于动态分配内存的一块区域。

常见接口：

```c
malloc()
calloc()
realloc()
free()
```

例如：

```c
uint8_t *buffer = malloc(128);

if (buffer != NULL)
{
    use_buffer(buffer);

    free(buffer);
}
```

### 5.6.1 malloc 和 free 的作用

```c
malloc(size)
```

作用：

```text
从堆中申请 size 字节内存
成功返回内存首地址
失败返回 NULL
```

```c
free(ptr)
```

作用：

```text
释放之前申请的内存
让这块内存重新回到堆管理器
```

### 5.6.2 在单片机中使用 malloc 的代价

在嵌入式中，`malloc()` 并不是不能用，而是要非常谨慎。

风险包括：

```text
分配失败
执行时间不确定
内存碎片
内存泄漏
多任务并发安全问题
调试困难
实时性变差
```

例如：

```c
uint8_t *buffer = malloc(1024);
```

如果堆空间不足，`malloc()` 会返回：

```c
NULL
```

如果不检查：

```c
buffer[0] = 1;
```

就可能访问非法地址，导致 `HardFault`。

正确写法：

```c
uint8_t *buffer = malloc(1024);

if (buffer == NULL)
{
    return ERROR_NO_MEMORY;
}
```

### 5.6.3 内存泄漏 Memory Leak

内存泄漏是指：

> 申请了内存，但后续没有释放，而且程序再也找不到这块内存。

例如：

```c
void bad_function(void)
{
    uint8_t *buffer = malloc(128);

    if (buffer == NULL)
    {
        return;
    }

    process(buffer);

    // 忘记 free(buffer)
}
```

如果这个函数反复调用，堆中的可用空间会越来越少，最后 `malloc()` 失败。

在长期运行的嵌入式系统中，内存泄漏非常危险，因为设备通常不会像电脑软件那样频繁重启。


### 5.6.4 内存碎片 Fragmentation

内存碎片是指：

> 堆里总剩余空间看起来还够，但被切成很多小块，无法满足一次较大的连续内存申请。

例如堆空间如下：

```text
[已用 100][空闲 50][已用 100][空闲 50][已用 100][空闲 50]
```

总空闲空间是：

```text
150 字节
```

但如果你申请：

```c
malloc(120);
```

可能失败，因为没有一块连续的 120 字节空间。

这就是碎片化。

在实时嵌入式系统中，碎片化比单纯空间不足更难排查。

### 5.6.5 为什么很多裸机项目避免动态内存

很多小型 MCU 项目更喜欢：

```text
静态数组
全局缓冲区
固定大小内存池
环形缓冲区
对象池
```

例如：

```c
static uint8_t rx_buffer[256];
static uint8_t tx_buffer[256];
```

优点：

```text
内存占用编译期可见
不会分配失败
没有碎片
实时性稳定
调试简单
```

缺点：

```text
不够灵活
长期占用 SRAM
缓冲区大小需要提前估计
```

## 5.7 静态内存、内存池和对象池

### 5.7.1 静态分配

静态分配是最简单、最稳定的方式。

例如：

```c
static uint8_t uart_rx_buffer[256];
static uint8_t spi_tx_buffer[512];
static uint16_t adc_buffer[128];
```

优点：

```text
编译期确定大小
不会运行时分配失败
不会产生碎片
适合裸机驱动
```

缺点：

```text
不灵活
长期占用内存
大小不好估计
```

### 5.7.2 内存池 Memory Pool

内存池是提前划分好一大块内存，然后运行时从中申请固定大小的小块。

例如：

```text
内存池总大小：1024 字节
每块大小：64 字节
总块数：16 块
```

优点：

```text
分配速度快
不会产生传统意义上的碎片
容易统计剩余块数
适合频繁申请释放的小对象
```

缺点：

```text
块大小固定
大对象不适合
小对象可能浪费空间
```

### 5.7.3 对象池 Object Pool

对象池是专门为某一种结构体对象准备的池。

例如：

```c
typedef struct
{
    uint8_t used;
    uint8_t data[64];
} Packet;

static Packet packet_pool[16];
```

申请时找一个未使用对象：

```c
Packet *packet_alloc(void)
{
    for (uint8_t i = 0; i < 16; i++)
    {
        if (packet_pool[i].used == 0)
        {
            packet_pool[i].used = 1;
            return &packet_pool[i];
        }
    }

    return NULL;
}
```

释放：

```c
void packet_free(Packet *packet)
{
    if (packet != NULL)
    {
        packet->used = 0;
    }
}
```

对象池常用于：

```text
协议包
消息节点
事件对象
任务控制块
驱动请求对象
```

## 5.8 FreeRTOS 的内存分配策略

FreeRTOS 中内存主要分两类：

```text
任务栈
内核对象内存
```

内核对象包括：

```text
任务控制块 TCB
队列
信号量
互斥锁
事件组
软件定时器
```

这些对象可以通过动态方式创建，也可以通过静态方式创建。

### 5.8.1 每个任务都有自己的栈

在裸机程序中，通常主要使用一个主栈。

在 FreeRTOS 中，每个任务都有独立的任务栈。

例如：

```c
xTaskCreate(
    task_led,
    "led",
    128,
    NULL,
    2,
    NULL);
```

这里的 `128` 是任务栈深度。

需要注意：

> 在很多 FreeRTOS 移植中，任务栈深度单位不是字节，而是 `StackType_t` 的个数，常见32位平台上一个单位通常是4字节。

所以：

```text
128 个 StackType_t ≈ 512 字节
```

具体要看平台移植和配置。

### 5.8.2 任务栈里放什么

任务栈用于保存：

```text
任务局部变量
函数调用现场
任务切换现场
中断返回相关上下文
部分临时数据
```

例如：

```c
void task_sensor(void *argument)
{
    uint8_t buffer[512];

    while (1)
    {
        sensor_read(buffer);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

这里的 `buffer[512]` 会占用该任务的栈空间。

如果任务栈只分配了 512 字节，那这个数组本身就可能把栈占满，导致任务栈溢出。

更推荐：

```c
static uint8_t sensor_buffer[512];

void task_sensor(void *argument)
{
    while (1)
    {
        sensor_read(sensor_buffer);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

但这又引入共享资源问题，需要根据实际情况加锁或设计所有权。

### 5.8.3 FreeRTOS 动态创建对象

例如动态创建任务：

```c
xTaskCreate(task_led, "led", 128, NULL, 2, NULL);
```

FreeRTOS 内部通常需要分配：

```text
任务控制块 TCB
任务栈
```

动态创建队列：

```c
xQueueCreate(10, sizeof(uint32_t));
```

内部需要分配：

```text
队列控制结构
队列存储区
```

这些内存通常来自 FreeRTOS 的堆：

```c
pvPortMalloc()
vPortFree()
```

而不是一定直接使用标准库的 `malloc/free`。

### 5.8.4 FreeRTOS 的 `heap_1` 到 `heap_5`

FreeRTOS 提供多种堆管理方案。

常见有：

```text
heap_1：只分配，不释放，最简单
heap_2：支持释放，但容易碎片化
heap_3：封装标准 malloc/free
heap_4：支持合并相邻空闲块，最常用
heap_5：支持多个不连续内存区域
```

简单理解：

| 方案     | 特点                | 适合场景           |
| ------ | ----------------- | -------------- |
| heap_1 | 只能申请，不能释放         | 对象只创建不删除       |
| heap_2 | 能释放，但不合并碎片        | 较少使用           |
| heap_3 | 使用标准库 malloc/free | 依赖C库堆          |
| heap_4 | 能释放，能合并空闲块        | 常用方案           |
| heap_5 | 支持多块内存区域          | 内部SRAM + 外部RAM |

对于初学 STM32 + FreeRTOS，常见选择是：

```c
heap_4.c
```

因为它比 heap_1 灵活，又比 heap_3 更可控。


### 5.8.5 `configTOTAL_HEAP_SIZE`

FreeRTOS 的动态内存池大小通常通过：

```c
#define configTOTAL_HEAP_SIZE    (20 * 1024)
```

配置。

这表示 FreeRTOS 内核对象和动态任务栈可以使用的堆空间总大小。

如果任务、队列、信号量创建失败，可能就是：

```text
configTOTAL_HEAP_SIZE 太小
```

或者 SRAM 本身不足。

### 5.8.6 静态创建任务和队列

老工程师常喜欢静态分配，因为更稳定、更可控。

动态任务创建：

```c
xTaskCreate(task_led, "led", 128, NULL, 2, NULL);
```

静态任务创建：

```c
static StackType_t led_task_stack[128];
static StaticTask_t led_task_tcb;

xTaskCreateStatic(
    task_led,
    "led",
    128,
    NULL,
    2,
    led_task_stack,
    &led_task_tcb);
```

静态方式中：

```text
任务栈由用户提供
任务控制块由用户提供
FreeRTOS 不需要从堆中动态申请
```

优点：

```text
不会动态分配失败
不会产生堆碎片
内存占用编译期可见
适合安全性和可靠性要求高的项目
```

缺点：

```text
写法更复杂
灵活性差
对象数量通常固定
```


# 6 附录
## 6.1 英语缩写
ACK （Acknowledge）：确认

AFIO （Alternate Function Input Output）：复用功能输入输出

AHB （Advanced high performance bus）：高级高性能总线

APB （Advanced Peripheral Bus）：先进外设总线

Arg （argument）：自变量

ARR （Auto-Reload Register）：自动重装载值

CCR （Capture/Compare Register）：捕获/比较寄存器

CK_CNT_OV （Clock Counter Overflow Frequency）：溢出频率

CMD （Command Prompt）：控制

CNT （Counter Clock）：计数时钟

Dec （decrement）：渐减

DMA （direct memory access）：直接存储器存取

DR （data register）：数据寄存器

Enum （enumeration）：列举

EOC （end of conversation）：转换结束（标志位）

ETR  (External Trigger)：外部触发

GPIO （General Purpose Input Output）：通用输入输出口

Impedance：阻抗

Inc （increment）：渐增

Init （initialization）：初始化

Int （integer）：整数

IRQ （Interrupt Request）：中断请求

IT （Interrupt）：中断

MDK （Micro controller Development Kit）：微控制器开发套件

NVIC （Nested Vectored Interrupt Controller）：嵌套向量中断控制器

OC （Output Compare）：输出比较

OLED （Organic Light Emitting Diode）：有机发光二极管

PERIPH （Peripheral）：外设

PSC （Prescaler）：预分频器

RAM  (Random Access Memory)：随机存取存储器

RCC （Reset and Clock Control）：复位与时钟控制

Reg （register）：寄存器

ROM (Read-Only Memory)：只读存储器

RX （receiver）：接收

SCL （Serial Clock）：时钟线

SDA （Serial Data）：数据线

TRGO (Trigger Output)：触发外部输出

TX （transmitter）：发送

VCC （Voltage Collector Collector）：代表电路的供电电压

## 6.2 英语全称

Analog：模拟的

Brief：摘要

Buffer：缓冲

Buzzer：蜂鸣器

Cache: 高速缓冲区

Calibration：校准

Casting：铸件

Combination：组合

Compatible:兼容的

Config/configure：布局

Inject：注入

Latch：锁存器

Misc：混杂的

Parameter：参数

Parity：平等、奇偶校验

Pending：待定的

Peripheral：外设

Pointer：指针

Port pin：端口

pre-emption priority：先占优先

Prescaler：预分频器

Pulse：脉冲

Retval：返回值

Sensor：传感器

Serial：序列的

Servo:伺服

Slave：从动装置
