
**C 语言基本功 → 指针与内存 → 字符串 → 位操作 → 链表/队列 → 排序查找 → 环形缓冲区 → 协议解析 → 状态机 → 寄存器操作 → ISR/并发 → 工程代码。**

尤其是校招，面试官往往更在意：**你能不能写出安全、边界正确、能在 MCU 上跑的 C 代码。**

下面这套我建议你直接当成自己的「嵌入式手撕代码题库」。

---

# 一、先明确：嵌入式手撕和 `LeetCode` 到底有什么区别

``LeetCode`` 经常是：

```text
数组
↓
寻找巧妙算法
↓
时间复杂度 O(n)
↓
哈希表 / 双指针 / DP / DFS
```

嵌入式面试更可能是：

```text
uint8_t buf[128];

请实现：
1. 字符串函数
2. memcpy
3. 位操作
4. 大小端转换
5. 环形缓冲区
6. 串口数据解析
7. 状态机
8. CRC
9. 链表
10. callback
11. 寄存器操作
12. ISR安全代码
```

甚至面试官可能直接问：

```c
#define SET_BIT(REG, BIT) ?
```

或者：

```c
void UART_IRQHandler(void)
{
    ...
}
```

让你现场补。

所以你刷题应该变成：

> **C语言手撕 60% + 数据结构算法 20% + 嵌入式场景题 20%。**

---

# 二、第一档：C语言基本功 ⭐⭐⭐⭐⭐

这些属于**不会就很危险**的题。

## 1. 手写 `strlen`

```c
size_t my_strlen(const char *str);
```

要求：

```c
my_strlen("hello") == 5
```

考：

- 指针
    
- `\0`
    
- NULL
    
- size_t
    

---

## 2. 手写 `strcpy`

```c
char *my_strcpy(char *dst, const char *src);
```

重点：

```c
while ((*dst++ = *src++) != '\0');
```

但面试时你必须能解释：

> 为什么这样写有溢出风险？

---

## 3. 手写 `strncpy` ([ X])

```c
char *my_strncpy(char *dst, const char *src, size_t n);
```

考察边界。

---

## 4. 手写 `strcmp`

```c
int my_strcmp(const char *s1, const char *s2);
```

返回：

```text
<0
=0
>0
```

---

## 5. 手写 `strcat`

```c
char *my_strcat(char *dst, const char *src);
```

---

## 6. 手写 `strstr`

找子字符串：

```c
char *my_strstr(const char *str, const char *substr);
```

例如：

```text
"abcdef"
   "cde"
```

返回 `c` 地址。

⭐⭐⭐⭐

---

# 三、内存函数 ⭐⭐⭐⭐⭐

嵌入式特别爱问。

## 7. 手写 `memcpy`

```c
void *my_memcpy(void *dst, const void *src, size_t n);
```

最基础版本：

```c
void *my_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    while (n--) {
        *d++ = *s++;
    }

    return dst;
}
```

你必须知道：

> memcpy **不能安全处理内存重叠**。

---

# 四、手写 `memmove` ⭐⭐⭐⭐⭐

这是非常经典的嵌入式面试题。

```c
void *my_memmove(void *dst, const void *src, size_t n);
```

关键：

```text
dst < src
从前往后复制

dst > src
从后往前复制
```

例如：

```text
abcdef
 ↓↓

memmove(buf + 2, buf, 4)
```

必须避免源数据被覆盖。

---

# 五、手写 `memset` ⭐⭐⭐⭐

```c
void *my_memset(void *ptr, int value, size_t n);
```

---

# 六、数组类 ⭐⭐⭐⭐⭐

## 10. 数组最大值

```c
int find_max(const int *arr, int n);
```

---

## 11. 数组最小值

```c
int find_min(const int *arr, int n);
```

---

## 12. 数组求和

```c
int sum_array(const int *arr, int n);
```

还可能问：

> 如果可能溢出怎么办？

---

## 13. 数组平均值

```c
double average(const int *arr, int n);
```

考：

```c
sum / n
```

整数除法问题。

---

## 14. 数组逆序 ⭐⭐⭐⭐⭐

```c
void reverse_array(int *arr, int n);
```

要求：

```text
1 2 3 4 5
↓
5 4 3 2 1
```

必须做到：

```text
O(n)
O(1)空间
```

---

## 15. 删除数组中的指定元素

例如：

```text
1 2 3 2 4

删除2

1 3 4
```

函数：

```c
int remove_value(int *arr, int n, int value);
```

返回新长度。

---

## 16. 数组去重

有序数组：

```text
1 1 2 2 3 3

→

1 2 3
```

⭐⭐⭐⭐

---

## 17. 两个有序数组合并

```text
1 3 5
2 4 6

→

1 2 3 4 5 6
```

---

## 18. 找数组第二大元素 ⭐⭐⭐⭐

注意：

```text
5 5 4
```

第二大到底是：

```text
5
```

还是：

```text
4
```

面试前要先确认定义。

---

## 19. 找唯一出现一次的数 ⭐⭐⭐⭐⭐

```text
1 2 3 2 1

→ 3
```

要求不用额外数组。

核心：

```c
result ^= arr[i];
```

这个非常符合嵌入式。

---

## 20. 找缺失数字

```text
0 1 2 4 5

→ 3
```

可以：

```text
求和
XOR
```

---

# 七、字符串类 ⭐⭐⭐⭐⭐

## 21. 字符串逆序

```c
void reverse_string(char *str);
```

例如：

```text
hello
↓
olleh
```

---

## 22. 判断回文字符串

```text
abcba
```

返回 true。

---

## 23. 字符串转整数 atoi ⭐⭐⭐⭐⭐

自己实现：

```c
int my_atoi(const char *str);
```

需要考虑：

```text
"123"
"-123"
"+123"
"00123"
```

高级一点还要考虑：

```text
溢出
非法字符
空格
```

---

## 24. 整数转字符串 `itoa` ⭐⭐⭐⭐⭐

```c
void my_itoa(int num, char *str);
```

例如：

```text
123
↓
"123"
```

还需要处理：

```text
-123
0
INT_MIN
```

---

## 25. 十六进制字符串转整数

```text
"1A3F"
↓
0x1A3F
```

例如：

```c
uint32_t hex_to_int(const char *str);
```

这个在嵌入式比普通算法题实用得多。

---

# 八、位运算 ⭐⭐⭐⭐⭐⭐⭐

这是嵌入式最重要的手撕模块之一。

## 26. 设置某一位

把 bit5 设置为 1：

```c
value |= (1U << 5);
```

---

## 27. 清除某一位

```c
value &= ~(1U << 5);
```

---

## 28. 翻转某一位

```c
value ^= (1U << 5);
```

---

## 29. 判断某一位是否为1

```c
if (value & (1U << 5))
```

---

## 30. 写宏 ⭐⭐⭐⭐⭐

```c
#define SET_BIT(x, n)      ((x) |=  (1U << (n)))
#define CLEAR_BIT(x, n)    ((x) &= ~(1U << (n)))
#define TOGGLE_BIT(x, n)   ((x) ^=  (1U << (n)))
#define GET_BIT(x, n)      (((x) >> (n)) & 1U)
```

---

# 九、取某一段 bit ⭐⭐⭐⭐⭐

例如：

```text
uint32_t data

取 bit[7:4]
```

要求写：

```c
(data >> 4) & 0x0F
```

非常高频。

---

# 十、修改寄存器中的某几个 bit ⭐⭐⭐⭐⭐

例如：

```text
REG bit[7:4] 设置成 0b1010
但不能影响其他位
```

核心：

```c
REG &= ~(0xFU << 4);
REG |=  (0xAU << 4);
```

这是必须掌握的。

---

# 十一、统计二进制中1的个数 ⭐⭐⭐⭐⭐

例如：

```text
0b10110100
```

一共有几个1？

基础：

```c
while (x) {
    count += x & 1U;
    x >>= 1;
}
```

优秀版本：

```c
while (x) {
    x &= x - 1;
    count++;
}
```

一定要理解：

```c
x & (x - 1)
```

会消掉最低位的 `1`。

---

# 十二、判断2的幂 ⭐⭐⭐⭐

```c
bool is_power_of_two(uint32_t x)
{
    return x != 0 && (x & (x - 1)) == 0;
}
```

---

# 十三、交换高低四位

```text
0xAB
↓
0xBA
```

---

# 十四、交换32位整数大小端 ⭐⭐⭐⭐⭐

```c
uint32_t swap32(uint32_t x);
```

目标：

```text
0x12345678
↓
0x78563412
```

例如：

```c
return ((x & 0x000000FFU) << 24) |
       ((x & 0x0000FF00U) << 8)  |
       ((x & 0x00FF0000U) >> 8)  |
       ((x & 0xFF000000U) >> 24);
```

---

# 十五、判断CPU大小端 ⭐⭐⭐⭐⭐

经典题：

```c
int is_little_endian(void)
{
    uint16_t x = 1;

    return *(uint8_t *)&x == 1;
}
```

面试官经常继续问：

> 为什么可以用 `uint8_t *` 查看？

---

# 十六、无临时变量交换两个整数

```c
a ^= b;
b ^= a;
a ^= b;
```

不过你还应该告诉面试官：

> 实际工程里我更倾向临时变量，可读性更高。

这个回答会比死背 XOR 好。

---

# 十七、结构体相关 ⭐⭐⭐⭐⭐

## 38. 计算结构体大小

例如：

```c
struct Test {
    char a;
    int b;
    char c;
};
```

问：

```c
sizeof(struct Test)
```

以及为什么。

这实际上同时考：

```text
内存对齐
padding
CPU访问效率
```

---

## 39. 调整结构体成员减少内存浪费

比如：

```c
struct Test {
    char a;
    int b;
    char c;
};
```

怎么调整成员顺序。

---

## 40. 自己实现 offsetof

理解：

```c
#define MY_OFFSETOF(type, member) \
    ((size_t)&(((type *)0)->member))
```

重点是理解，不建议工程里自己造轮子。

---

# 十八、指针类 ⭐⭐⭐⭐⭐⭐⭐

这里是嵌入式面试的重灾区。

## 41. 指针交换两个变量

```c
void swap(int *a, int *b);
```

---

## 42. 指针遍历数组

禁止：

```c
arr[i]
```

只能用：

```c
*p
p++
```

---

## 43. 二级指针修改指针 ⭐⭐⭐⭐⭐

例如：

```c
void alloc_buffer(char **buf);
```

面试官问：

> 为什么不是 `char *buf`？

---

## 44. 函数指针 ⭐⭐⭐⭐⭐

声明：

```c
int (*func)(int, int);
```

实现：

```c
int add(int a, int b);
```

然后：

```c
func = add;
```

---

# 十九、callback ⭐⭐⭐⭐⭐

这是嵌入式非常重要的题。

实现：

```c
typedef void (*uart_callback_t)(uint8_t data);

void uart_register_callback(uart_callback_t cb);
```

收到 UART 数据后调用：

```c
callback(data);
```

你应该自己完整写一遍。

---

# 二十、函数指针数组 ⭐⭐⭐⭐

例如：

```c
void cmd_start(void);
void cmd_stop(void);
void cmd_reset(void);
```

通过：

```text
cmd = 0
cmd = 1
cmd = 2
```

调用对应函数。

可以写：

```c
void (*cmd_table[])(void) = {
    cmd_start,
    cmd_stop,
    cmd_reset
};
```

这和实际固件命令分发高度相关。

---

# 二十一、链表 ⭐⭐⭐⭐⭐

嵌入式校招一般不要求你写红黑树，但**单链表必须会**。

结构：

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;
```

必须练：

### 46. 创建链表

```c
Node *create_node(int value);
```

### 47. 头插法

```c
void push_front(Node **head, int value);
```

### 48. 尾插法

```c
void push_back(Node **head, int value);
```

### 49. 删除节点

```c
void delete_node(Node **head, int value);
```

### 50. 查找节点

```c
Node *find_node(Node *head, int value);
```

### 51. 计算链表长度

### 52. 链表逆序 ⭐⭐⭐⭐⭐⭐⭐

```text
1 → 2 → 3 → NULL

↓

3 → 2 → 1 → NULL
```

必须裸写：

```c
Node *reverse_list(Node *head);
```

核心：

```c
prev
curr
next
```

### 53. 找链表中间节点

快慢指针。

### 54. 判断链表是否有环 ⭐⭐⭐⭐⭐

```c
slow
fast
```

### 55. 找倒数第K个节点

双指针。

### 56. 合并两个有序链表

够用了。

---

# 二十二、栈 ⭐⭐⭐⭐

自己用数组实现：

```c
typedef struct {
    int data[100];
    int top;
} Stack;
```

实现：

```c
stack_init()
push()
pop()
peek()
is_empty()
is_full()
```

---

# 二十三、普通队列 ⭐⭐⭐⭐

实现：

```c
queue_init()
enqueue()
dequeue()
is_empty()
is_full()
```

但是嵌入式真正重点在下一题。

---

# 二十四、环形缓冲区 Ring Buffer ⭐⭐⭐⭐⭐⭐⭐⭐⭐

这是我非常建议你重点练的一道。

甚至比你刷 20 道 `LeetCode` 都值。

```c
#define BUFFER_SIZE 128

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
} RingBuffer;
```

要求实现：

```c
void rb_init(RingBuffer *rb);

bool rb_write(RingBuffer *rb, uint8_t data);

bool rb_read(RingBuffer *rb, uint8_t *data);

bool rb_is_empty(const RingBuffer *rb);

bool rb_is_full(const RingBuffer *rb);
```

你必须搞清楚：

```text
head
tail
空
满
回绕
```

比如：

```c
head = (head + 1) % BUFFER_SIZE;
```

进一步会问：

> 如果 BUFFER_SIZE 是 2 的幂，怎么优化？

```c
head = (head + 1) & (BUFFER_SIZE - 1);
```

这道属于**嵌入式核心手撕题**。

---

# 二十五、排序算法

不需要疯狂刷算法。

这几个必须会。

## 63. 冒泡排序 ⭐⭐⭐⭐⭐

```c
void bubble_sort(int *arr, int n);
```

---

## 64. 选择排序 ⭐⭐⭐

---

## 65. 插入排序 ⭐⭐⭐⭐

实际上小数组在嵌入式里还挺值得理解。

---

## 66. 快速排序 ⭐⭐⭐⭐

至少会写基本版本：

```text
partition
递归
```

但优先级明显低于 RingBuffer。

---

# 二十六、查找

## 67. 顺序查找

---

## 68. 二分查找 ⭐⭐⭐⭐⭐

```c
int binary_search(const int *arr, int n, int target);
```

一定注意：

```c
mid = left + (right - left) / 2;
```

---

# 二十七、递归

不需要狂练。

会下面几个即可。

## 69. 阶乘

```c
int factorial(int n);
```

然后面试官可能问：

> 嵌入式系统为什么不推荐大量递归？

答案涉及：

```text
栈空间
无法预测最大调用深度
实时性
stack overflow
```

---

## 70. Fibonacci

能写递归版本。

但应该主动知道递归版本效率低。

---

# 二十八、宏定义题 ⭐⭐⭐⭐⭐⭐⭐

这类特别像嵌入式笔试。

## 71.

```c
#define MAX(a,b) ?
```

基本：

```c
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
```

面试官继续问：

```c
MAX(i++, j++)
```

发生什么？

这是宏副作用。

---

## 72. ARRAY_SIZE

```c
#define ARRAY_SIZE(arr) \
    (sizeof(arr) / sizeof((arr)[0]))
```

然后：

> 为什么传指针进去失效？

必须会。

---

## 73. container_of

Linux 嵌入式岗可能问：

```c
container_of(ptr, type, member)
```

普通 MCU 岗优先级低一点。

---

# 二十九、寄存器操作 ⭐⭐⭐⭐⭐⭐⭐

例如：

```c
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
```

要求：

> 把 GPIOA pin5 设置成输出模式。

面试官其实是在考：

```text
volatile
地址
位运算
读-修改-写
```

---

# 三十、volatile 场景 ⭐⭐⭐⭐⭐⭐⭐

可能让你改：

```c
int flag = 0;

while (flag == 0)
{
}
```

如果 `flag` 在 ISR 中修改：

```c
volatile int flag;
```

但一定要知道：

> `volatile` ≠ 原子性  
> `volatile` ≠ 线程安全

这是非常高频的追问。

---

# 三十一、软件延时/计时器题

## 76. 判断超时 ⭐⭐⭐⭐⭐

例如：

```c
uint32_t start = get_tick();
```

实现：

```c
bool timeout(uint32_t start,
             uint32_t timeout_ms);
```

经典写法：

```c
return (uint32_t)(get_tick() - start) >= timeout_ms;
```

这还能处理 tick 溢出。

非常嵌入式。

---

# 三十二、按键消抖 ⭐⭐⭐⭐⭐

要求：

```text
GPIO检测到按下
10~20ms后再次确认
```

但面试官可能要求：

> **不能使用 delay。**

于是你需要写非阻塞状态机：

```text
IDLE
↓
DEBOUNCE
↓
PRESSED
↓
RELEASE
```

---

# 三十三、LED非阻塞闪烁 ⭐⭐⭐⭐⭐

不要：

```c
LED_ON();
delay_ms(500);
LED_OFF();
delay_ms(500);
```

而是：

```c
if (tick - last_tick >= 500) {
    LED_TOGGLE();
    last_tick = tick;
}
```

这是典型嵌入式思维题。

---

# 三十四、状态机 FSM ⭐⭐⭐⭐⭐⭐⭐⭐⭐

例如串口 AT 命令：

```text
IDLE
↓
SEND_CMD
↓
WAIT_RESPONSE
↓
SUCCESS
或者
ERROR
```

要求写：

```c
typedef enum
{
    STATE_IDLE,
    STATE_SEND,
    STATE_WAIT,
    STATE_SUCCESS,
    STATE_ERROR
} State;
```

然后：

```c
void state_machine_run(void);
```

这类题你一定得会。

---

# 三十五、UART帧解析 ⭐⭐⭐⭐⭐⭐⭐⭐⭐

这是**顶级重要**。

假设协议：

```text
AA 55 LEN CMD DATA... CRC
```

例如：

```text
AA 55 03 01 12 34 56 78
```

让你实现：

```c
void parser_input(uint8_t byte);
```

要求能够处理：

```text
逐字节接收
帧头
长度
命令
payload
CRC
错误恢复
丢包
粘包
半包
```

标准思路就是：

```text
WAIT_HEADER1
WAIT_HEADER2
WAIT_LEN
WAIT_DATA
WAIT_CRC
```

状态机。

⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐

---

# 三十六、TLV解析 ⭐⭐⭐⭐⭐⭐

协议：

```text
TYPE
LENGTH
VALUE
```

比如：

```text
01 02 12 34
02 01 56
```

实现：

```c
void parse_tlv(const uint8_t *buf, size_t len);
```

重点：

```text
越界检查
length是否合法
未知TYPE跳过
```

---

# 三十七、大小端协议转换 ⭐⭐⭐⭐⭐⭐⭐

例如串口收到：

```text
12 34 56 78
```

这是 big endian。

转换：

```c
uint32_t value =
    ((uint32_t)buf[0] << 24) |
    ((uint32_t)buf[1] << 16) |
    ((uint32_t)buf[2] << 8) |
    buf[3];
```

必须会。

---

# 三十八、uint32_t 拆成字节 ⭐⭐⭐⭐⭐

```c
uint32_t data = 0x12345678;
```

拆：

```text
12
34
56
78
```

---

# 三十九、CRC / 校验 ⭐⭐⭐⭐⭐⭐

## 84. XOR checksum

```c
uint8_t checksum(const uint8_t *data, size_t len);
```

---

## 85. 累加和 checksum

---

## 86. CRC8

至少理解：

```text
初始值
多项式
移位
XOR
```

---

## 87. CRC16

例如 Modbus CRC16。

嵌入式岗有一定概率要求解释或补代码。

不用把所有 CRC 多项式背下来，但是你必须理解算法结构。

---

# 四十、内存池 ⭐⭐⭐⭐⭐

高级一点会让你：

> 在不使用 malloc 的情况下实现简单固定块内存分配。

例如：

```c
#define BLOCK_SIZE 32
#define BLOCK_NUM  16
```

实现：

```c
void *pool_alloc(void);
void pool_free(void *ptr);
```

这是非常典型的嵌入式思维。

---

# 四十一、不允许 malloc 的链表

面试官可能突然加条件：

> MCU 裸机环境，不允许动态内存。

于是：

```c
Node nodes[32];
```

自己管理：

```text
used/free
```

这就比单纯 `LeetCode` 更符合实际。

---

# 四十二、固定长度 FIFO ⭐⭐⭐⭐⭐

UART：

```text
ISR生产数据
main消费者
```

实现：

```c
fifo_put()
fifo_get()
```

实际上就是 RingBuffer。

但还会继续问：

> ISR 和主循环同时访问 head/tail 有问题吗？

于是进入：

```text
volatile
原子操作
临界区
单生产者单消费者
```

---

# 四十三、生产者消费者思维 ⭐⭐⭐⭐⭐⭐

例如：

```text
UART ISR
    ↓
RingBuffer
    ↓
main loop
    ↓
protocol parser
```

让你写代码。

这个架构你要熟到可以现场画出来。

---

# 四十四、ISR代码改错 ⭐⭐⭐⭐⭐⭐⭐

例如：

```c
void USART_IRQHandler(void)
{
    printf("receive data");
    malloc(100);
    delay_ms(100);
}
```

问：

> 有什么问题？

你应该找出：

```text
printf耗时
malloc不可预测
delay阻塞
ISR过长
可能重入
```

改成：

```c
void USART_IRQHandler(void)
{
    uint8_t data = UART_READ();

    rb_write(&rx_buffer, data);
}
```

然后 main loop 处理。

---

# 四十五、volatile + ISR flag

```c
volatile uint8_t uart_rx_flag = 0;

void UART_IRQHandler(void)
{
    uart_rx_flag = 1;
}

int main(void)
{
    while (1)
    {
        if (uart_rx_flag)
        {
            uart_rx_flag = 0;
            process_uart();
        }
    }
}
```

面试官可能追问：

> 如果 flag 是 32 位变量呢？  
> 如果 MCU 对这个宽度的访问不是原子的呢？

---

# 四十六、临界区 ⭐⭐⭐⭐⭐

例如：

```c
counter++;
```

被 main 和 ISR 同时访问。

让你保证安全。

可能要求：

```text
关中断
↓
修改
↓
恢复中断
```

重点不是背 API，而是理解：

```text
read
modify
write
```

不是原子操作。

---

# 四十七、DMA缓冲区处理 ⭐⭐⭐⭐⭐⭐⭐

例如：

```text
UART DMA一次接收128 bytes
```

让你：

```c
void uart_rx_process(uint8_t *buf,
                     uint16_t len);
```

然后解析：

```text
AT response
packet
TLV
```

有时还会结合：

```text
DMA + IDLE interrupt
```

---

# 四十八、双缓冲 Ping-Pong Buffer ⭐⭐⭐⭐⭐

```text
DMA写Buffer A
CPU处理Buffer B

↓切换

DMA写Buffer B
CPU处理Buffer A
```

要求设计：

```c
uint8_t buffer[2][128];
```

很符合嵌入式岗位。

---

# 四十九、AT命令解析 ⭐⭐⭐⭐⭐⭐⭐

例如接收到：

```text
AT+CWJAP...
OK
```

让你判断：

```text
OK
ERROR
FAIL
```

不能简单：

```c
strcmp(rxbuf, "OK")
```

因为可能收到：

```text
\r\nWIFI CONNECTED\r\nOK\r\n
```

所以要考虑：

```text
strstr
环形缓冲区
状态机
超时
```

---

# 五十、命令表设计 ⭐⭐⭐⭐⭐

例如串口 shell：

```text
led on
led off
reset
version
```

定义：

```c
typedef void (*cmd_func_t)(void);

typedef struct
{
    const char *name;
    cmd_func_t func;
} Command;
```

然后：

```c
Command command_table[] = {
    {"reset", reset_handler},
    {"version", version_handler}
};
```

让你实现命令查找和执行。

这道特别适合嵌入式面试。

---

# 五十一、IAP固件头解析 ⭐⭐⭐⭐⭐⭐

假设：

```c
typedef struct
{
    uint32_t magic;
    uint32_t size;
    uint32_t version;
    uint32_t crc;
} FirmwareHeader;
```

要求：

```text
检查magic
检查size
检查CRC
检查版本
```

实现：

```c
bool firmware_verify(
    const FirmwareHeader *header,
    const uint8_t *firmware);
```

---

# 五十二、Magic Header搜索

例如串口数据：

```text
00 33 AA 55 5A A5 ...
```

查找：

```text
0xAA 0x55 0x5A 0xA5
```

要求找到帧头。

---

# 五十三、Flash边界检查 ⭐⭐⭐⭐⭐

写：

```c
bool flash_write(uint32_t addr,
                 const uint8_t *data,
                 size_t len);
```

首先必须验证：

```text
addr是否合法
addr + len是否越界
len是否为0
data是否NULL
```

---

# 五十四、对齐函数 ⭐⭐⭐⭐⭐

例如地址向上4字节对齐：

```c
uint32_t align_up_4(uint32_t x)
{
    return (x + 3U) & ~3U;
}
```

通用版本：

```c
align_up(value, alignment)
```

要求 `alignment` 为 2 的幂。

---

# 五十五、判断地址是否对齐

```c
bool is_aligned(uintptr_t addr,
                uint32_t alignment);
```

核心：

```c
(addr & (alignment - 1)) == 0
```

---

# 五十六、有限状态协议解析器 ⭐⭐⭐⭐⭐⭐⭐⭐⭐

这个我建议你单独刷。

给定：

```text
HEADER = 0xAA55

LEN

CMD

PAYLOAD

CRC
```

要求：

```c
void protocol_receive_byte(uint8_t byte);
```

只能一次传一个 byte。

必须解决：

```text
帧同步
丢帧
半包
错包
CRC错误
长度错误
重新同步
```

如果你能把这题写好，嵌入式校招已经非常有用了。

---

# 五十七、按位打印整数

```c
void print_binary(uint32_t value);
```

输出：

```text
10101010...
```

---

# 五十八、寻找最低位的1

```c
x & -x
```

理解原理。

---

# 五十九、清除最低位的1

```c
x &= x - 1;
```

---

# 六十、获取最低 n 位

```c
x & ((1U << n) - 1)
```

注意：

```text
n == 32
```

的问题。

这是面试官很喜欢追问的边界。

---

# 六十一、循环左移/右移 ⭐⭐⭐⭐

```c
uint32_t rotate_left(uint32_t x,
                     unsigned int n);
```

不同于普通 `<<`。

---

# 六十二、奇偶判断

禁止 `%`：

```c
x & 1
```

---

# 六十三、不用乘法计算 x * 8

```c
x << 3
```

然后要知道：

> 有符号整数溢出不能乱搞。

---

# 六十四、绝对值

```c
int my_abs(int x);
```

追问：

```text
INT_MIN 怎么处理？
```

非常适合考边界意识。

---

# 六十五、判断整数加法是否溢出 ⭐⭐⭐⭐⭐

例如：

```c
bool safe_add_int(int a, int b, int *result);
```

这是比普通 `LeetCode` 更有工程味的题。

---

# 六十六、解析十进制数据

串口收到：

```text
TEMP=25
```

要求把：

```text
"25"
```

转成：

```c
int temp = 25;
```

---

# 六十七、解析浮点/定点数据

例如：

```text
23.45
```

但是 MCU 不想使用 `float`。

转成：

```text
2345
```

表示：

```text
23.45 × 100
```

这种定点思维很值得练。

---

# 六十八、BCD转换 ⭐⭐⭐⭐

例如：

```text
0x25
```

表示十进制：

```text
25
```

要求：

```c
uint8_t bcd_to_decimal(uint8_t bcd);
uint8_t decimal_to_bcd(uint8_t value);
```

RTC相关岗位很容易碰到。

---

# 六十九、简单滑动平均滤波 ⭐⭐⭐⭐⭐

传感器：

```text
10 11 9 10 100 10...
```

要求实现最近 N 个数据的平均：

```c
int moving_average(int new_sample);
```

最好不要每次重新累加 N 个：

```text
sum -= oldest;
sum += newest;
```

再配合 RingBuffer。

这是一道很好的综合题。

---

# 七十、限幅滤波

```c
if (value > max)
    value = max;
```

或者检测：

```text
|new-old| > threshold
```

判定异常数据。

---

# 七十一、软件看门狗任务表 ⭐⭐⭐⭐

例如：

```text
Task A
Task B
Task C
```

各自需要定期喂软件 watchdog。

要求设计：

```c
uint32_t task_alive_flags;
```

利用 bit：

```text
bit0 A
bit1 B
bit2 C
```

全部正常：

```c
flags == 0x07
```

才喂硬件 watchdog。

这就是非常典型的：

> 位运算 + 嵌入式系统设计。

---

# 七十二、事件标志位

定义：

```c
#define EVENT_UART  (1U << 0)
#define EVENT_TIMER (1U << 1)
#define EVENT_KEY   (1U << 2)
```

实现：

```c
event_set()
event_clear()
event_test()
```

---

# 七十三、有限长度日志缓冲区

实现：

```text
只保留最近N条log
```

本质：

```text
RingBuffer
```

---

# 七十四、消息队列

定义：

```c
typedef struct
{
    uint8_t type;
    uint8_t data[16];
    uint8_t len;
} Message;
```

实现：

```c
msg_queue_put()
msg_queue_get()
```

比纯 int Queue 更贴近实际工程。

---

# 七十五、排序结构体

例如：

```c
typedef struct
{
    uint16_t id;
    int temperature;
} Sensor;
```

按照：

```text
temperature
```

排序。

---

# 七十六、查表 LUT ⭐⭐⭐⭐

例如：

```c
ADC value
↓
temperature
```

给你一个查找表：

```c
const uint16_t table[];
```

要求：

```text
查最近值
```

或者插值。

---

# 七十七、线性插值

```text
(x1,y1)
(x2,y2)

给 x 求 y
```

实现：

```c
int linear_interpolation(...);
```

嵌入式传感器标定很常见。

---

# 七十八、软件 PWM

要求：

```text
一个1ms Tick
实现100级PWM
```

思路：

```c
counter++;

if (counter < duty)
    GPIO_HIGH;
else
    GPIO_LOW;
```

---

# 七十九、循环任务调度 ⭐⭐⭐⭐⭐

裸机：

```c
while (1)
{
}
```

要求：

```text
task1 每10ms
task2 每100ms
task3 每1000ms
```

实现：

```c
if (tick - last1 >= 10)
```

这是很好的裸机调度手撕题。

---

# 八十、简单软件定时器 ⭐⭐⭐⭐⭐⭐

实现：

```c
timer_start()
timer_stop()
timer_expired()
```

例如：

```c
typedef struct
{
    uint32_t start;
    uint32_t period;
    bool running;
} SoftTimer;
```

---

# 八十一、无阻塞 UART 发送

不要：

```c
while (!(UART->SR & TXE));
```

而是设计：

```text
TX FIFO
+
TXE interrupt
```

实现：

```c
uart_send_async();
```

这是高级但很好的手撕题。

---

# 八十二、双链表

优先级：

⭐⭐⭐

会：

```text
插入
删除
```

即可。

不用投入太多。

---

# 八十三、二叉树

只准备：

```text
前序
中序
后序
层序
```

以及：

```text
求高度
```

优先级：

⭐⭐⭐

嵌入式 MCU 校招一般明显低于链表/RingBuffer。

---

# 八十四、哈希表

知道原理：

```text
hash
bucket
collision
```

即可。

通常不用作为主要手撕内容。

---

# 八十五、排序复杂度必须知道

即使不让你写代码，也可能追问：

```text
冒泡       O(n²)
选择       O(n²)
插入       O(n²)
快速排序   平均O(nlogn)
二分查找   O(logn)
```

还要知道空间复杂度。

---

# 八十六、经典 C 代码改错

这个在笔试里非常多：

```c
char *func(void)
{
    char buf[10] = "hello";
    return buf;
}
```

问：

> 哪里错？

因为：

```text
buf在栈上
函数返回后生命周期结束
```

---

# 八十七、malloc错误

```c
int *p = malloc(10);

for (int i = 0; i < 10; i++)
    p[i] = i;
```

哪里有问题？

应该：

```c
malloc(sizeof(int) * 10);
```

---

# 八十八、内存泄漏

```c
void func(void)
{
    int *p = malloc(100);
}
```

缺：

```c
free(p);
```

---

# 八十九、野指针

```c
int *p = malloc(sizeof(int));

free(p);

*p = 10;
```

Use After Free。

建议：

```c
free(p);
p = NULL;
```

---

# 九十、重复释放

```c
free(p);
free(p);
```

---

# 九十一、NULL指针检查

写：

```c
int process(const uint8_t *buf,
            size_t len)
```

第一件事：

```c
if (buf == NULL)
    return -1;
```

当然，也要根据 API contract 决定。

---

# 九十二、数组越界

```c
int a[10];

for (int i = 0; i <= 10; i++)
```

找 bug。

---

# 九十三、sizeof 数组 vs 指针 ⭐⭐⭐⭐⭐

```c
void func(int arr[])
{
    printf("%zu", sizeof(arr));
}
```

这里：

```c
arr
```

实际上是：

```c
int *
```

经典 C 笔试题。

---

# 九十四、static变量题 ⭐⭐⭐⭐⭐

```c
int func(void)
{
    static int x = 0;
    return ++x;
}
```

连续：

```c
func();
func();
func();
```

结果？

---

# 九十五、static函数

```c
static void func(void);
```

问作用：

```text
限制链接范围
只在当前 translation unit 可见
```

---

# 九十六、const 指针辨析 ⭐⭐⭐⭐⭐

必须秒答：

```c
const int *p;
```

```text
不能通过p修改*p
p可以改变
```

---

```c
int * const p;
```

```text
p不能变
*p可以改变
```

---

```c
const int * const p;
```

两个都限制。

---

# 九十七、volatile 指针

```c
volatile uint32_t *reg;
```

为什么寄存器通常用：

```c
volatile
```

必须解释。

---

# 九十八、typedef函数指针

```c
typedef void (*Callback)(int);
```

让你解释：

```c
Callback cb;
```

到底是什么。

---

# 九十九、宏和 inline 的区别

可能让你：

> 把宏改成 static inline function。

例如：

```c
static inline uint32_t min_u32(uint32_t a,
                               uint32_t b)
{
    return a < b ? a : b;
}
```

需要解释为什么更安全。

---

# 一百、综合大题：串口通信模块 ⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐

如果我只允许你准备一道嵌入式综合手撕题，就是它：

```text
UART ISR
    ↓
RingBuffer
    ↓
Protocol Parser
    ↓
CRC Check
    ↓
Command Dispatcher
    ↓
Callback
```

要求自己实现：

```c
uart_rx_isr()

ringbuffer_write()

ringbuffer_read()

protocol_parser()

crc_check()

command_dispatch()

callback()
```

这一个项目基本同时覆盖：

```text
指针
数组
结构体
typedef
函数指针
callback
volatile
ISR
ring buffer
状态机
协议
CRC
大小端
bit操作
边界处理
模块化
```

这比单刷 `LeetCode` 价值大很多。

---

# 你秋招真正要达到的水平

我建议把题目分成三个等级。

## 第一档：必须闭眼手写

这些你应该做到**不用查资料，15分钟内基本无 bug**：

```text
strlen
strcpy
strcmp
memcpy
memmove

数组逆序
字符串逆序
atoi

SET_BIT
CLEAR_BIT
GET_BIT
位域提取
大小端转换
统计bit 1

swap
函数指针
callback

单链表：
插入
删除
查找
反转
判环

Stack
Queue
RingBuffer

冒泡
插入排序
二分查找

超时判断
非阻塞LED
按键消抖
状态机

UART协议解析
TLV
checksum
CRC基本思想
```

这些是你的**主战场**。

---

# 第二档：必须能设计出来

```text
UART ISR + RingBuffer

DMA Buffer

Ping-Pong Buffer

AT命令状态机

Message Queue

Command Table

Firmware Header

IAP校验

Flash边界检查

软件Timer

裸机Scheduler

Memory Pool
```

面试官可能不会要求全部写完，但你应该能快速写框架。

---

# 第三档：了解即可

你没必要在秋招前疯狂投入：

```text
红黑树
AVL
Trie
复杂图论
Dijkstra
Floyd
并查集
复杂动态规划
回溯组合搜索
复杂DFS/BFS
Segment Tree
Fenwick Tree
```

除非你投：

```text
互联网C++
算法岗
Linux系统软件
自动驾驶算法基础设施
高性能计算
```

否则性价比很低。

---

# `LeetCode`只刷这些就够了

你还是可以把 `LeetCode` 当**数据结构练习器**，而不是主线。

推荐只刷类似：

```text
1    Two Sum                 看思路即可
20   Valid Parentheses       Stack
21   Merge Two Sorted Lists  链表
26   Remove Duplicates       数组
27   Remove Element          数组
88   Merge Sorted Array      数组
141  Linked List Cycle       快慢指针
160  Intersection List       链表
206  Reverse Linked List     必刷
283  Move Zeroes             数组
344  Reverse String          必刷
704  Binary Search           必刷
```

做完这些以后，**不要继续陷进 `LeetCode` Medium 海洋里**。

把时间转去：

```text
C语言手撕
↓
RingBuffer
↓
FSM
↓
UART解析
↓
DMA
↓
Callback
↓
CRC
↓
寄存器
↓
ISR
```

会更符合嵌入式秋招。

---

# 我给你建议的刷题顺序

你现在不要直接从上面100题随机刷。

按照这个顺序：

```text
第一阶段
C基础
│
├─ 指针
├─ 数组
├─ 字符串
├─ memcpy/memmove
├─ static
├─ const
└─ volatile

        ↓

第二阶段
位运算
│
├─ bit操作
├─ mask
├─ 寄存器
├─ 大小端
└─ 对齐

        ↓

第三阶段
数据结构
│
├─ Linked List
├─ Stack
├─ Queue
└─ RingBuffer ★★★★★

        ↓

第四阶段
基础算法
│
├─ Bubble
├─ Insertion
├─ Binary Search
└─ 简单双指针

        ↓

第五阶段
嵌入式手撕
│
├─ Timer
├─ Debounce
├─ FSM
├─ UART
├─ DMA
├─ TLV
├─ CRC
└─ Callback

        ↓

第六阶段
综合题

UART ISR
   ↓
RingBuffer
   ↓
Packet Parser
   ↓
CRC
   ↓
Command Dispatch
   ↓
Callback
```

**这才是一条适合嵌入式秋招的刷题路线。**

尤其如果目标是 STM32/MCU/嵌入式软件岗，我甚至会认为：**能完整手写一个高质量 RingBuffer + UART 状态机协议解析器，比做出好几道 `LeetCode` Medium 更能体现你的实际水平。**

下一步最适合的是把上面的内容进一步做成一套 **「嵌入式秋招纯 C 手撕代码 100 题」**：每一道我给你固定成 **题目 → 面试官考点 → 你先自己写 → 标准 C 答案 → 常见错误 → 面试追问** 的格式。这样你可以每天按 5 题直接刷，最终形成自己的手撕代码题库。