# 第一部分：C 程序与基础类型

# 1. C 程序的运行过程与 main 函数

## 1.1 STM32 上电后并不是直接进入 main

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

## 1.2 `int main(void)` 的组成

```c
int main(void)
{
    system_init();

    while (1)
    {
        application_process();
    }
}
```

### `main`

函数名。标准 C 程序使用 `main` 作为程序入口函数的名称。

### `(void)`

明确表示该函数不接收参数。

```c
int main (void);   // 明确无参数
```

不要把下面的旧式声明与其混淆：

```c
int main();       // 在旧式 C 声明中，参数信息不完整
```

### `int`

表示函数返回一个整数。在桌面操作系统中，返回值通常用于报告退出状态：

```c
return 0;         // 通常表示正常结束
```

在典型裸机 STM32 程序中，`main()` 一般进入永久循环，正常情况下不会返回。不过仍应使用符合工具链要求的标准写法。


## 1.3 函数名和函数地址

**函数名在多数表达式中会转换为指向该函数的指针**：

```c
void led_on(void)
{

}

void (*callback)(void) = led_on;
```

但更严谨的说法是：

> 函数名在多数表达式中会自动转换为函数指针，而不是把函数名简单理解成一个普通整数地址。

---

# 2. 基本数据类型与固定宽度整数

## 2.1 常见基本类型

|      类型       |   含义   | STM32F407 常见大小 |         标准保证         |
| :-----------: | :----: | :------------: | :------------------: |
|    `char`     | 字符/小整数 |      1 字节      | `sizeof(char)` 永远为 1 |
|    `short`    |  短整型   |      2 字节      |       至少 16 位        |
|     `int`     |  基本整型  |      4 字节      |       至少 16 位        |
|    `long`     |  长整型   |      4 字节      |       至少 32 位        |
|  `long long`  |  长长整型  |      8 字节      |       至少 64 位        |
|    `float`    | 单精度浮点  |      4 字节      |         实现相关         |
|   `double`    | 双精度浮点  |      8 字节      |    精度不低于 `float`     |
| `long double` |  扩展浮点  |      实现相关      |         实现相关         |

需要区分三种层级：

- **标准 C**：只规定类型之间的最小范围和顺序；
- **常见实现**：大多数现代平台的常见大小；
- **STM32F407**：32 位 ARM 工具链下的具体实现。

实际代码中使用：

```c
sizeof(int)
sizeof(void *)
```

确认当前工具链结果，不要只凭电脑经验判断单片机。

## 2.2 `signed` 与 `unsigned`

```c
signed int temperature = -10;
unsigned int count = 100U;
```

无符号类型只能表示非负数，但同样位宽下可以表示更大的正数范围。

嵌入式代码中常见后缀：

```c
100U     // unsigned int
100UL    // unsigned long
```

## 2.3 固定宽度整数类型

包含：

```c
#include <stdint.h>
```

常用类型：

```c
uint8_t
uint16_t
uint32_t
int8_t
int16_t
int32_t
```

例如：

```c
uint16_t adc_value;
uint32_t system_tick;
int16_t temperature;
```

它们适合：

- 寄存器字段；
- 通信协议；
- 固定格式数据；
- 需要明确位宽的硬件接口。

注意拼写是：

```c
uint16_t
```

不是 `Unit_16`、`uint_16` 或 `Uint16`。

## 2.4 `char`、字符常量和字符串

```c
char ch = 'A';
char text[] = "A";
```

二者区别：

|  表达式  |   含义   |      内存内容      |
| :---: | :----: | :------------: |
| `'A'` |  字符常量  |     字符编码值      |
| `"A"` | 字符串字面量 | `'A'` 和结尾 `\0` |

在标准 C 中，普通字符常量 `'A'` 的类型是 `int`，但通常可以存入 `char`：

```c
char ch = 'A';
```

字符串字面量 `"A"` 对应两个字符：

```text
'A'  '\0'
```

## 2.5 `sizeof`

```c
uint32_t value;
size_t size = sizeof(value);
```

`sizeof` 的结果类型是 `size_t`。

数组示例：

```c
int array[5];

sizeof(array);       // 整个数组大小
sizeof(array[0]);    // 单个元素大小
```

数组元素数量：

```c
#define ARRAY_SIZE(array) \
    (sizeof(array) / sizeof((array)[0]))
```

该宏只适用于真正的数组对象，不适用于已经退化成指针的函数参数。



# 3. C 语言关键字概览

关键字由 C 标准规定，不能作为变量名、函数名或结构体成员名。

## 3.1 C90 的 32 个关键字

```text
auto      break     case      char      const     continue
default   do        double    else      enum      extern
float     for       goto      if        int       long
register  return    short     signed    sizeof    static
struct    switch    typedef   union     unsigned  void
volatile  while
```

## 3.2 常见分类

|  类别   |                                             关键字                                             |
| :---: | :-----------------------------------------------------------------------------------------: |
| 基本类型  |           `char`、`short`、`int`、`long`、`signed`、`unsigned`、`float`、`double`、`void`           |
| 流程控制  | `if`、`else`、`switch`、`case`、`default`、`for`、`while`、`do`、`break`、`continue`、`goto`、`return` |
| 自定义类型 |                              `struct`、`union`、`enum`、`typedef`                              |
| 存储与链接 |                             `auto`、`register`、`static`、`extern`                             |
| 类型限定  |                                     `const`、`volatile`                                      |
|  其他   |                                          `sizeof`                                           |

C99/C11 还增加了 `_Bool`、`inline`、`restrict`、`_Atomic` 等关键字。

注意：

```c
bool
true
false
```

通常由：

```c
#include <stdbool.h>
```

提供，不应和原始关键字 `_Bool` 混淆。

---
# 第二部分：变量、函数与流程控制

# 4. 变量的作用域、存储期和链接属性

这三个概念必须分开理解。

## 4.1 作用域：名字在哪里可以使用

### 块作用域

```c
void function(void)
{
    int value = 10;
}
```

`value` 只能在当前 `{}` 内通过名字访问。

### 文件作用域

```c
static int module_state;

void module_run(void)
{
    module_state++;
}
```

定义在所有函数之外的名字，从声明位置开始具有文件作用域。

## 4.2 存储期：对象在内存中存在多久

### 自动存储期

普通局部变量通常具有自动存储期：

```c
void test(void)
{
    int count = 0;
}
```

每次进入函数都会建立新的 `count`，退出函数后对象的生命周期结束。

### 静态存储期

文件作用域变量和局部 `static` 变量具有静态存储期：

```c
static int global_counter;

void test(void)
{
    static int local_counter;
}
```

它们在整个程序运行期间存在。

## 4.3 链接属性：同名声明是否指向同一个实体

### 外部链接

```c
int system_count = 0;
```

其他翻译单元可以通过匹配的 `extern` 声明引用它。

### 内部链接

```c
static int module_count = 0;
```

该名字只在当前 `.c` 文件对应的翻译单元内有效。

### 无链接

普通局部变量通常没有链接属性：

```c
void function(void)
{
    int value;
}
```

## 4.4 局部变量不等于一定在栈上

普通局部变量通常由编译器放在栈或寄存器中，但标准 C 主要规定其语义，不强制具体物理位置。

局部静态变量：

```c
void test(void)
{
    static int count;
}
```

虽然作用域在函数内，但它具有静态存储期，不是每次调用都重新创建的普通栈变量。

---

# 5. 函数、参数和返回值

## 5.1 函数声明、定义与调用

### 声明

告诉编译器函数的名称、参数和返回类型：

```c
void gpio_init(GPIO_TypeDef *gpio, uint16_t pin);
```

### 定义

给出具体实现：

```c
void gpio_init(GPIO_TypeDef *gpio, uint16_t pin)
{
    /* 具体配置过程 */
}
```

### 调用

```c
gpio_init(GPIOA, GPIO_Pin_5);
```

不要把：

```c
void gpio_init(...);
```

和：

```c
gpio_init(...);
```

混淆。前者是声明或定义的一部分，后者是调用。

## 5.2 C 语言默认是值传递

```c
void set_value(int value)
{
    value = 100;
}
```

调用：

```c
int number = 10;
set_value(number);
```

函数得到的是 `number` 值的副本，修改 `value` 不会修改原变量。

## 5.3 使用指针修改调用者对象

```c
void set_value(int *value)
{
    if (value != NULL)
    {
        *value = 100;
    }
}
```

调用：

```c
int number = 10;
set_value(&number);
```

此时函数通过地址修改原对象。

## 5.4 指针参数的主要作用

- 修改调用者的数据；
- 避免复制大型结构体；
- 传入数组或缓冲区；
- 实现多个输出结果；
- 访问 Handle 或硬件寄存器。

大型结构体只读传参建议：

```c
void config_print(const DeviceConfig *config);
```

含义：

- 只传递一个地址；
- 函数不能通过该指针修改配置对象。

## 5.5 返回值与错误状态

简单函数：

```c
int add(int a, int b)
{
    return a + b;
}
```

嵌入式驱动更推荐将状态和输出数据分开：

```c
typedef enum
{
    DRIVER_OK = 0,
    DRIVER_ERROR_PARAMETER,
    DRIVER_ERROR_TIMEOUT
} DriverStatus;

DriverStatus sensor_read(uint16_t *value)
{
    if (value == NULL)
    {
        return DRIVER_ERROR_PARAMETER;
    }

    *value = 1234U;
    return DRIVER_OK;
}
```

这样不会把某个合法数据值误当作错误码。

---

# 6. 流程控制

# 6.1 `if...else`

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

## 6.2 `while`

基本语法：

```c
while (condition)
{
    process();
}
```

### 主循环

```c
while (1)
{
    key_process();
    uart_process();
    sensor_process();
}
```

### 轮询等待

```c
uint32_t timeout = UART_TIMEOUT;

while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
{
    if (timeout == 0U)
    {
        return UART_ERROR_TIMEOUT;
    }

    timeout--;
}
```

不推荐在工程代码中长期使用无限空等待：

```c
while (flag == 0U)
{
}
```

外设异常时可能永久卡死，应设计超时或非阻塞机制。

## 6.3 `for`

适合循环次数明确的场景：

```c
for (uint16_t i = 0U; i < length; i++)
{
    buffer[i] = 0U;
}
```

## 6.4 `switch...case`

适合判断同一个整数、字符或枚举表达式是否等于多个固定值：

```c
switch (command)
{
    case CMD_LED_ON:
        led_on();
        break;

    case CMD_LED_OFF:
        led_off();
        break;

    default:
        command_error();
        break;
}
```

执行过程：

1. 计算 `switch` 表达式；
2. 寻找匹配的 `case`；
3. 从匹配位置开始执行；
4. 遇到 `break` 退出；
5. 没有匹配项时执行 `default`。

`case` 后必须是整型常量表达式：

```c
case 1:
case 'A':
case CMD_START:
```

普通运行时变量不能作为 `case` 值。

### Fall-through

没有 `break` 时会继续执行后面的分支：

```c
switch (grade)
{
    case 'A':
    case 'B':
    case 'C':
        pass();
        break;

    default:
        fail();
        break;
}
```

这里的贯穿是有意用来合并多个相同处理分支。

### `switch` 与 `if` 的选择

| 情况 | 更适合 |
|---|---|
| 同一整数变量匹配固定值 | `switch` |
| 范围判断 | `if...else` |
| 浮点数 | `if...else` |
| 字符串 | `strcmp()` + `if` |
| 枚举状态 | `switch` |

不要简单断言 `switch` 一定更快。编译器可能生成连续比较或跳转表，应优先选择更清晰的表达方式。

## 6.5 `break`、`continue` 和 `return`

- `break`：退出最近的循环或 `switch`；
- `continue`：进入最近循环的下一轮；
- `return`：结束整个函数并可返回一个值。

`switch` 位于循环内时，`break` 通常只退出 `switch`，不会退出外层循环。

---

# 7. 运算符

## 7.1 逻辑运算符

| 运算符 | 含义 |
|---|---|
| `&&` | 逻辑与，全部为真才为真 |
| `||` | 逻辑或，至少一个为真 |
| `!` | 逻辑非 |

```c
if ((temperature > 50) && (voltage < 3.0f))
{
    alarm_on();
}
```

逻辑运算具有短路特性：

```c
if ((ptr != NULL) && (*ptr > 0))
{
}
```

若 `ptr == NULL`，第二个条件不会继续求值，避免解引用空指针。

## 7.2 位运算符

| 运算符 | 含义 | 常见用途 |
|---|---|---|
| `&` | 按位与 | 掩码、检查位、清位 |
| `|` | 按位或 | 置位、组合标志 |
| `^` | 按位异或 | 翻转位 |
| `~` | 按位取反 | 构造清位掩码 |
| `<<` | 左移 | 构造掩码、拼接数据 |
| `>>` | 右移 | 提取字段、拆分数据 |

### 置位

```c
register_value |= (1UL << 5);
```

### 清位

```c
register_value &= ~(1UL << 5);
```

### 检查位

```c
if ((register_value & (1UL << 5)) != 0U)
{
}
```

### 翻转位

```c
register_value ^= (1UL << 5);
```

## 7.3 数据拼接与拆分

两个字节拼成 16 位整数：

```c
uint16_t result =
    ((uint16_t)high_byte << 8) |
    (uint16_t)low_byte;
```

拆分 32 位地址：

```c
uint8_t byte3 = (uint8_t)(address >> 24);
uint8_t byte2 = (uint8_t)(address >> 16);
uint8_t byte1 = (uint8_t)(address >> 8);
uint8_t byte0 = (uint8_t)address;
```

## 7.4 注意有符号移位

对有符号负数进行右移、或让有符号数左移溢出，可能产生实现相关或未定义行为。操作寄存器和位字段时，优先使用明确的无符号类型：

```c
uint32_t mask = 1UL << 31;
```

---
# 第三部分：指针与复合数据类型

# 8. 指针

## 8.1 指针是什么

指针变量用于保存对象或函数的地址：

```c
int value = 100;
int *ptr = &value;
```

可以类比为：

- `value` 是房间中的内容；
- `&value` 是房间地址；
- `ptr` 是记录该地址的纸条；
- `*ptr` 是根据地址进入房间读取或修改内容。

## 8.2 `&` 与 `*`

```c
int value = 10;
int *ptr = &value;

*ptr = 20;
```

执行后：

```c
value == 20
```

符号 `*` 有两种常见语法含义：

```c
int *ptr;     // 声明：ptr 是指向 int 的指针
*ptr = 20;    // 表达式：解引用 ptr
```

不要把它和乘法运算：

```c
a * b
```

混淆。

## 8.3 指针大小由平台决定

```c
sizeof(void *)
```

常见情况：

| 平台 | 普通指针常见大小 |
|---|---:|
| 16 位平台 | 2 字节 |
| STM32F407 等 32 位平台 | 4 字节 |
| 常见 64 位桌面程序 | 8 字节 |

不能笼统地说“指针一定占 8 字节”。

虽然不同类型指针在同一平台上通常大小相同，但它们的含义和步长不同：

```c
char *char_ptr;
int *int_ptr;
double *double_ptr;
```

## 8.4 指针步长

对于：

```c
T *ptr;
```

表达式：

```c
ptr + 1
```

表示移动到下一个 `T` 对象的位置，地址变化通常相当于：

```text
sizeof(T) 个字节
```

例如常见实现中：

```c
char *p1;      // p1 + 1 通常增加 1 字节
uint32_t *p2;  // p2 + 1 通常增加 4 字节
```

指针自身占多少字节，与 `p + 1` 的步长不是一回事。

## 8.5 空指针和非法指针

```c
int *ptr = NULL;
```

调用或解引用前检查：

```c
if (ptr != NULL)
{
    *ptr = 10;
}
```

常见错误：

```c
int *ptr;
*ptr = 10;       // ptr 未初始化，行为未定义
```

指针在使用前必须：

- 指向有效对象；
- 或明确设置为 `NULL`；
- 并确保对象的生命周期尚未结束。

## 8.6 指针作为函数参数

查找数组最大值：

```c
#include <stddef.h>

int find_max(const int *array, size_t count, int *result)
{
    if ((array == NULL) || (result == NULL) || (count == 0U))
    {
        return -1;
    }

    int max_value = array[0];

    for (size_t i = 1U; i < count; i++)
    {
        if (array[i] > max_value)
        {
            max_value = array[i];
        }
    }

    *result = max_value;
    return 0;
}
```

调用：

```c
int values[] = {4, 5, 8, 7, 9, 10, 88, 76};
int max_value;

if (find_max(values, ARRAY_SIZE(values), &max_value) == 0)
{
    printf("Max = %d\n", max_value);
}
```

## 8.7 结构体指针和 `->`

```c
typedef struct
{
    uint32_t baud_rate;
    uint8_t enabled;
} UartConfig;

UartConfig config;
UartConfig *config_ptr = &config;
```

普通结构体变量使用点号：

```c
config.baud_rate = 115200U;
```

结构体指针使用箭头：

```c
config_ptr->baud_rate = 115200U;
```

等价关系：

```c
config_ptr->baud_rate
(*config_ptr).baud_rate
```

`->` 的语法含义只是“通过结构体指针访问成员”，不表示物理数据流方向。

## 8.8 `void *`

`void *` 是通用对象指针，不能直接解引用：

```c
void *context;
```

使用时通常需要转换回原类型：

```c
typedef struct
{
    uint32_t count;
} AppContext;

AppContext *app = (AppContext *)context;
```

函数指针与对象指针是不同类别，不应默认认为函数指针可以安全地存入 `void *`。

## 8.9 二级指针

```c
int **pointer_to_pointer;
```

常见作用：让函数修改调用者的指针变量。

```c
void select_buffer(uint8_t **buffer, uint8_t *new_buffer)
{
    if (buffer != NULL)
    {
        *buffer = new_buffer;
    }
}
```

二级指针还常见于链表、动态内存、字符串数组和对象创建接口。

---

# 9. 数组、字符与字符串

## 9.1 数组是连续的同类型对象集合

```c
int array[5] = {1, 2, 3, 4, 5};
```

数组对象本身不是指针变量。

但数组名在大多数表达式中会转换成指向首元素的指针：

```c
array        // 大多数表达式中转换为 &array[0]
```

特殊情况包括：

```c
sizeof(array)
&array
```

此时不会按普通首元素指针处理。

## 9.2 下标与指针运算

```c
array[i]
```

等价于：

```c
*(array + i)
```

数组下标从 0 开始，是因为第 0 个元素位于相对首地址偏移 0 的位置。

## 9.3 数组和指针的区别

```c
int array[5];
int *ptr = array;
```

假设 `int` 为 4 字节、指针为 4 字节：

```c
sizeof(array);  // 可能为 20
sizeof(ptr);    // 可能为 4
```

数组：

- 拥有整块元素存储空间；
- 数组名不是一个可重新赋值的指针变量。

指针：

- 保存地址；
- 可以改变指向。

## 9.4 数组作为函数参数

```c
void process(int array[10]);
```

在函数参数中会调整为类似：

```c
void process(int *array);
```

所以函数内：

```c
sizeof(array)
```

得到的是指针大小，而不是调用者数组的完整大小。长度应单独传入：

```c
void process(int *array, size_t length);
```

## 9.5 字符串

C 字符串是以 `\0` 结尾的字符序列：

```c
char name[] = "Da Yang";
```

内存中末尾自动包含：

```c
'\0'
```

字符串指针建议只读：

```c
const char *message = "Hello";
```

不要修改字符串字面量：

```c
message[0] = 'h';   // 行为未定义
```

需要可修改字符串时：

```c
char message[] = "Hello";
message[0] = 'h';
```

## 9.6 遍历字符串

```c
void print_string(const char *text)
{
    if (text == NULL)
    {
        return;
    }

    while (*text != '\0')
    {
        putchar(*text);
        text++;
    }
}
```

---

# 10. 结构体、联合体和枚举

# 10.1 结构体 `struct`

结构体把多个可能不同类型的成员组合成一个新类型。

```c
struct Student
{
    char name[20];
    int age;
    float score;
};
```

定义变量：

```c
struct Student student;
```

访问成员：

```c
student.age = 18;
student.score = 95.5f;
```

可以把结构体理解为“数据模板”，结构体变量才是根据模板创建的具体对象。

## 10.2 定义类型时同时定义变量

```c
struct Point
{
    int x;
    int y;
} point1;
```

这里：

- `struct Point` 是结构体类型；
- `point1` 是结构体变量。

继续定义其他变量：

```c
struct Point point2;
```

## 10.3 匿名结构体变量

```c
struct
{
    int x;
    int y;
} point;
```

`point` 是变量，但这个匿名结构体没有可直接复用的标签名。

## 10.4 结构体作为函数参数

按值传递会复制整个结构体：

```c
void config_apply(DeviceConfig config);
```

较大的配置或 Handle 通常传指针：

```c
void config_apply(const DeviceConfig *config);
```

## 10.5 结构体对齐

结构体大小通常不等于所有成员大小的简单相加：

```c
typedef struct
{
    uint8_t a;
    uint32_t b;
    uint8_t c;
} Data;
```

编译器可能在成员之间加入填充字节，以满足对齐要求：

```c
sizeof(Data)
```

可能大于 6。

这一内容应在后续“内存管理与数据布局”章节中进一步展开。

# 10.6 联合体 `union`

联合体的成员共享同一块存储空间：

```c
typedef union
{
    uint32_t value;
    uint8_t bytes[4];
} DataConverter;
```

```c
DataConverter converter;
converter.value = 0x12345678U;
```

成员共享内存，但使用联合体查看字节序时要考虑：

- 大小端；
- 类型别名规则；
- 编译器实现。

通信协议中更稳妥的方式通常是移位和掩码。

# 10.7 枚举 `enum`

枚举为一组相关整数值提供名字：

```c
typedef enum
{
    STATE_IDLE = 0,
    STATE_RUNNING,
    STATE_ERROR
} SystemState;
```

定义变量：

```c
SystemState state = STATE_IDLE;
```

默认情况下：

```text
STATE_IDLE    = 0
STATE_RUNNING = 1
STATE_ERROR   = 2
```

也可以手动指定协议值：

```c
typedef enum
{
    FLASH_CMD_WRITE_ENABLE = 0x06,
    FLASH_CMD_READ_STATUS  = 0x05,
    FLASH_CMD_JEDEC_ID     = 0x9F
} FlashCommand;
```

注意：C 的枚举不是严格封闭集合。即使变量类型是枚举，语言本身通常不能完全阻止它保存列表外的整数值。因此状态机仍建议设置 `default`。

---

# 11. typedef 类型别名

## 11.1 基本形式

```c
typedef 原类型 新类型名;
```

例如：

```c
typedef unsigned int UInt;
```

使用：

```c
UInt count = 10U;
```

`typedef` 通常不会创造一个本质上全新的类型，而是为已有类型增加别名。

## 11.2 结构体类型别名

不使用 `typedef`：

```c
struct Student
{
    int age;
};

struct Student student;
```

使用 `typedef`：

```c
typedef struct
{
    int age;
} Student;

Student student;
```

保留标签的写法：

```c
typedef struct Student
{
    int age;
} Student;
```

此时二者都存在：

```c
struct Student student1;
Student student2;
```

## 11.3 自引用结构体

推荐先前向声明：

```c
typedef struct Node Node;

struct Node
{
    int data;
    Node *next;
};
```

常用于链表。

## 11.4 枚举类型别名

```c
typedef enum
{
    DRIVER_OK = 0,
    DRIVER_ERROR
} DriverStatus;
```

## 11.5 指针类型别名

```c
typedef int *IntPtr;
```

```c
IntPtr p1;
IntPtr p2;
```

两者都是 `int *`。

但指针别名可能隐藏 `const` 的真实位置：

```c
typedef int *IntPtr;
const IntPtr ptr = NULL;
```

等价于：

```c
int * const ptr = NULL;
```

而不是：

```c
const int *ptr;
```

因此公开接口中不要过度使用隐藏星号的指针别名。

## 11.6 数组类型别名

```c
typedef uint8_t PacketBuffer[128];

PacketBuffer rx_buffer;
PacketBuffer tx_buffer;
```

## 11.7 函数指针类型别名

原始写法：

```c
int (*operation)(int, int);
```

使用 `typedef`：

```c
typedef int (*Operation)(int, int);

Operation operation;
```

## 11.8 `typedef` 和 `#define`

```c
typedef int *IntPtr;
IntPtr a, b;           // a、b 都是 int *
```

宏只是文本替换：

```c
#define INT_PTR int *
INT_PTR a, b;          // 展开后为 int *a, b;
```

此时只有 `a` 是指针，`b` 是 `int`。

结论：

- 类型别名使用 `typedef`；
- 常量、条件编译和宏表达式使用 `#define`。

---
# 第四部分：模块化与嵌入式关键字

# 12. 预处理器、头文件和源文件

## 12.1 预处理器

以 `#` 开头的指令通常由预处理器在正式编译前处理。

常见指令：

| 指令 | 用途 |
|---|---|
| `#include` | 包含头文件 |
| `#define` | 定义宏 |
| `#if`、`#ifdef`、`#ifndef` | 条件编译 |
| `#endif` | 结束条件编译 |
| `#pragma` | 编译器相关控制 |

## 12.2 `#include`

```c
#include <stdint.h>
#include "uart.h"
```

常见约定：

- `<...>`：系统库或工具链头文件；
- `"..."`：项目自己的头文件。

可以把它近似理解为预处理阶段的文本包含，但实际查找路径和处理方式由工具链规则决定。

## 12.3 `#define`

对象式宏：

```c
#define BUFFER_SIZE 128U
#define GPIO_PIN_4  ((uint16_t)0x0010U)
```

末尾通常不加分号：

```c
#define BUFFER_SIZE 128U   // 正确
```

宏函数要注意括号：

```c
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
```

复杂逻辑优先考虑 `static inline` 函数，避免宏的重复求值和类型问题。

## 12.4 头文件保护

推荐：

```c
#ifndef PROJECT_UART_H
#define PROJECT_UART_H

#include <stdint.h>

void uart_init(void);

#endif
```

头文件保护解决的是：

> 同一个头文件在同一个翻译单元中通过直接或间接路径被重复包含。

每个 `.c` 文件会独立进行预处理。一个 `.c` 文件中定义的头文件保护宏不会自动传播到另一个 `.c` 文件。

不建议使用双下划线开头的名称：

```c
#ifndef __UART_H__
```

这类名称可能属于实现保留标识符。项目中使用：

```c
#ifndef PROJECT_UART_H
```

更稳妥。

## 12.5 `.h` 和 `.c` 的职责

### 头文件 `.h`

对外说明模块可以提供什么：

- 公共类型；
- 宏和枚举；
- 函数声明；
- 必要的 `extern` 声明。

```c
#ifndef PROJECT_BUZZER_H
#define PROJECT_BUZZER_H

void buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);

#endif
```

### 源文件 `.c`

保存具体实现和模块私有内容：

```c
#include "buzzer.h"

static void buzzer_gpio_init(void)
{
}

void buzzer_init(void)
{
    buzzer_gpio_init();
}
```

### 应用层

```c
#include "buzzer.h"

int main(void)
{
    buzzer_init();

    while (1)
    {
    }
}
```

## 12.6 推荐的模块结构

```text
uart.h
├── 公共类型
├── 公共宏
├── 函数声明
└── 必要的 extern 声明

uart.c
├── Handle 或对象定义
├── static 私有变量
├── static 私有函数
└── 公共接口实现
```

---

# 13. static 与 extern

# 13.1 局部 `static`

```c
void test(void)
{
    static int count = 0;

    count++;
    printf("%d\n", count);
}
```

多次调用输出：

```text
1
2
3
```

特性：

- 只初始化一次；
- 作用域仍在当前函数块；
- 生命周期持续整个程序运行期间；
- 不适合未经保护地被多个任务、递归调用或中断共享。

## 13.2 文件作用域 `static` 变量

```c
/* uart.c */
static uint8_t rx_buffer[128];
```

含义：

- 变量具有内部链接；
- 只向当前翻译单元提供该名字；
- 其他 `.c` 文件不能通过 `extern` 匹配到它。

## 13.3 `static` 函数

```c
static void uart_gpio_init(void)
{
}

void uart_init(void)
{
    uart_gpio_init();
}
```

`uart_gpio_init()` 是模块私有函数；`uart_init()` 是对外公共接口。

## 13.4 `extern`

定义只出现一次：

```c
/* system.c */
int system_count = 0;
```

在头文件中声明：

```c
/* system.h */
#ifndef PROJECT_SYSTEM_H
#define PROJECT_SYSTEM_H

extern int system_count;

#endif
```

其他文件通过头文件使用：

```c
#include "system.h"

void function(void)
{
    system_count++;
}
```

`extern` 的含义不是“强行访问任何文件的变量”，而是声明：

> 具有匹配外部链接定义的对象在其他位置存在。

如果真正定义是：

```c
static int system_count;
```

其他文件的 `extern int system_count;` 无法匹配它。

## 13.5 不要在头文件中定义普通 `static` 状态变量

```c
/* 不推荐 */
static int count = 0;
```

每个包含该头文件的 `.c` 文件都可能得到一份独立对象，通常不是想要的共享效果。

头文件中常见的 `static inline` 函数是另一种用途：

```c
static inline uint32_t bit_mask(uint32_t bit)
{
    return 1UL << bit;
}
```

---

# 14. const 与 volatile

# 14.1 `const`

`const` 表示不能通过当前表达式或访问路径修改对象。

```c
const int value = 10;
```

```c
value = 20;   // 不允许
```

但 `const` 不必然意味着物理值永远不会变化。例如只读硬件状态寄存器可能由硬件更新。

## 14.2 `const` 与指针

### 指向只读数据的指针

```c
const int *ptr;
```

- `ptr` 可以改变指向；
- 不能通过 `ptr` 修改数据。

### 常量指针

```c
int * const ptr = &value;
```

- 指针不能改变地址；
- 可以修改指向的数据。

### 指向只读数据的常量指针

```c
const int * const ptr = &value;
```

两者都不能通过该声明改变。

| 声明 | 指针能否改地址 | 能否通过指针改数据 |
|---|---:|---:|
| `int *ptr` | 可以 | 可以 |
| `const int *ptr` | 可以 | 不可以 |
| `int * const ptr` | 不可以 | 可以 |
| `const int * const ptr` | 不可以 | 不可以 |

函数只读输入：

```c
void uart_send(const uint8_t *data, uint16_t length);
```

## 14.3 `volatile`

`volatile` 表示对象可能被当前代码流之外的因素改变，因此编译器必须按易失访问规则处理相关读写。

典型来源：

- 外设硬件；
- 中断服务程序；
- DMA 或其他总线主设备；
- 另一个执行上下文。

### 硬件寄存器

```c
#define USART_STATUS_REG \
    (*(volatile uint32_t *)0x40011000UL)
```

```c
while ((USART_STATUS_REG & USART_TXE_MASK) == 0U)
{
}
```

### 主程序和中断共享标志

```c
static volatile uint8_t rx_complete = 0U;

void USART1_IRQHandler(void)
{
    rx_complete = 1U;
}

int main(void)
{
    while (rx_complete == 0U)
    {
    }

    process_data();
}
```

## 14.4 `volatile` 不能解决什么

`volatile` 不等于：

- 原子操作；
- 线程安全；
- 互斥锁；
- 内存屏障；
- Cache 一致性；
- 防止逻辑竞争。

例如：

```c
volatile uint32_t counter;
counter++;
```

`counter++` 可能包含读取、加一、写回多个步骤，不能自动保证并发原子性。

在多任务或复杂中断环境中，仍可能需要：

- 临界区；
- 原子操作；
- 互斥量；
- 信号量；
- 队列；
- 内存屏障；
- Cache clean/invalidate。

## 14.5 常见组合

```c
static volatile uint8_t flag;
```

含义：

- 文件私有；
- 整个程序期间存在；
- 可能被异步修改。

```c
static const uint16_t lookup_table[] = {1U, 2U, 3U};
```

含义：文件私有的只读查找表。

```c
volatile const uint32_t status_register;
```

软件只读，但值可能由外部硬件改变。

---

# 15. 函数指针与回调函数

# 15.1 函数指针

普通数据指针：

```c
int value = 10;
int *ptr = &value;
```

函数指针：

```c
int add(int a, int b)
{
    return a + b;
}

int (*operation)(int, int) = add;
```

调用：

```c
int result = operation(10, 20);
```

也可以写：

```c
int result = (*operation)(10, 20);
```

## 15.2 为什么括号不能省略

函数指针：

```c
int (*operation)(int, int);
```

返回指针的函数：

```c
int *operation(int, int);
```

`()` 优先级高于 `*`，因此函数指针变量名周围必须加括号。

## 15.3 函数类型必须匹配

函数指针的：

- 返回类型；
- 参数数量；
- 参数类型；

必须与目标函数兼容。

不要通过强制转换隐藏类型不匹配：

```c
callback = (Callback)wrong_function;   // 不推荐，可能导致未定义行为
```

## 15.4 使用 `typedef`

```c
typedef int (*Operation)(int, int);

Operation operation = add;
```

函数指针数组：

```c
static const Operation operation_table[] =
{
    add,
    subtract,
    multiply
};
```

## 15.5 回调函数

回调机制：

```text
应用层定义函数
→ 把函数地址注册给模块
→ 模块保存函数地址
→ 事件发生
→ 模块通过函数指针调用应用层函数
```

定义类型：

```c
typedef void (*ButtonCallback)(void);
```

保存回调：

```c
static ButtonCallback button_callback = NULL;
```

注册：

```c
void button_register_callback(ButtonCallback callback)
{
    button_callback = callback;
}
```

事件发生时调用：

```c
void button_process(void)
{
    if (button_is_pressed())
    {
        ButtonCallback callback = button_callback;

        if (callback != NULL)
        {
            callback();
        }
    }
}
```

应用层：

```c
static void app_button_pressed(void)
{
    led_toggle();
}

int main(void)
{
    button_register_callback(app_button_pressed);

    while (1)
    {
        button_process();
    }
}
```

## 15.6 带上下文的回调

```c
typedef void (*DataCallback)(
    void *context,
    const uint8_t *data,
    uint16_t length);
```

Handle 中保存：

```c
typedef struct
{
    DataCallback callback;
    void *callback_context;
} DataReceiver;
```

调用：

```c
if (receiver->callback != NULL)
{
    receiver->callback(
        receiver->callback_context,
        data,
        length);
}
```

这种设计让同一个回调函数可以操作不同应用对象。

## 15.7 中断回调注意事项

若回调由中断函数调用，它仍处于中断上下文中。回调应尽量短：

推荐：

- 保存少量数据；
- 设置标志；
- 发送队列或任务通知；
- 使用 RTOS 的 `FromISR` 接口。

不推荐：

- 长时间延时；
- 大量打印；
- 大循环计算；
- 等待普通互斥锁；
- 在中断中处理完整业务协议。

---

# 16. Handle 句柄

## 16.1 Handle 不是关键字

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

## 16.2 Handle 中通常保存什么

- 硬件实例地址；
- 配置参数；
- 运行状态；
- 发送和接收缓冲区；
- 当前计数；
- 错误码；
- DMA Handle；
- 回调函数；
- 用户上下文。

## 16.3 为什么传 Handle 指针

```c
DriverStatus uart_init(UartHandle *handle);
```

原因：

- 避免复制整个结构体；
- 函数可以修改原 Handle 的状态；
- 一套驱动函数支持多个设备实例；
- 中断、DMA 和回调可以共享设备上下文。

## 16.4 Handle 的生命周期

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

## 16.5 模块私有和公共 Handle

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

---

# 17. 状态机

## 17.1 状态机是什么

状态机用于描述：

> 系统当前处于什么状态、当前状态执行什么动作，以及发生什么事件时切换到哪个状态。

四个核心元素：

- 状态；
- 事件；
- 条件；
- 动作。

## 17.2 `enum + switch` 状态机

```c
typedef enum
{
    SYSTEM_STATE_INIT = 0,
    SYSTEM_STATE_IDLE,
    SYSTEM_STATE_RUNNING,
    SYSTEM_STATE_ERROR
} SystemState;

static SystemState current_state = SYSTEM_STATE_INIT;
```

```c
void system_state_machine(void)
{
    switch (current_state)
    {
        case SYSTEM_STATE_INIT:
        {
            system_init();
            current_state = SYSTEM_STATE_IDLE;
            break;
        }

        case SYSTEM_STATE_IDLE:
        {
            if (start_requested())
            {
                current_state = SYSTEM_STATE_RUNNING;
            }

            break;
        }

        case SYSTEM_STATE_RUNNING:
        {
            system_run();

            if (error_detected())
            {
                current_state = SYSTEM_STATE_ERROR;
            }

            break;
        }

        case SYSTEM_STATE_ERROR:
        {
            system_stop();
            error_process();
            break;
        }

        default:
        {
            current_state = SYSTEM_STATE_ERROR;
            break;
        }
    }
}
```

主循环：

```c
while (1)
{
    system_state_machine();
    communication_process();
    sensor_process();
}
```

## 17.3 状态和事件的区别

```text
状态：系统现在处于什么阶段
事件：发生了什么事情
```

例如：

```text
当前状态：IDLE
发生事件：START
新状态：RUNNING
```

## 17.4 非阻塞状态机

阻塞式：

```c
motor_start();
delay_ms(1000);
motor_run();
```

非阻塞式：

```c
case MOTOR_STATE_STARTING:
{
    if ((system_tick - start_tick) >= 1000U)
    {
        current_state = MOTOR_STATE_RUNNING;
    }

    break;
}
```

非阻塞设计可以让主循环继续处理其他模块。

## 17.5 状态机常见用途

- 电机控制；
- 按键消抖；
- UART 协议解析；
- 网络连接；
- 蓝牙连接；
- 设备初始化；
- 错误恢复；
- 菜单系统；
- 固件升级流程。

## 17.6 状态机不等于 RTOS 任务

状态机是逻辑组织方式，RTOS 任务是调度执行单元。

```c
void motor_task(void *parameter)
{
    while (1)
    {
        motor_state_machine();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

这里 `motor_task` 是任务，`motor_state_machine` 是任务内部的状态机。

---
# 第五部分：STM32 中的 C 语言应用

# 18. 内存映射寄存器与结构体指针

## 18.1 外设寄存器位于固定地址

STM32 把外设寄存器映射到统一地址空间中。某个地址不只是普通数字，而是对应实际硬件寄存器。

例如概念性定义：

```c
#define GPIOA_BASE 0x40020000UL
```

编译器只看到整数地址时，并不知道该区域的寄存器布局。

## 18.2 使用寄存器结构体描述布局

```c
typedef struct
{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
} GpioRegisters;
```

将固定地址转换成结构体指针：

```c
#define GPIOA ((GpioRegisters *)GPIOA_BASE)
```

现在可以写：

```c
GPIOA->ODR |= (1UL << 5);
```

编译器根据：

- `GPIOA_BASE`；
- `GpioRegisters` 成员偏移；
- `ODR` 的类型；

计算实际寄存器地址并执行访问。

## 18.3 `(GPIO_TypeDef *)` 强制类型转换

```c
(GPIO_TypeDef *)GPIOA_BASE
```

含义：

> 把整数形式的地址解释为“指向 GPIO 寄存器结构体的指针”。

这不是创建新硬件，也不是复制结构体，只是告诉编译器如何解释该地址。

## 18.4 `ADC1->DR`

```c
uint32_t value = ADC1->DR;
```

代码层面：

```c
ADC1->DR
```

等价于：

```c
(*ADC1).DR
```

硬件层面：ADC 完成转换后，硬件会更新数据寄存器 `DR`，CPU 再通过内存映射地址读取它。

注意：`->` 在 C 语法中不代表“ADC 把数据传送到 DR”，它只表示结构体指针成员访问。

## 18.5 为什么寄存器成员使用 `volatile`

硬件可能独立改变寄存器值：

```c
volatile uint32_t status;
```

若没有适当的易失限定，编译器可能根据普通内存对象的假设优化掉重复读取。

CMSIS 常见宏的语义大致是：

```c
__I   // 软件只读，硬件可能改变
__O   // 软件只写
__IO  // 软件可读写，硬件也可能改变
```

具体定义应查看当前 CMSIS 版本和工具链头文件。

## 18.6 读改写风险

```c
GPIOA->ODR |= GPIO_PIN_5;
```

通常是：

```text
读取 ODR
→ 修改某一位
→ 写回 ODR
```

当寄存器同时可能被中断或其他执行上下文修改时，可能发生竞争。部分 STM32 外设提供原子置位/复位寄存器，如 GPIO 的 BSRR，应优先使用适合的硬件机制。

# 19. `printf` 与调试输出

## 19.1 基本格式

```c
printf("格式控制字符串", 参数列表);
```

常见格式说明符：

| 格式 | 含义 |
|---|---|
| `%d`、`%i` | 有符号十进制整数 |
| `%u` | 无符号十进制整数 |
| `%x`、`%X` | 无符号十六进制整数 |
| `%o` | 无符号八进制整数 |
| `%c` | 字符 |
| `%s` | 字符串 |
| `%f` | 浮点数 |
| `%p` | 指针地址 |
| `%%` | 百分号本身 |

示例：

```c
#include <stdio.h>

int main(void)
{
    int number = 123;
    double pi = 3.1415926;
    const char *text = "Hello";

    printf("整数：%d\n", number);
    printf("两位小数：%.2f\n", pi);
    printf("宽度 5，右对齐：%5d\n", number);
    printf("宽度 5，左对齐：%-5d\n", number);
    printf("字符串：%s\n", text);
    printf("字符串前三个字符：%.3s\n", text);

    return 0;
}
```

## 19.2 打印指针

正确：

```c
printf("%p\n", (void *)ptr);
```

不要使用：

```c
printf("%d\n", ptr);
printf("%x\n", ptr);
```

因为指针和 `int`/`unsigned int` 的大小与表示不一定兼容。

## 19.3 固定宽度整数输出

为了可移植地输出 `uint32_t` 等类型，可以使用：

```c
#include <inttypes.h>

uint32_t value = 100U;
printf("%" PRIu32 "\n", value);
```

很多 STM32 工程中会根据工具链实际类型使用强制转换：

```c
printf("%lu\n", (unsigned long)value);
```

但需要明确当前工具链中目标类型与格式的匹配关系。

## 19.4 STM32 中重定向 `printf`

常见做法是实现工具链要求的输出钩子，例如：

```c
int fputc(int ch, FILE *stream)
{
    (void)stream;
    uart_send_byte(&uart1, (uint8_t)ch);
    return ch;
}
```

不同工具链可能要求：

- `fputc()`；
- `_write()`；
- 特定的半主机配置。

应根据 Keil ARMCC、Armclang 或 GCC 的运行库要求实现。

## 19.5 中断中谨慎使用 `printf`

`printf` 可能：

- 执行时间长；
- 使用较多栈空间；
- 不可重入；
- 等待串口发送；
- 影响实时性。

中断中通常只记录简短事件或设置标志，把完整日志放到主循环或日志任务中输出。

---

# 20. 常见错误与编码规范

## 20.1 使用英文半角符号

错误：

```c
printf(“value=%d\n”， value)；
```

正确：

```c
printf("value=%d\n", value);
```

C 代码中要使用英文半角：

```text
() {} [] ; , ' "
```

## 20.2 大小写敏感

```c
z.c = 1.2f;
Z.c = 1.2f;
```

`z` 和 `Z` 是不同标识符。

## 20.3 单词和类型拼写

常见正确形式：

```text
bit          不是 bite
uint16_t     不是 Unit_16
signed       不是 sighed
_Atomic      不是 automic
struct       不是 Struct
```

## 20.4 初始化指针和结构体

```c
DeviceHandle handle = {0};
uint8_t *buffer = NULL;
```

不要直接使用未初始化指针或包含不确定成员的 Handle。

## 20.5 检查缓冲区边界

```c
if (rx_length < (RX_BUFFER_SIZE - 1U))
{
    rx_buffer[rx_length] = data;
    rx_length++;
}
```

需要作为字符串使用时，为 `\0` 保留位置。

## 20.6 轮询必须考虑超时

```c
while (hardware_not_ready())
{
    if (timeout == 0U)
    {
        return DRIVER_ERROR_TIMEOUT;
    }

    timeout--;
}
```

更高层项目还应考虑基于系统 Tick 的真实时间超时，而不是只依赖循环次数。

## 20.7 不把合法数据值兼作错误码

不推荐：

```c
uint8_t spi_read_byte(void);   // 失败也返回 0xFF
```

推荐：

```c
DriverStatus spi_read_byte(uint8_t *value);
```

## 20.8 注释解释原因，不只翻译代码

较弱注释：

```c
value++;   // value 加一
```

较好注释：

```c
value++;   // 记录已经接收的有效字节数
```

## 20.9 统一命名风格

类型：

```c
UartHandle
DriverStatus
SystemState
```

函数和变量：

```c
uart_init
uart_send_byte
rx_length
```

宏：

```c
UART_RX_BUFFER_SIZE
DRIVER_TIMEOUT_MS
```

建议不要使用含义不明确的类型名：

```c
typedef_spi
```

更清晰：

```c
SpiHandle
SpiConfig
```

## 20.10 文件末尾保留换行

源文件末尾保留一个换行符，符合文本文件和多数工具链习惯，避免相关警告。

## 20.11 二进制字面量

传统 C90/C99/C11/C17 标准没有统一规定 `0b1010` 语法，但部分编译器长期作为扩展支持；C23 已正式加入二进制整数字面量。

旧工具链或追求兼容性时，嵌入式代码通常使用：

```c
0x0AU
```

或者通过移位构造：

```c
1UL << 5
```

---

# 附录

# A. 常见缩写

| 缩写   | 全称                                   | 含义         |
| ---- | ------------------------------------ | ---------- |
| ACK  | Acknowledge                          | 应答         |
| AF   | Alternate Function                   | 复用功能       |
| AHB  | Advanced High-performance Bus        | 高性能总线      |
| APB  | Advanced Peripheral Bus              | 外设总线       |
| ARR  | Auto-Reload Register                 | 自动重装载寄存器   |
| CCR  | Capture/Compare Register             | 捕获/比较寄存器   |
| DMA  | Direct Memory Access                 | 直接存储器访问    |
| DR   | Data Register                        | 数据寄存器      |
| EOC  | End of Conversion                    | 转换结束       |
| EXTI | External Interrupt/Event Controller  | 外部中断/事件控制器 |
| GPIO | General-Purpose Input/Output         | 通用输入输出     |
| IRQ  | Interrupt Request                    | 中断请求       |
| ISR  | Interrupt Service Routine            | 中断服务程序     |
| NVIC | Nested Vectored Interrupt Controller | 嵌套向量中断控制器  |
| PSC  | Prescaler                            | 预分频器       |
| RCC  | Reset and Clock Control              | 复位和时钟控制    |
| RX   | Receive                              | 接收         |
| TX   | Transmit                             | 发送         |
| SCL  | Serial Clock Line                    | 串行时钟线      |
| SDA  | Serial Data Line                     | 串行数据线      |
| TRGO | Trigger Output                       | 触发输出       |

# B. 常见英文词汇

| 英文            | 含义       |
| ------------- | -------- |
| buffer        | 缓冲区      |
| callback      | 回调       |
| configuration | 配置       |
| context       | 上下文      |
| handle        | 句柄、上下文对象 |
| instance      | 实例       |
| interrupt     | 中断       |
| parameter     | 形参、参数    |
| argument      | 实参       |
| peripheral    | 外设       |
| pointer       | 指针       |
| prescaler     | 预分频器     |
| register      | 寄存器      |
| return value  | 返回值      |
| sensor        | 传感器      |
| timeout       | 超时       |
| volatile      | 易失的      |


# D. 学习检查表

完成本册后，应能独立解释：

- `main()` 前发生了什么；
- 作用域、存储期和链接属性的区别；
- `static` 在不同位置的含义；
- `extern` 为什么无法访问另一个文件的 `static` 对象；
- 数组与指针的区别；
- `const int *` 和 `int * const` 的区别；
- `volatile` 能解决什么、不能解决什么；
- `typedef` 与 `#define` 的区别；
- 函数指针和回调函数如何工作；
- Handle 为什么通常传指针；
- 状态机如何实现非阻塞流程；
- `GPIOA->ODR` 的地址、结构体和寄存器含义；
- 为什么轮询函数需要超时；
- 为什么中断中不应执行大量业务逻辑。


