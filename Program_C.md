# 1 C语言基本数据类型以及大小

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


# 2 int main (void) 函数

**这是 C 语言程序的入口**，也是整个程序的起跑线。简单来说：单片机上电复位后，干完必要的杂活（启动文件），就会直奔这里，开始执行你写的代码。

## 2.1 main：主角（函数名）
   
含义：**这是“主函数”的名字**。

地位：它是特殊的、唯一的。编译器在编译代码时，会满世界找叫 main 的函数。找到了，就把它设为程序开始的地方。

注意：你不能随便给它改名（比如改成 start 或 begin），否则编译器会报错，因为它找不到“带头大哥”了。

对于函数，C 语言规定：**函数名本身，就直接代表它在内存中的首地址！**

## 2.2 void：参数列表

位置：括号里的 void。

含义：表示这个函数不接受任何参数。

通俗理解：这就像老板（系统）命令这个主函数去干活，但不需要给它任何额外的文件或指令。它启动就是了。

对比：在电脑编程（Windows/Linux）中，你可能见过 int main(int argc, char * argv[ ])，那是为了接收命令行参数（比如你运行程序时输入的命令）。但在嵌入式（STM32）里，通常没人给它传参，所以是 void。

## 2.3 int：返回类型

位置：最前面的 int。

含义：表示这个函数执行结束后，会返回一个整数 (integer) 给调用它的系统。

标准写法：**通常在函数的最后写一行 return 0;**

0：通常代表“程序成功执行，完美结束”。

**非0：代表“程序出错了”**。

## 2.4 举例

场景 A：电脑软件 (Windows/Linux)

流程：你双击软件 -> 软件运行 -> 你点击关闭 -> main 函数执行到最后 -> return 0; -> 告诉 Windows 系统“我关掉了，一切正常”。

int 的作用：真的有作用，用来给操作系统汇报工作状态。

场景 B：嵌入式系统 (STM32/Keil)

流程：单片机上电 -> 跳到 main -> 进入 while(1) 死循环 -> 永远在循环里跑。

int 的作用：完全是个摆设！因为单片机程序是不能“退出”的（退出了单片机就瘫痪了）。程序永远跑不到最后的 return 0; 那一行。

那为什么还要写 int？ 主要是为了符合 C 语言的标准语法规范。如果不写 int 而写 void main()，有些严格的编译器会发出警告（Warning），看着心烦，所以大家约定俗成写 int。

# 3 Struct 结构体

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


## 3.1 Structure就是一个“快递包裹”

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


## 3.2 Pointer：就是“门牌号”

(1)什么是指针？

普通变量：存的是内容。（比如：抽屉里放着一个苹果）

**指针变量：存的是地址。（比如：纸条上写着“苹果在301号抽屉”）**。


(2)核心符号：

'&' (取地址符)：获取某个变量在内存中的地址。即“它在哪？”

'*' (解引用符)：通过地址去拿里面的东西。即“去这个地方拿东西”。
  
(3)代码示例：

``` c
int a = 100;          //一个普通的整数

int *p;               //定义一个指针p(专门用来存地址)

p = &a;               //把 a 的地址存进p(现在 p指向a)
//*p就等同于 a
*p = 200;
//意思就是:去p指向的地址，把那里的数字改成208
//现在a 的值也变成了200
```

## 3.3 结构体 + 指针：嵌入式的灵魂

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

## 3.4 ADC1 -> DR

1.物理层面的含义：**数据的“终点站”**

ADC (Worker)：是负责干活的模数转换器。它在后台默默地把电压变成数字。

-> (Flow)：代表“写入”或“传送”的动作。

DR (Data Register)：数据寄存器。这是 ADC 模块对外展示结果的唯一窗口。

“ADC -> DR” 的意思就是： ADC 硬件内部电路转换完电压后，自动把算出来的那个数字（比如 2048），搬运并存储到 DR 寄存器里去。

形象比喻：ADC 是厨房里的厨师。DR 是厨房门口的出餐台。ADC -> DR：就是厨师把做好的菜（数据），端到了出餐台上。

CPU（你）平时进不去厨房，你看不到厨师是怎么炒菜的。你只能去出餐台 (DR) 拿菜。

2.代码层面的含义：**结构体指针**

在 STM32 的 C 语言编程中，-> 是结构体指针成员访问符。

->箭头操作符是一个二合一的超级快捷键，在其内部就已经包含了*的功能

A−>B≡(∗A).B

'* A：表示“解引用”，即取出 A 指向的那个结构体（进大门）。

. B：表示“成员访问”，即找到结构体里的 B 成员（进房间）。

ADC1 -> DR编码器会把他翻译为 （* ADC1）. DR   * ADC1：先用钥匙打开 ADC1 这个大楼的门（完成了你想要的“取内容”操作）。.DR：然后直接走到 DR 这个房间，拿走里面的数据。结论： 因为 -> 已经帮你把“开门（取内容）”这件事做了，所以不需要再手动加一个 *。

ADC1 是一个指针，指向 ADC1 外设在内存中的基地址（大本营）。DR 是这个结构体里的一个成员（偏移地址）。

当写代码 Value = ADC1->DR; 时，在对 CPU 说： “请去 ADC1 的大本营，找到那个叫 DR 的房间，把里面的数字读出来给我。”

# 4 Typedef （定义类型）

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

## 4.1 定义变量

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

## 4.2 定义结构体

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

## 4.3 Handle 类型

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

## 4.4 定义枚举类型

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

## 4.5 定义指针类型

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

## 4.6 定义数组类型

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

## 4.7 定义函数指针类型

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

## 4.8 回调函数类型

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

## 4.9 `typedef` 和 `#define` 的区别

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

# 5 Pointer 指针

## 5.1 指针(Pointer)

指针是C语言的一个重要知识点，其使用灵活、功能强大，是C语言的灵魂。**指针与底层硬件联系紧密，使用指针可操作数据的地址，实现数据的间接访问**。

指针即指针变量，**用于存放其他数据单元（变量/数组/结构体/函数等）的首地址**。若指针存放了某个数据单元的首地址，则这个指针指向了这个数据单元，若指针存放的值是0，则这个指针为空指针。

指针就是一张写着地址的小纸条，想象内存就是一个巨大的大酒店，里面有成千上万个房间。

变量 (int a = 100;)： 你在酒店开了一间房，房间里住着一个叫 100 的客人。为了方便，你给这个房间起个别名叫 a。地址 (&a)： 虽然你叫它 a，但在酒店管理系统（操作系统）里，它真正的名字是 房间号（比如 0x7ffee000 这种奇怪的十六进制数字）。

**指针变量，就是用来专门记房间号的笔记本**。

普通变量 (int a)：存的是数据（比如 100）。

指针变量 (int* p)：存的是地址（比如 0x7ffee000）。

比喻：a 是房间，p 是房卡（或者写着房间号的纸条）。

## 5.2 定义一个指针变量

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


## 5.3 指针的内存和步长

1)内存：

一个指针分配8个字节的内存，里面存储地址，因为现在的内存太大了，导致“门牌号”数字变得超级长，4 个字节写不下了。8 个字节，仅仅是为了完整地写下那一个“首地址”的编号。

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

 char* p = name，**数组名等于第0个元素的地址**，赋给指针之后，指针的内部就会存储第0个元素的地址，*p就代表“D”
 
 p++ 代表指针增加一个步长，因为是char类型，所以代表当前地址+1，对应的 * p就应该表示“a”

## 5.4 指针的操作

若已定义：
```c
	int a;		//定义一个int型的数据
	int *p;		//定义一个指向int型数据的指针
```
则对指针p有如下的操作方式

|操作方式|	举例|	解释|
|:---:|:---:|:---:|
|取地址	|P = &a;|	将数据a的首地址赋值给p|
|取内容	|* p;	|取出指针指向的数据单元|
|加	|P ++;	|使指针向下移动1个数据宽度|
|	|P = p + 5;|	使指针向下移动5个数据宽度|
|减	|P --;	|使指针向上移动1个数据宽度|
|	|P = p - 5;	|使指针向上移动5个数据宽度|

* * 在C语言中有三个功能
 
(1)A * B 代表两个数的相乘

(2)Int p  * 出现在变量类型后面（int char等），表示是int类型，代表是一个int类型的指针

(3) * p  在的前面没有变量类型，表示取内容，取出指针指向的数据单元

## 5.5 例子解析
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

## 5.6 数组与指针

数组是一些相同数据类型的变量组成的集合，其数组名即为指向该数据类型的指针。数组的定义等效于申请内存、定义指针和初始化

<img width="515" height="197" alt="image" src="https://github.com/user-attachments/assets/8094735f-051b-4018-9880-9a4fcb8c8454" />


当你定义一个数组 int arr[5]; 时，内存里发生了两件事：1.分配空间： **系统划出了一块连续的内存（能存5个整数）**。2.生成标签： **arr 这个名字，其实就代表了这块内存首元素的地址**。数学公式：arr≡&arr[0](数组名等于第0个元素的地址)

平时用的 arr[2]，其实是写给人类看的简化写法。编译器在编译时，会把它无情地翻译成指针运算。翻译公式：arr[i]⇔∗(arr+i).arr:首地址（起点）。+ i：向后跳 i 步（回忆一下，因为是 int 数组，每一步跳 4 字节）。* (...)：解引用，取出那个地方的值。

这也是为什么数组下标从 0 开始：arr[0]→∗(arr+0) 起点往后跳 0 步 → 就是起点本身。如果从 1 开始，电脑就需要多做一次减法运算 * (arr + i - 1)，效率就低了。

## 5.7 注意事项

在对指针取内容之前，一定要确保指针指在了合法的位置，否则将会导致程序出现不可预知的错误

同级指针之间才能相互赋值，跨级赋值将会导致编译器报错或警告

<img width="515" height="197" alt="image" src="https://github.com/user-attachments/assets/c9792c80-c975-448e-95c4-33b1b0e8f238" />

## 5.8 指针的应用
(1)传递参数

**使用指针传递大容量的参数**，主函数和子函数使用的是同一套数据，避免了参数传递过程中的数据复制，提高了运行效率，减少了内存占用

使用指针传递输出参数，利用主函数和子函数使用同一套数据的特性，实现数据的返回，可实现多返回值函数的设计

(2)传递返回值

将模块内的公有部分返回，让主函数持有模块的“句柄”，便于程序对指定对象的操作

(3)直接访问物理地址下的数据

**访问硬件指定内存下的数据**，如设备ID号等

将复杂格式的数据转换为字节，方便通信与存储

## 5.9 示例
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

## 5.10 高级表达

int* p＝arr

等价于int* p = &arr[0];

等价于int*p;  p＝&arr [0]; 因为数组名本身就等于第0个元素的地址

## 5.11 用指针传参


# 6 Function pointer（函数指针）

在纯 C 语言（尤其是底层、嵌入式、驱动开发）中，使用函数指针的核心目的只有四个字：**动态调用**。

如果不用函数指针，C 语言的函数调用都是死板、硬编码（写死）的。而有了函数指针，**程序就可以在运行时根据不同的情况，动态地决定去执行哪段代码**,主要针对回调函数使用

## 6.1 什么是函数指针
普通指针保存的是**数据地址**：

```c
int value = 10;int *ptr = &value;
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

## 6.2 函数指针声明

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

## 6.3 函数指针的赋值

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

## 6.4 使用 typedef 简化函数指针

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

## 6.5 使用示例

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

# 7 变量作用域

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

# 8 Keyword（关键字）

关键字（keyword）是由 **C 语言标准预先规定、具有特殊语法含义的单词，不能作为变量名、函数名或结构体成员名**

在C90中有32个对应的关键字，之后还有C99以及C11的新增关键字
```
auto             break        case        charconst       continue    default     dodouble          else        enum        externfloat       for         goto       ifint             long        register    returnshort       signed      sizeof     staticstruct      switch      typedef     unionunsigned     void        volatile   while
```

## 8.1 数据相关的关键字
char、 int、 short、 long、 sighed、 unsigned、 float、 double、  void、 bool 、 complex（复数）、 imaginary（虚数）

## 8.2 流程控制关键字
if、 else、 case 、switch、 default、 for、 while、 do、 break、 continue、 goto、 return

## 8.3 自定义数据结构关键字
Struct、 union、 enum

## 8.4 存储类别关键字
auto、 register、 static、 extern、 typedef

## 8.5 类型限定相关关键字
const、 volatile、 restrict、 automic


# 9 Static

`static`在不同位置含义不同，主要有三种用途：

1. 局部静态变量：**延长生命周期**；

2. 文件级静态变量：**限制在当前 `.c` 文件**；

3. 静态函数：**限制函数只能在当前 `.c` 文件调用**。

## 9.1 局部变量中的static

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

## 9.2 文件中的static

假设在 `uart.c` 中写：

```c
static uint8_t rx_buffer[128];
```

这里的 `static` 表示：

 `rx_buffer` 只属于 `uart.c` 这个编译单元，**其他 `.c` 文件不能通过 `extern` 正常访问它**。

这叫作**内部链接**，其他的文件不可以直接使用这个变量，这种设计更安全，因为其他模块无法随意修改 UART 模块内部状态。

## 9.3 静态函数
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


# 10 Const
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

## 10.1 const与指针
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

# 11 Volatile

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

# 12 Enum （enumeration）（枚举）

## 12.1 定义
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

## 12.2 在enum中赋值

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


# 13 While 循环

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

# 14 Switch...case

## 14.1 定义
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

## 14.2 基本语法

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

## 14.3 `switch` 和 `if...else` 的区别

### 14.3.1 适合使用 `switch` 的情况

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

### 14.3.2 适合使用 `if...else` 的情况

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

# 15 State Machine（状态机）

## 15.1 定义

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

## 15.2 状态机的四个基本组成部分

### 15.2.1 状态

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

### 15.2.2 事件

促使状态发生变化的事情。

例如：

```
按下启动按钮
收到串口命令
定时器超时
传感器检测到故障数据
接收完成
```

### 15.2.3 转移条件

决定是否切换状态的判断条件。

例如：

```c
if (start_button_pressed())
{    
	state = STATE_RUNNING;
}
```

### 15.2.4 动作

在某个状态下执行的操作。

例如：

```c
motor_start();
led_on();
uart_send_data();
```

## 15.3 最简单的状态机代码

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

## 15.4 状态机和普通流程代码的区别

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

# 16 ‘#’
‘#’是预处理器（Preprocessor） 的信号灯。当编译器真正开始编译代码之前，预处理器会先扫描一遍代码。每当它看到 ‘#’，它就知道：“这是给我的指令，我要在编译器接入之前处理它。”

|符号/指令	|名称|	作用|	典型场景|
|:---:|:---:|:---:|:---:|
|#include	|包含	|导入头文件|	几乎所有 C++ 程序|
|#define	|定义	|文本替换/宏定义	|常量、简单的内联逻辑|
|# (在宏中)	|字符串化|	将参数变为字符串 "..."	|调试日志、反射辅助|
|## (在宏中)	连接|	将两个代码片段拼成一个	|自动生成变量名|
|# ifndef / '#endif'|	条件编译	|控制代码块的开启/关闭	|跨平台兼容、头文件保护|
|#pragma	|杂项指令	|编译器特有控制|	'#pragma once'|

## 16.1 '#Include' 包含
它的本质就是由编译器自动执行的复制粘贴

在编译器真正开始翻译代码（编译）之前，有一个叫“预处理器”的小助手会先跑一遍。**当它看到 '#include' 时，它会做以下动作：找到那个文件名（比如 stdio.h 或 led.h）；把那个文件里的所有内容复制出来；原地覆盖掉 '#include' 这一行**。

## 16.2 '#define' 宏定义

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

## 16.3 '#ifndef' 避免重复定义错误
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

# 17 Extern 声明

extern 的意思是：“请注意，这个变量（或函数）已经在别的文件里定义过了，这里只是引用它，请编译器去别处找它的真身。”

extern 是什么：**它是连接多个 C 文件的桥梁**。

什么时候用：当需要在 A.c 里使用 B.c 里定义的全局变量时。

怎么用（最佳实践）：

在 B.c 中定义变量：int count = 0;

在 B.h 中声明变量：extern int count;

在 A.c 中 # include "B.h"。


# 18 Printf输出

1. C语言输出规则主要由 printf( ) 函数控制，核心是格式控制字符串，包含普通字符（原样输出）、转义字符（如\n换行、\t制表），以及最关键的格式说明符（如%d(Decimal十进制)整数、%f浮点数、%s字符串）来指定数据类型和显示方式。输出格式支持宽度、精度、对齐方式（左对齐-、右对齐）等修饰，例如%5d输出整数占5位，%.2f输出两位小数。

2. 格式：printf("格式控制", 输出列表);

- 格式控制字符串: 双引号内的部分。

- 输出列表: 用逗号分隔的变量或常量。

3. 格式说明符 
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

  
4. 修饰符（放在%和类型之间）

- 宽度 (m): %m.nf，指定最小输出宽度，不足则补空格。
	
- 精度 (n): %.nf，指定小数位数，%.ns指定字符串取前n个字符。
	
- 标志:
	-: 左对齐
	
	+: 正数显示+号。
	
	0: 用前导零填充宽度。 


5. 示例
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

# 19 Handle（句柄）

`Handle`通常翻译为：

- 句柄；
- 设备句柄；
- 控制句柄；
- 上下文对象。

> 把一个设备的硬件地址、配置参数、运行状态、缓冲区、错误码和回调函数集中保存在一个对象中。

例如：

```c
typedef struct
{    
	USART_TypeDef *USARTx;    
	uint32_t baud_rate;    
	const uint8_t *tx_buffer;    
	uint16_t tx_length;    
	volatile uint16_t tx_count;    
	uint8_t *rx_buffer;    
	uint16_t rx_length;    
	volatile uint16_t rx_count;    
	volatile uint8_t state;    
	volatile uint32_t error_code;
} UART_Handle;
```

定义两个实例：

```c
UART_Handle uart1;
UART_Handle uart2;
```

虽然类型相同，但表示两个不同的 UART 软件上下文：

```
uart1 → 管理 USART1
uart2 → 管理 USART2
```

所以可以这样理解：

```
UART_Handle 是设备管理档案的格式uart1 是 USART1 的具体档案uart2 是 USART2 的具体档案
```

# 20 Callback（回调函数）

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

## 20.1 函数指针
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

## 20.2 回调函数的三个基本步骤

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

```
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
# 21 ‘ ’和“ ”的区别
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

举例：char str[ ]=”A”;

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


|特性|	'A' (单引号)|	"A" (双引号)|
|:---:|:---:|:---:|
|类型	|char (字符/整数)	|char* 或 char[] (地址/数组)|
|内存样子|	[ 65 ]	|[ 65 ] [ 0 ] (自动补了 \0)|
|大小	|1 字节	|2 字节|
|生活类比|	一颗子弹	|一个只装了一颗子弹的弹夹|

# 22 编写.h和.c文件

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

# 23 GPIO_Init(..., ...)和 void GPIO_Init(..., ...)

(1)GPIO_Init(..., ...);

这是：函数调用 (Function Call)

含义：“现在，立刻，马上去执行这件事！”

位置：必须写在函数体（比如 main() 或 GPIO_Init()）的大括号 { } 里面。

作用：CPU 执行到这一行时，会真的去设置寄存器，把GPIO重新配置

(2)void GPIO_Init(..., ...);

这是：函数声明/定义... 

这里有个核心细节：如果你在括号里填的是 GPIOB 这种具体的值，而前面加了 void，这通常是错误的写法。

①如果在 main 函数里这样写是错误的

void 是用来定义新函数的。
在 main 函数里，不需要定义这个函数（ST公司已经定义好了），你只需要使用它。
加了 void，编译器以为你想在 main 里面再创造一个函数，但语法又不对，所以报错。
②在 .h 头文件或帮助文档

注意区别：这里括号里写的是 类型 (uint8_t)，而不是具体的值 (GPIOB)。

含义：这是在告诉编译器，“有一个叫 GPIOx 的函数，它不返回值 (void)，它需要两个 uint8_t 类型的参数”。

|写法|	比喻|	电脑的反应|
|:---:|:---:|:---:|
|void GPIO_Init(...)|	一本菜谱	|“哦，我知道这就叫‘做这道菜的方法’，但我不会现在去做。”|
|GPIO_Init(...)|	给厨师下个单	|“好的！我这就去把这道菜做出来！”|

# 24 (GPIO_TypeDef *)强制类型转换

STM32 的 GPIOA 硬件实际上就住在一个固定的内存地址上，比如 0x40010800。但在编译器眼里，0x40010800 只是一个枯燥的十六进制整数 (Integer)，就像 100 或 999 一样。

(GPIO_TypeDef *) 的作用：给数字“穿制服”

‘# define GPIOA   ( (GPIO_TypeDef * ) 0x40010800 )’

0x40010800：原本只是一个单纯的数字（物理基地址）。

(GPIO_TypeDef *)：给这个数字穿上了“结构体指针”的制服。

结果：**现在编译器认为 GPIOA 是一个合法的指针了**。

使用：所以你才能写 GPIOA->ODR。编译器会想：“哦，GPIOA 是个指针，指向一个结构体，那我去找这个地址偏移量为 ODR 的地方写数据。”

形象比喻：

0x40010800：这就像是地图上的一个经纬度坐标 (116.40, 39.90)。

(GPIO_TypeDef *)：这就像是在这个坐标上插了一个牌子，上面写着 “紫禁城”（而且规定了紫禁城里有太和殿、中和殿等布局）。

转换后：你不再说“去坐标 (116.40, 39.90)”，而是说“去紫禁城的太和殿 (GPIOA->ODR)”。

公式含义：纯数字地址→具有具体功能的结构体指针

# 25 Unit_16 和 int
Unit_16代表unsigned 16即无符号的16位（2字节）

int 是个“变色龙” (可移植性问题)

这是最大的坑。**在 C 语言标准中，int 的长度是不固定的，它取决于编译器和 CPU 的位数**。在 51 单片机 / Arduino Uno (8位/16位处理器) 上：int 通常是 16位 (2字节)。在 STM32 (32位 ARM 处理器) 上：int 通常是 32位 (4字节)。在 电脑 (64位处理器) 上：int 也是 32位，但在某些古老或特殊的系统上可能不同。

后果： 如果你写了 int a; 并且假设它是 16 位的，当你把代码从 Arduino 移植到 STM32 时，内存占用突然翻倍了；反之，如果从 STM32 移植到 8 位机，原本能存 100000 的变量突然溢出了（因为 16 位最大只能存 32767）。

相比之下 **uint16_t： 不管你在什么 CPU 上，它永远、绝对、一定是 16 位 (2字节)**。这给底层驱动开发提供了绝对的安全感。

|特性|	int|	uint16_t|	评价|
|:---:|:---:|:---:|:---:|
|大小	|看心情 (2字节或4字节)|	固定 (2字节)	|uint16_t 更可靠|
|符号	|有符号 (包含负数)	|无符号 (全是正数)	|硬件参数通常不需要负数|
|最大值	|32767 (如果是16位)	|65535	|uint16_t 容量更大|
|用途	|算个加减法，做个循环变量|	配置寄存器，定义硬件接口|	专业对口|

# 26 运算符
## 26.1 位运算符

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

## 26.2 逻辑运算符

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

## 26.3 >>和<<

'>>'右移运算符

'<<'左移运算符  **箭头的方向代表移动的方向**

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

# 27 智能提示的快速理解

<img width="626" height="22" alt="image" src="https://github.com/user-attachments/assets/bc17be69-04d6-41c5-b748-d2d0b287d287" />

拆成三部分理解，函数名和两个参数

函数名为GPIO_ResetBits，这个函数的作用是让某个引脚输出低电平（如果是控制LED，通常是熄灭；如果是低电平触发的电路，则是开启）

第一个参数GPIO_TypeDef *GPIOx，是一个指针，GPIOx 是一个占位符，提示要填入端口组的名字。可以填：GPIOA, GPIOB, GPIOC 等。他代表需要输入一个指针变量，当输入GPIOA时就是一个指针GPIOA在内部的定义为
```c
// 1. 这是一个纯粹的数字（基地址）
#define GPIOA_BASE    (APB2PERIPH_BASE + 0x0800)  // 算出结果是 0x40010800
// 2. 这是一个指针（把数字强制转换成了指针）
#define GPIOA         ((GPIO_TypeDef *) GPIOA_BASE)
所以GPIOA实际上就是代表了0x40010800的数据
```

第二个参数uint16_t GPIO_Pin，**uint16_t 只是告诉你这是一个 16 位的整数**（不用深究，知道是数字就行）；GPIO_Pin 提示要填入具体的引脚号，可以填：GPIO_Pin_0, GPIO_Pin_5, GPIO_Pin_13 等（或者是它们的组合 GPIO_Pin_0 | GPIO_Pin_1）

# 28 注意
1.在编写的时候，下边没有提示可以按Ctrl+Alt+空格，会出现提示

2.在编写的时候需要在最后空出一行，防止编译器或某些工具将最后一行代码视为被截断或忽略，避免潜在的编译警告（last line of file ends without a newline）或运行时错误，保证代码的完整性和兼容性GPIO（General Purpose Input Output）

3.C语言不支持二进制，在编写时只能使用16进制编写

4.

# 29 英语单词
## 29.1 缩写
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

## 29.2 全称

Analog：模拟的

Brief：摘要

Buffer：缓冲

Buzzer：蜂鸣器

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
