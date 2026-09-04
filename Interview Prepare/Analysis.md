
这份内容应该归纳到你的知识体系里，位置是：

**嵌入式软件基础能力 → C 语言工程能力 → 手撕代码 / 面试高频题库**

更细一点，可以这样放：

```
嵌入式软件知识体系
├─ C语言核心
│  ├─ 指针 / 数组 / 字符串
│  ├─ 内存函数 memcpy / memmove / memset
│  ├─ const / static / volatile / typedef
│  └─ 宏 / inline / sizeof / 结构体对齐
│
├─ 位操作与寄存器
│  ├─ bit set / clear / toggle / get
│  ├─ mask / 位域提取 / 位域修改
│  ├─ 大小端 / 对齐 / BCD
│  └─ volatile寄存器访问
│
├─ 数据结构
│  ├─ 链表
│  ├─ 栈 / 队列
│  ├─ 环形缓冲区 RingBuffer
│  └─ 消息队列 / 内存池
│
├─ 基础算法
│  ├─ 排序
│  ├─ 查找
│  ├─ 双指针
│  └─ 简单递归
│
├─ 嵌入式场景代码
│  ├─ ISR / 临界区 / 并发
│  ├─ UART / DMA / FIFO
│  ├─ 状态机 FSM
│  ├─ 协议解析 / TLV / CRC
│  └─ Timer / Debounce / Scheduler
│
└─ 综合工程题
   └─ UART ISR → RingBuffer → Parser → CRC → Command → Callback
```

你这份材料本质上不是普通算法题，而是 **“嵌入式 C 工程能力训练路线”**。面试官真正想看的是：你能不能写出边界正确、内存安全、可在 MCU 上运行的 C 代码。

---

## 一、整体解析：这份内容的核心价值

这份题库覆盖了嵌入式 C 面试最常见的几类能力：

```
C语法是否扎实
↓
指针和内存是否安全
↓
位操作是否熟练
↓
能否写基础数据结构
↓
能否处理串口 / 协议 / ISR / 状态机
↓
能否组合成一个可运行的模块
```

和 LeetCode 不同，嵌入式面试很少考复杂动态规划、图论、红黑树。它更喜欢问：

```
#define SET_BIT(REG, BIT) ((REG) |= (1U << (BIT)))
```

或者：

```
void UART_IRQHandler(void)
{
    uint8_t data = UART_READ();
    rb_write(&rx_buffer, data);
}
```

所以你的主线应该是：

```
C语言手撕 60%
嵌入式场景题 25%
基础算法数据结构 15%
```

---

# 二、第一大类：C 语言基本功

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 字符串 / 指针 / 内存
```

## 1. strlen

```
#include <stddef.h>

size_t my_strlen(const char *str)
{
    size_t len = 0;

    if (str == NULL) {
        return 0;
    }

    while (*str != '\0') {
        len++;
        str++;
    }

    return len;
}
```

考点：

```
1. 字符串以 '\0' 结尾
2. 参数应该用 const char *
3. 返回值应该用 size_t
4. NULL 是否处理，要看函数约定
```

面试回答重点：

> `strlen` 只统计 `\0` 前面的字符数量，不包含 `\0` 本身。标准库 `strlen(NULL)` 是未定义行为，工程里如果追求健壮性，可以主动判断 NULL。

---

## 2. strcpy

```
char *my_strcpy(char *dst, const char *src)
{
    char *ret = dst;

    if (dst == NULL || src == NULL) {
        return NULL;
    }

    while ((*dst++ = *src++) != '\0') {
    }

    return ret;
}
```

考点：

```
1. 返回原始 dst
2. src 要 const
3. 会复制 '\0'
4. 不检查目标缓冲区大小
```

面试追问：

> 为什么 `strcpy` 危险？

回答：

> 因为函数不知道 `dst` 的容量，如果 `src` 比 `dst` 大，会发生缓冲区溢出。工程里更倾向使用带长度参数的接口，例如 `snprintf` 或自己封装安全版本。

---

## 3. strncpy

```
char *my_strncpy(char *dst, const char *src, size_t n)
{
    char *ret = dst;

    if (dst == NULL || src == NULL) {
        return NULL;
    }

    while (n > 0 && *src != '\0') {
        *dst++ = *src++;
        n--;
    }

    while (n > 0) {
        *dst++ = '\0';
        n--;
    }

    return ret;
}
```

重点：

```
strncpy 不一定保证字符串以 '\0' 结尾。
如果 src 长度 >= n，目标数组不会自动补 '\0'。
```

更安全的工程写法通常是：

```
dst[n - 1] = '\0';
```

但前提是 `n > 0`。

---

## 4. strcmp

```
int my_strcmp(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL) {
        return 0;
    }

    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}
```

考点：

```
1. 返回 <0、=0、>0
2. 不一定返回 -1 / 0 / 1
3. 用 unsigned char 避免 char 符号问题
```

---

## 5. strcat

```
char *my_strcat(char *dst, const char *src)
{
    char *ret = dst;

    if (dst == NULL || src == NULL) {
        return NULL;
    }

    while (*dst != '\0') {
        dst++;
    }

    while ((*dst++ = *src++) != '\0') {
    }

    return ret;
}
```

风险：

```
和 strcpy 一样，无法知道 dst 剩余空间，容易溢出。
```

---

## 6. strstr

```
char *my_strstr(const char *str, const char *substr)
{
    if (str == NULL || substr == NULL) {
        return NULL;
    }

    if (*substr == '\0') {
        return (char *)str;
    }

    for (; *str != '\0'; str++) {
        const char *p = str;
        const char *q = substr;

        while (*p != '\0' && *q != '\0' && *p == *q) {
            p++;
            q++;
        }

        if (*q == '\0') {
            return (char *)str;
        }
    }

    return NULL;
}
```

归纳点：

```
这是字符串匹配基础题，嵌入式里常用于 AT 命令、串口响应解析。
```

例如：

```
if (my_strstr(rx_buf, "OK") != NULL) {
    // AT命令成功
}
```

---

# 三、内存函数

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 内存操作
```

## 7. memcpy

```
void *my_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    if (dst == NULL || src == NULL) {
        return NULL;
    }

    while (n--) {
        *d++ = *s++;
    }

    return dst;
}
```

核心回答：

> `memcpy` 用于不重叠内存区域之间的复制。如果源和目标区域重叠，结果未定义，应该使用 `memmove`。

---

## 8. memmove

```
void *my_memmove(void *dst, const void *src, size_t n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    if (dst == NULL || src == NULL) {
        return NULL;
    }

    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else if (d > s) {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }

    return dst;
}
```

重点解释：

```
dst < src：从前往后复制
dst > src：从后往前复制
```

例如：

```
memmove(buf + 2, buf, 4);
```

如果从前往后复制，原始数据会被覆盖，所以必须倒着拷贝。

---

## 9. memset

```
void *my_memset(void *ptr, int value, size_t n)
{
    unsigned char *p = ptr;

    if (ptr == NULL) {
        return NULL;
    }

    while (n--) {
        *p++ = (unsigned char)value;
    }

    return ptr;
}
```

考点：

```
memset 是按字节写，不是按 int 写。
```

所以：

```
int arr[10];
memset(arr, 1, sizeof(arr));
```

不会得到每个元素都是整数 `1`，而是每个字节都是 `0x01`。

---

# 四、数组题

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 数组与边界
```

## 10. 最大值

```
int find_max(const int *arr, int n)
{
    int max;

    if (arr == NULL || n <= 0) {
        return 0;
    }

    max = arr[0];

    for (int i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    return max;
}
```

面试重点：

```
数组为空怎么办？
n <= 0 怎么办？
返回错误码还是默认值？
```

工程里更好的设计是：

```
bool find_max(const int *arr, int n, int *out);
```

---

## 11. 最小值

```
int find_min(const int *arr, int n)
{
    int min;

    if (arr == NULL || n <= 0) {
        return 0;
    }

    min = arr[0];

    for (int i = 1; i < n; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }

    return min;
}
```

---

## 12. 求和

```
long long sum_array(const int *arr, int n)
{
    long long sum = 0;

    if (arr == NULL || n <= 0) {
        return 0;
    }

    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}
```

如果可能溢出：

```
1. 用更大类型，例如 int64_t
2. 每次加之前检查是否超过范围
3. 返回 bool 表示是否成功
```

---

## 13. 平均值

```
double average(const int *arr, int n)
{
    long long sum = 0;

    if (arr == NULL || n <= 0) {
        return 0.0;
    }

    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    return (double)sum / n;
}
```

重点：

```
不能直接 int / int，否则是整数除法。
```

---

## 14. 数组逆序

```
void reverse_array(int *arr, int n)
{
    int left = 0;
    int right = n - 1;

    if (arr == NULL || n <= 1) {
        return;
    }

    while (left < right) {
        int tmp = arr[left];
        arr[left] = arr[right];
        arr[right] = tmp;

        left++;
        right--;
    }
}
```

考点：

```
O(n) 时间
O(1) 空间
双指针
```

---

## 15. 删除指定元素

```
int remove_value(int *arr, int n, int value)
{
    int write = 0;

    if (arr == NULL || n <= 0) {
        return 0;
    }

    for (int read = 0; read < n; read++) {
        if (arr[read] != value) {
            arr[write++] = arr[read];
        }
    }

    return write;
}
```

这就是典型双指针。

---

## 16. 有序数组去重

```
int remove_duplicates(int *arr, int n)
{
    int write = 1;

    if (arr == NULL || n <= 0) {
        return 0;
    }

    for (int read = 1; read < n; read++) {
        if (arr[read] != arr[write - 1]) {
            arr[write++] = arr[read];
        }
    }

    return write;
}
```

注意：

```
这个版本要求数组已有序。
```

---

## 17. 合并两个有序数组

```
int merge_sorted(const int *a, int n,
                 const int *b, int m,
                 int *out)
{
    int i = 0;
    int j = 0;
    int k = 0;

    if (a == NULL || b == NULL || out == NULL) {
        return 0;
    }

    while (i < n && j < m) {
        if (a[i] <= b[j]) {
            out[k++] = a[i++];
        } else {
            out[k++] = b[j++];
        }
    }

    while (i < n) {
        out[k++] = a[i++];
    }

    while (j < m) {
        out[k++] = b[j++];
    }

    return k;
}
```

---

## 18. 第二大元素

先确认定义：

```
是否允许重复？
5 5 4 的第二大是 5 还是 4？
```

如果要求“第二个不同的最大值”：

```
#include <stdbool.h>
#include <limits.h>

bool second_distinct_max(const int *arr, int n, int *out)
{
    int max1 = INT_MIN;
    int max2 = INT_MIN;
    bool has_max1 = false;
    bool has_max2 = false;

    if (arr == NULL || out == NULL || n <= 1) {
        return false;
    }

    for (int i = 0; i < n; i++) {
        int x = arr[i];

        if (!has_max1 || x > max1) {
            if (has_max1 && x != max1) {
                max2 = max1;
                has_max2 = true;
            }
            max1 = x;
            has_max1 = true;
        } else if (x < max1 && (!has_max2 || x > max2)) {
            max2 = x;
            has_max2 = true;
        }
    }

    if (!has_max2) {
        return false;
    }

    *out = max2;
    return true;
}
```

---

## 19. 找唯一出现一次的数

适用条件：

```
其他数字都出现两次，只有一个数字出现一次。
```

```
int find_single(const int *arr, int n)
{
    int result = 0;

    if (arr == NULL || n <= 0) {
        return 0;
    }

    for (int i = 0; i < n; i++) {
        result ^= arr[i];
    }

    return result;
}
```

原理：

```
a ^ a = 0
a ^ 0 = a
异或满足交换律和结合律
```

---

## 20. 找缺失数字

数组包含 `0 ~ n`，缺一个。

```
int missing_number(const int *arr, int n)
{
    int result = n;

    if (arr == NULL || n < 0) {
        return -1;
    }

    for (int i = 0; i < n; i++) {
        result ^= i;
        result ^= arr[i];
    }

    return result;
}
```

---

# 五、字符串转换题

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 字符串解析
```

## 21. 字符串逆序

```
void reverse_string(char *str)
{
    char *left;
    char *right;

    if (str == NULL) {
        return;
    }

    left = str;
    right = str;

    while (*right != '\0') {
        right++;
    }

    if (right == str) {
        return;
    }

    right--;

    while (left < right) {
        char tmp = *left;
        *left = *right;
        *right = tmp;

        left++;
        right--;
    }
}
```

---

## 22. 判断回文

```
#include <stdbool.h>
#include <string.h>

bool is_palindrome(const char *str)
{
    size_t left = 0;
    size_t right;

    if (str == NULL) {
        return false;
    }

    right = strlen(str);

    if (right == 0) {
        return true;
    }

    right--;

    while (left < right) {
        if (str[left] != str[right]) {
            return false;
        }

        left++;
        right--;
    }

    return true;
}
```

---

## 23. atoi

```
#include <limits.h>
#include <ctype.h>

int my_atoi(const char *str)
{
    int sign = 1;
    int result = 0;

    if (str == NULL) {
        return 0;
    }

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        int digit = *str - '0';

        if (sign == 1 && result > (INT_MAX - digit) / 10) {
            return INT_MAX;
        }

        if (sign == -1 && result > (INT_MAX - digit) / 10) {
            return INT_MIN;
        }

        result = result * 10 + digit;
        str++;
    }

    return sign * result;
}
```

重点：

```
空格
正负号
非法字符
溢出
```

---

## 24. itoa

```
#include <limits.h>

void my_itoa(int num, char *str)
{
    char buf[12];
    int i = 0;
    int j = 0;
    unsigned int value;

    if (str == NULL) {
        return;
    }

    if (num < 0) {
        str[j++] = '-';
        value = (unsigned int)(-(num + 1)) + 1U;
    } else {
        value = (unsigned int)num;
    }

    do {
        buf[i++] = (char)('0' + value % 10U);
        value /= 10U;
    } while (value != 0U);

    while (i > 0) {
        str[j++] = buf[--i];
    }

    str[j] = '\0';
}
```

为什么要特殊处理 `INT_MIN`：

```
因为在二补码系统里，INT_MIN 的绝对值比 INT_MAX 大 1。
直接 -INT_MIN 会溢出。
```

---

## 25. 十六进制字符串转整数

```
#include <stdint.h>
#include <stdbool.h>

bool hex_to_uint32(const char *str, uint32_t *out)
{
    uint32_t value = 0;

    if (str == NULL || out == NULL) {
        return false;
    }

    while (*str != '\0') {
        uint8_t digit;

        if (*str >= '0' && *str <= '9') {
            digit = (uint8_t)(*str - '0');
        } else if (*str >= 'a' && *str <= 'f') {
            digit = (uint8_t)(*str - 'a' + 10);
        } else if (*str >= 'A' && *str <= 'F') {
            digit = (uint8_t)(*str - 'A' + 10);
        } else {
            return false;
        }

        if (value > 0x0FFFFFFFU) {
            return false;
        }

        value = (value << 4) | digit;
        str++;
    }

    *out = value;
    return true;
}
```

---

# 六、位运算与寄存器

归档位置：

```
嵌入式软件知识体系 → 位操作与寄存器
```

## 基础 bit 操作

```
#define SET_BIT(x, n)      ((x) |=  (1U << (n)))
#define CLEAR_BIT(x, n)    ((x) &= ~(1U << (n)))
#define TOGGLE_BIT(x, n)   ((x) ^=  (1U << (n)))
#define GET_BIT(x, n)      (((x) >> (n)) & 1U)
```

必须注意：

```
1U << n 中 n 不能大于等于类型位宽。
例如 32位 unsigned int 上，1U << 32 是未定义行为。
```

---

## 取 bit[7:4]

```
uint32_t field = (data >> 4) & 0x0FU;
```

通用写法：

```
#define GET_FIELD(x, shift, mask) (((x) >> (shift)) & (mask))
```

---

## 修改寄存器 bit[7:4]

```
REG &= ~(0xFU << 4);
REG |=  (0xAU << 4);
```

通用写法：

```
#define MODIFY_FIELD(reg, mask, shift, value) \
    do {                                      \
        (reg) = ((reg) & ~((mask) << (shift))) | \
                (((value) & (mask)) << (shift)); \
    } while (0)
```

---

## 统计 1 的个数

```
int count_bits(uint32_t x)
{
    int count = 0;

    while (x != 0U) {
        x &= x - 1U;
        count++;
    }

    return count;
}
```

解释：

```
x & (x - 1) 会清掉 x 最低位的 1。
```

---

## 判断 2 的幂

```
#include <stdbool.h>
#include <stdint.h>

bool is_power_of_two(uint32_t x)
{
    return x != 0U && (x & (x - 1U)) == 0U;
}
```

---

## 交换高低四位

```
uint8_t swap_nibble(uint8_t x)
{
    return (uint8_t)((x << 4) | (x >> 4));
}
```

例如：

```
0xAB → 0xBA
```

---

## 32 位大小端交换

```
uint32_t swap32(uint32_t x)
{
    return ((x & 0x000000FFU) << 24) |
           ((x & 0x0000FF00U) << 8)  |
           ((x & 0x00FF0000U) >> 8)  |
           ((x & 0xFF000000U) >> 24);
}
```

---

## 判断 CPU 大小端

```
#include <stdint.h>

int is_little_endian(void)
{
    uint16_t x = 1;
    return *(uint8_t *)&x == 1U;
}
```

为什么可以用 `uint8_t *` 看？

```
C 语言允许通过字符类型指针查看对象的字节表示。
uint8_t 通常是 unsigned char 的 typedef。
```

---

# 七、结构体与内存对齐

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 结构体 / 对齐 / padding
```

例如：

```
struct Test {
    char a;
    int b;
    char c;
};
```

在常见 32 位平台上：

```
char a 占 1 字节
后面为了让 int b 4 字节对齐，填充 3 字节
int b 占 4 字节
char c 占 1 字节
结构体整体要按最大成员 int 的 4 字节对齐，所以末尾再填充 3 字节

总大小：12 字节
```

优化顺序：

```
struct Test {
    int b;
    char a;
    char c;
};
```

常见大小：

```
8 字节
```

---

## offsetof

```
#include <stddef.h>

#define MY_OFFSETOF(type, member) \
    ((size_t)&(((type *)0)->member))
```

面试解释：

```
把 0 地址强转成结构体指针，再取成员地址。
成员地址相对于 0 的偏移量，就是 member 在结构体中的 offset。
```

工程建议：

```
实际项目中直接用标准库 offsetof，不要自己造。
```

---

# 八、指针与函数指针

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 指针 / 函数指针 / callback
```

## 指针交换变量

```
void swap(int *a, int *b)
{
    int tmp;

    if (a == NULL || b == NULL) {
        return;
    }

    tmp = *a;
    *a = *b;
    *b = tmp;
}
```

---

## 二级指针修改指针

```
#include <stdlib.h>

void alloc_buffer(char **buf)
{
    if (buf == NULL) {
        return;
    }

    *buf = malloc(128);
}
```

为什么要 `char **buf`？

```
如果只传 char *buf，函数拿到的是指针的副本。
修改副本不会改变调用者手里的指针。
要修改调用者的指针变量本身，需要传它的地址，也就是 char **。
```

---

## 函数指针

```
int add(int a, int b)
{
    return a + b;
}

int (*func)(int, int);

func = add;

int result = func(1, 2);
```

---

## callback

```
#include <stdint.h>

typedef void (*uart_callback_t)(uint8_t data);

static uart_callback_t g_uart_callback = 0;

void uart_register_callback(uart_callback_t cb)
{
    g_uart_callback = cb;
}

void uart_rx_on_byte(uint8_t data)
{
    if (g_uart_callback != 0) {
        g_uart_callback(data);
    }
}
```

嵌入式里的意义：

```
驱动层不直接知道业务逻辑。
驱动收到数据后，通过 callback 通知上层。
```

---

## 函数指针数组

```
void cmd_start(void);
void cmd_stop(void);
void cmd_reset(void);

void (*cmd_table[])(void) = {
    cmd_start,
    cmd_stop,
    cmd_reset
};

void run_cmd(unsigned int cmd)
{
    if (cmd < sizeof(cmd_table) / sizeof(cmd_table[0])) {
        cmd_table[cmd]();
    }
}
```

归档：

```
嵌入式软件知识体系 → 模块化设计 → 命令分发 / 回调机制
```

---

# 九、链表

归档位置：

```
嵌入式软件知识体系 → 数据结构 → 单链表
```

## 节点定义

```
typedef struct Node {
    int data;
    struct Node *next;
} Node;
```

## 创建节点

```
#include <stdlib.h>

Node *create_node(int value)
{
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        return NULL;
    }

    node->data = value;
    node->next = NULL;

    return node;
}
```

---

## 头插法

```
void push_front(Node **head, int value)
{
    Node *node;

    if (head == NULL) {
        return;
    }

    node = create_node(value);
    if (node == NULL) {
        return;
    }

    node->next = *head;
    *head = node;
}
```

---

## 尾插法

```
void push_back(Node **head, int value)
{
    Node *node;
    Node *cur;

    if (head == NULL) {
        return;
    }

    node = create_node(value);
    if (node == NULL) {
        return;
    }

    if (*head == NULL) {
        *head = node;
        return;
    }

    cur = *head;
    while (cur->next != NULL) {
        cur = cur->next;
    }

    cur->next = node;
}
```

---

## 删除节点

```
void delete_node(Node **head, int value)
{
    Node *cur;
    Node *prev = NULL;

    if (head == NULL || *head == NULL) {
        return;
    }

    cur = *head;

    while (cur != NULL) {
        if (cur->data == value) {
            if (prev == NULL) {
                *head = cur->next;
            } else {
                prev->next = cur->next;
            }

            free(cur);
            return;
        }

        prev = cur;
        cur = cur->next;
    }
}
```

---

## 查找节点

```
Node *find_node(Node *head, int value)
{
    while (head != NULL) {
        if (head->data == value) {
            return head;
        }

        head = head->next;
    }

    return NULL;
}
```

---

## 链表反转

```
Node *reverse_list(Node *head)
{
    Node *prev = NULL;
    Node *curr = head;
    Node *next = NULL;

    while (curr != NULL) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    return prev;
}
```

必须能解释：

```
prev：已经反转好的前半部分
curr：当前正在处理的节点
next：提前保存后续链表，避免断链
```

---

## 判断链表是否有环

```
#include <stdbool.h>

bool has_cycle(Node *head)
{
    Node *slow = head;
    Node *fast = head;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return true;
        }
    }

    return false;
}
```

---

# 十、栈、队列、环形缓冲区

归档位置：

```
嵌入式软件知识体系 → 数据结构 → Stack / Queue / RingBuffer
```

## 栈

```
#include <stdbool.h>

#define STACK_SIZE 100

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

void stack_init(Stack *s)
{
    s->top = 0;
}

bool stack_push(Stack *s, int value)
{
    if (s->top >= STACK_SIZE) {
        return false;
    }

    s->data[s->top++] = value;
    return true;
}

bool stack_pop(Stack *s, int *out)
{
    if (s->top <= 0 || out == NULL) {
        return false;
    }

    *out = s->data[--s->top];
    return true;
}
```

---

## 环形缓冲区

这是整份内容里最重要的题之一。

```
#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 128

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
} RingBuffer;

void rb_init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

bool rb_is_empty(const RingBuffer *rb)
{
    return rb->head == rb->tail;
}

bool rb_is_full(const RingBuffer *rb)
{
    return ((rb->head + 1U) % BUFFER_SIZE) == rb->tail;
}

bool rb_write(RingBuffer *rb, uint8_t data)
{
    uint16_t next = (rb->head + 1U) % BUFFER_SIZE;

    if (next == rb->tail) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = next;

    return true;
}

bool rb_read(RingBuffer *rb, uint8_t *data)
{
    if (data == NULL || rb_is_empty(rb)) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1U) % BUFFER_SIZE;

    return true;
}
```

关键解释：

```
head 指向下一次写入位置
tail 指向下一次读取位置
head == tail 表示空
(head + 1) % size == tail 表示满
这种实现会浪费一个字节空间，用来区分空和满
```

如果 `BUFFER_SIZE` 是 2 的幂：

```
rb->head = (rb->head + 1U) & (BUFFER_SIZE - 1U);
```

前提：

```
BUFFER_SIZE 必须是 2、4、8、16、32、64、128...
```

---

# 十一、排序与查找

归档位置：

```
嵌入式软件知识体系 → 基础算法
```

## 冒泡排序

```
void bubble_sort(int *arr, int n)
{
    if (arr == NULL || n <= 1) {
        return;
    }

    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;

        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
                swapped = true;
            }
        }

        if (!swapped) {
            break;
        }
    }
}
```

复杂度：

```
时间：O(n²)
空间：O(1)
```

---

## 插入排序

```
void insertion_sort(int *arr, int n)
{
    if (arr == NULL || n <= 1) {
        return;
    }

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}
```

嵌入式价值：

```
小数组、近乎有序数组时，插入排序简单且效果不错。
```

---

## 二分查找

```
int binary_search(const int *arr, int n, int target)
{
    int left = 0;
    int right = n - 1;

    if (arr == NULL || n <= 0) {
        return -1;
    }

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return mid;
        }

        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}
```

重点：

```
mid = left + (right - left) / 2
避免 left + right 溢出。
```

---

# 十二、宏、static、const、volatile

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 关键字与宏
```

## MAX 宏

```
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
```

问题：

```
MAX(i++, j++)
```

可能导致参数被求值多次，产生副作用。

更好的做法：

```
static inline int max_int(int a, int b)
{
    return a > b ? a : b;
}
```

---

## ARRAY_SIZE

```
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

为什么传指针会失效？

```
数组作为函数参数时会退化成指针。
sizeof(arr) 得到的是指针大小，不是数组总大小。
```

---

## const 指针

```
const int *p;
```

含义：

```
不能通过 p 修改 *p
p 自己可以指向别处
```

```
int * const p;
```

含义：

```
p 不能指向别处
可以通过 p 修改 *p
```

```
const int * const p;
```

含义：

```
p 不能变
*p 也不能通过 p 修改
```

---

## static 变量

```
int func(void)
{
    static int x = 0;
    return ++x;
}
```

连续调用三次返回：

```
1
2
3
```

原因：

```
static 局部变量生命周期贯穿整个程序运行期间，但作用域仍限制在函数内部。
```

---

## static 函数

```
static void helper(void)
{
}
```

含义：

```
限制函数的链接范围，只能在当前 .c 文件中使用。
```

---

## volatile

```
volatile uint8_t flag;
```

使用场景：

```
1. ISR 和 main 共享变量
2. 硬件寄存器
3. DMA 修改的内存
4. 多线程或 RTOS 任务共享的特殊变量
```

必须会说：

```
volatile 只保证每次都从内存读写，不保证原子性，也不保证线程安全。
```

---

# 十三、寄存器操作

归档位置：

```
嵌入式软件知识体系 → 位操作与寄存器 → MMIO
```

例如：

```
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000U)
```

把 GPIOA pin5 设置为输出模式。

假设每个 pin 占 2 bit：

```
pin5 对应 bit[11:10]
输出模式假设为 01
```

代码：

```
GPIOA_MODER &= ~(0x3U << (5U * 2U));
GPIOA_MODER |=  (0x1U << (5U * 2U));
```

考点：

```
1. volatile
2. 固定地址访问
3. 位清除
4. 位设置
5. 读-改-写
```

---

# 十四、定时、消抖、非阻塞

归档位置：

```
嵌入式软件知识体系 → 嵌入式场景代码 → Timer / Non-blocking
```

## 超时判断

```
#include <stdint.h>
#include <stdbool.h>

extern uint32_t get_tick(void);

bool timeout(uint32_t start, uint32_t timeout_ms)
{
    return (uint32_t)(get_tick() - start) >= timeout_ms;
}
```

为什么能处理 tick 溢出？

```
无符号整数溢出按模运算。
只要 timeout_ms 小于计数范围的一半，这种写法就是可靠的。
```

---

## 非阻塞 LED 闪烁

```
void led_task(void)
{
    static uint32_t last_tick = 0;

    if ((uint32_t)(get_tick() - last_tick) >= 500U) {
        LED_TOGGLE();
        last_tick = get_tick();
    }
}
```

重点：

```
不要 delay 阻塞。
主循环应该一直跑，让其他任务也有机会执行。
```

---

## 按键消抖状态机

```
typedef enum {
    KEY_IDLE,
    KEY_DEBOUNCE,
    KEY_PRESSED
} KeyState;

void key_task(void)
{
    static KeyState state = KEY_IDLE;
    static uint32_t tick = 0;

    switch (state) {
    case KEY_IDLE:
        if (KEY_IS_DOWN()) {
            tick = get_tick();
            state = KEY_DEBOUNCE;
        }
        break;

    case KEY_DEBOUNCE:
        if ((uint32_t)(get_tick() - tick) >= 20U) {
            if (KEY_IS_DOWN()) {
                state = KEY_PRESSED;
                on_key_pressed();
            } else {
                state = KEY_IDLE;
            }
        }
        break;

    case KEY_PRESSED:
        if (!KEY_IS_DOWN()) {
            state = KEY_IDLE;
        }
        break;
    }
}
```

---

# 十五、状态机 FSM

归档位置：

```
嵌入式软件知识体系 → 嵌入式场景代码 → 状态机
```

状态机的核心是：

```
当前状态
输入事件
状态转移
动作
```

例如 AT 命令：

```
typedef enum {
    STATE_IDLE,
    STATE_SEND,
    STATE_WAIT,
    STATE_SUCCESS,
    STATE_ERROR
} State;

static State state = STATE_IDLE;
static uint32_t start_tick;

void state_machine_run(void)
{
    switch (state) {
    case STATE_IDLE:
        if (need_send_cmd()) {
            state = STATE_SEND;
        }
        break;

    case STATE_SEND:
        uart_send("AT\r\n");
        start_tick = get_tick();
        state = STATE_WAIT;
        break;

    case STATE_WAIT:
        if (response_has_ok()) {
            state = STATE_SUCCESS;
        } else if ((uint32_t)(get_tick() - start_tick) >= 1000U) {
            state = STATE_ERROR;
        }
        break;

    case STATE_SUCCESS:
        handle_success();
        state = STATE_IDLE;
        break;

    case STATE_ERROR:
        handle_error();
        state = STATE_IDLE;
        break;
    }
}
```

面试关键词：

```
非阻塞
超时
错误恢复
状态清晰
```

---

# 十六、UART 协议解析

归档位置：

```
嵌入式软件知识体系 → 协议解析 → UART Parser / FSM / CRC
```

假设协议：

```
AA 55 LEN CMD DATA... CRC
```

逐字节解析：

```
#define MAX_PAYLOAD 64

typedef enum {
    WAIT_HEADER1,
    WAIT_HEADER2,
    WAIT_LEN,
    WAIT_CMD,
    WAIT_PAYLOAD,
    WAIT_CRC
} ParserState;

typedef struct {
    ParserState state;
    uint8_t len;
    uint8_t cmd;
    uint8_t payload[MAX_PAYLOAD];
    uint8_t index;
} Parser;

uint8_t checksum_xor(const uint8_t *data, unsigned int len)
{
    uint8_t sum = 0;

    for (unsigned int i = 0; i < len; i++) {
        sum ^= data[i];
    }

    return sum;
}

void parser_init(Parser *p)
{
    p->state = WAIT_HEADER1;
    p->len = 0;
    p->cmd = 0;
    p->index = 0;
}

void handle_packet(uint8_t cmd, const uint8_t *payload, uint8_t len);

void parser_input(Parser *p, uint8_t byte)
{
    switch (p->state) {
    case WAIT_HEADER1:
        if (byte == 0xAAU) {
            p->state = WAIT_HEADER2;
        }
        break;

    case WAIT_HEADER2:
        if (byte == 0x55U) {
            p->state = WAIT_LEN;
        } else if (byte == 0xAAU) {
            p->state = WAIT_HEADER2;
        } else {
            p->state = WAIT_HEADER1;
        }
        break;

    case WAIT_LEN:
        if (byte <= MAX_PAYLOAD) {
            p->len = byte;
            p->index = 0;
            p->state = WAIT_CMD;
        } else {
            p->state = WAIT_HEADER1;
        }
        break;

    case WAIT_CMD:
        p->cmd = byte;
        if (p->len == 0U) {
            p->state = WAIT_CRC;
        } else {
            p->state = WAIT_PAYLOAD;
        }
        break;

    case WAIT_PAYLOAD:
        p->payload[p->index++] = byte;
        if (p->index >= p->len) {
            p->state = WAIT_CRC;
        }
        break;

    case WAIT_CRC: {
        uint8_t calc = p->cmd;

        calc ^= p->len;
        for (uint8_t i = 0; i < p->len; i++) {
            calc ^= p->payload[i];
        }

        if (calc == byte) {
            handle_packet(p->cmd, p->payload, p->len);
        }

        p->state = WAIT_HEADER1;
        break;
    }
    }
}
```

必须能讲：

```
1. 逐字节接收
2. 帧头同步
3. 长度检查
4. payload 越界保护
5. CRC / checksum 校验
6. 出错后回到 WAIT_HEADER1
7. 能处理半包、粘包、错包
```

---

# 十七、TLV 解析

归档位置：

```
嵌入式软件知识体系 → 协议解析 → TLV
```

```
void parse_tlv(const uint8_t *buf, size_t len)
{
    size_t i = 0;

    if (buf == NULL) {
        return;
    }

    while (i + 2U <= len) {
        uint8_t type = buf[i++];
        uint8_t length = buf[i++];

        if (length > len - i) {
            break;
        }

        switch (type) {
        case 0x01:
            handle_type_1(&buf[i], length);
            break;

        case 0x02:
            handle_type_2(&buf[i], length);
            break;

        default:
            break;
        }

        i += length;
    }
}
```

重点：

```
length 必须做越界检查。
未知 type 应该跳过，而不是直接崩。
```

---

# 十八、CRC / Checksum

归档位置：

```
嵌入式软件知识体系 → 协议解析 → 校验
```

## XOR checksum

```
uint8_t checksum_xor(const uint8_t *data, size_t len)
{
    uint8_t sum = 0;

    if (data == NULL) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        sum ^= data[i];
    }

    return sum;
}
```

## 累加和

```
uint8_t checksum_sum8(const uint8_t *data, size_t len)
{
    uint8_t sum = 0;

    if (data == NULL) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        sum = (uint8_t)(sum + data[i]);
    }

    return sum;
}
```

CRC 面试回答：

```
CRC 的核心是把数据看作二进制多项式，然后用指定多项式做模 2 除法。
工程里要明确：初始值、多项式、输入反转、输出反转、最终异或值。
```

---

# 十九、ISR 与并发

归档位置：

```
嵌入式软件知识体系 → 嵌入式场景代码 → ISR / 并发 / 临界区
```

错误 ISR：

```
void USART_IRQHandler(void)
{
    printf("receive data");
    malloc(100);
    delay_ms(100);
}
```

问题：

```
printf 慢
malloc 不确定
delay 阻塞
ISR 时间过长
可能重入
影响实时性
```

正确思路：

```
void USART_IRQHandler(void)
{
    uint8_t data = UART_READ();
    rb_write(&rx_buffer, data);
}
```

然后主循环处理：

```
void main_loop(void)
{
    uint8_t data;

    while (rb_read(&rx_buffer, &data)) {
        parser_input(&parser, data);
    }
}
```

---

## volatile flag

```
volatile uint8_t uart_rx_flag = 0;

void UART_IRQHandler(void)
{
    uart_rx_flag = 1;
}

int main(void)
{
    while (1) {
        if (uart_rx_flag) {
            uart_rx_flag = 0;
            process_uart();
        }
    }
}
```

必须回答：

```
volatile 可以防止编译器把 flag 缓存在寄存器里。
但 volatile 不保证原子性。
```

---

## 临界区

```
void counter_inc_safe(void)
{
    uint32_t primask = disable_irq();

    counter++;

    restore_irq(primask);
}
```

重点：

```
counter++ 不是一条抽象原子操作。
它通常包含 read → add → write。
如果 ISR 中断发生在中间，就可能丢计数。
```

---

# 二十、DMA、双缓冲、生产者消费者

归档位置：

```
嵌入式软件知识体系 → 外设通信 → UART / DMA / Buffer
```

## DMA + IDLE 思路

```
UART DMA 接收到数据
↓
IDLE 中断说明一帧或一段数据结束
↓
计算本次接收长度
↓
把数据送给 parser 或 ring buffer
↓
重新启动 DMA
```

## Ping-Pong Buffer

```
#define DMA_BUF_SIZE 128

static uint8_t dma_buf[2][DMA_BUF_SIZE];
static volatile uint8_t active_buf = 0;

void dma_complete_callback(void)
{
    uint8_t done = active_buf;

    active_buf ^= 1U;

    dma_start(dma_buf[active_buf], DMA_BUF_SIZE);

    process_buffer_later(done);
}
```

解释：

```
DMA 写一个 buffer，CPU 处理另一个 buffer。
这样可以减少接收丢数据的概率。
```

---

# 二十一、命令表设计

归档位置：

```
嵌入式软件知识体系 → 模块化设计 → Command Table
```

```
#include <string.h>
#include <stdbool.h>

typedef void (*cmd_func_t)(void);

typedef struct {
    const char *name;
    cmd_func_t func;
} Command;

void reset_handler(void);
void version_handler(void);

static const Command command_table[] = {
    {"reset", reset_handler},
    {"version", version_handler},
};

bool command_execute(const char *name)
{
    if (name == NULL) {
        return false;
    }

    for (size_t i = 0; i < sizeof(command_table) / sizeof(command_table[0]); i++) {
        if (strcmp(name, command_table[i].name) == 0) {
            command_table[i].func();
            return true;
        }
    }

    return false;
}
```

面试价值：

```
函数指针
结构体
查表
模块化
串口 shell
```

---

# 二十二、Flash 边界检查

归档位置：

```
嵌入式软件知识体系 → 存储与启动 → Flash / IAP
```

```
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define FLASH_START 0x08000000U
#define FLASH_SIZE  0x00100000U
#define FLASH_END   (FLASH_START + FLASH_SIZE)

bool flash_range_valid(uint32_t addr, const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0U) {
        return false;
    }

    if (addr < FLASH_START) {
        return false;
    }

    if (len > FLASH_END - addr) {
        return false;
    }

    return true;
}
```

重点：

```
不要直接判断 addr + len > FLASH_END。
因为 addr + len 可能整数溢出。
```

---

# 二十三、对齐

归档位置：

```
嵌入式软件知识体系 → 位操作与寄存器 → 地址对齐
```

## 向上 4 字节对齐

```
uint32_t align_up_4(uint32_t x)
{
    return (x + 3U) & ~3U;
}
```

## 通用 2 的幂对齐

```
uint32_t align_up(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1U) & ~(alignment - 1U);
}
```

前提：

```
alignment 必须是 2 的幂。
```

## 判断地址是否对齐

```
#include <stdint.h>
#include <stdbool.h>

bool is_aligned(uintptr_t addr, uint32_t alignment)
{
    return alignment != 0U &&
           (alignment & (alignment - 1U)) == 0U &&
           (addr & (alignment - 1U)) == 0U;
}
```

---

# 二十四、固定点、BCD、滤波

归档位置：

```
嵌入式软件知识体系 → 传感器数据处理
```

## 定点数解析

字符串：

```
"23.45"
```

转成：

```
2345
```

含义：

```
实际值 = 2345 / 100
```

好处：

```
避免 MCU 使用 float，减少代码体积和运算开销。
```

---

## BCD 转十进制

```
uint8_t bcd_to_decimal(uint8_t bcd)
{
    return (uint8_t)(((bcd >> 4) * 10U) + (bcd & 0x0FU));
}

uint8_t decimal_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10U) << 4) | (value % 10U));
}
```

常见场景：

```
RTC 芯片
时钟日历
传感器寄存器
```

---

## 滑动平均

```
#define AVG_N 8

typedef struct {
    int buf[AVG_N];
    int sum;
    unsigned int index;
    unsigned int count;
} MovingAverage;

int moving_average_update(MovingAverage *ma, int sample)
{
    if (ma->count < AVG_N) {
        ma->buf[ma->index] = sample;
        ma->sum += sample;
        ma->count++;
    } else {
        ma->sum -= ma->buf[ma->index];
        ma->buf[ma->index] = sample;
        ma->sum += sample;
    }

    ma->index = (ma->index + 1U) % AVG_N;

    return ma->sum / (int)ma->count;
}
```

归纳：

```
这是 RingBuffer + 传感器滤波 的综合题。
```

---

# 二十五、软件调度与软件定时器

归档位置：

```
嵌入式软件知识体系 → 裸机架构 → Scheduler / SoftTimer
```

## 循环任务调度

```
void scheduler_loop(void)
{
    static uint32_t last_10ms = 0;
    static uint32_t last_100ms = 0;
    static uint32_t last_1000ms = 0;
    uint32_t now = get_tick();

    if ((uint32_t)(now - last_10ms) >= 10U) {
        last_10ms = now;
        task_10ms();
    }

    if ((uint32_t)(now - last_100ms) >= 100U) {
        last_100ms = now;
        task_100ms();
    }

    if ((uint32_t)(now - last_1000ms) >= 1000U) {
        last_1000ms = now;
        task_1000ms();
    }
}
```

---

## 软件定时器

```
typedef struct {
    uint32_t start;
    uint32_t period;
    bool running;
} SoftTimer;

void timer_start(SoftTimer *t, uint32_t period)
{
    t->start = get_tick();
    t->period = period;
    t->running = true;
}

void timer_stop(SoftTimer *t)
{
    t->running = false;
}

bool timer_expired(const SoftTimer *t)
{
    if (!t->running) {
        return false;
    }

    return (uint32_t)(get_tick() - t->start) >= t->period;
}
```

---

# 二十六、经典 C 改错题

归档位置：

```
嵌入式软件知识体系 → C语言核心 → 常见 Bug
```

## 返回局部数组

错误：

```
char *func(void)
{
    char buf[10] = "hello";
    return buf;
}
```

原因：

```
buf 是栈变量，函数返回后生命周期结束。
返回它的地址会形成悬空指针。
```

---

## malloc 大小错误

错误：

```
int *p = malloc(10);
```

正确：

```
int *p = malloc(sizeof(int) * 10);
```

---

## use after free

错误：

```
free(p);
*p = 10;
```

正确习惯：

```
free(p);
p = NULL;
```

---

## 数组越界

错误：

```
int a[10];

for (int i = 0; i <= 10; i++) {
    a[i] = i;
}
```

原因：

```
合法下标是 0 到 9。
i <= 10 会访问 a[10]，越界。
```

---

## sizeof 数组退化

```
void func(int arr[])
{
    printf("%zu", sizeof(arr));
}
```

这里 `arr` 实际是：

```
int *arr
```

所以 `sizeof(arr)` 是指针大小，不是数组长度。

---

# 二十七、综合大题：串口通信模块

归档位置：

```
嵌入式软件知识体系 → 综合项目 → UART通信模块
```

这是最值得你重点整理的一题。

架构：

```
UART ISR
↓
RingBuffer
↓
Main Loop
↓
Protocol Parser
↓
Checksum / CRC
↓
Command Dispatcher
↓
Callback
```

标准回答框架：

```
void UART_IRQHandler(void)
{
    if (UART_RX_READY()) {
        uint8_t data = UART_READ();
        rb_write(&rx_rb, data);
    }
}

void uart_process(void)
{
    uint8_t data;

    while (rb_read(&rx_rb, &data)) {
        parser_input(&parser, data);
    }
}

void main_loop(void)
{
    while (1) {
        uart_process();
        key_task();
        led_task();
        scheduler_loop();
    }
}
```

面试官想看到的是：

```
1. ISR 里只做最少事情
2. 数据先进 RingBuffer
3. 主循环慢慢解析
4. 协议解析用状态机
5. 校验失败能恢复
6. 命令用表驱动
7. 上层通过 callback 解耦
```

这题如果你能讲清楚，嵌入式校招会很加分。

---

# 二十八、你应该怎么整理成自己的知识体系

我建议你不要按 1 到 100 顺序背，而是整理成 **8 个专题文件夹 / 笔记章节**。

## 1. C语言基础手撕

放这些：

```
strlen
strcpy
strcmp
strcat
strstr
memcpy
memmove
memset
atoi
itoa
sizeof
static
const
volatile
宏和 inline
```

目标：

```
保证 C 语言基础不丢分。
```

---

## 2. 指针与内存模型

放这些：

```
一级指针
二级指针
数组指针
函数指针
callback
结构体对齐
offsetof
malloc/free
野指针
内存泄漏
重复释放
```

目标：

```
形成“内存生命周期”和“指针传参”的直觉。
```

---

## 3. 位操作与寄存器

放这些：

```
SET_BIT
CLEAR_BIT
GET_BIT
位域提取
位域修改
大小端
bit count
2的幂
对齐
寄存器读改写
volatile register
```

目标：

```
看到寄存器配置题能秒写 mask。
```

---

## 4. 基础数据结构

放这些：

```
数组
链表
栈
普通队列
环形缓冲区
消息队列
内存池
```

重点：

```
RingBuffer 优先级最高。
```

---

## 5. 基础算法

放这些：

```
冒泡排序
选择排序
插入排序
快速排序基础版
顺序查找
二分查找
双指针
递归边界
```

目标：

```
够用，不要在复杂算法里陷太深。
```

---

## 6. 协议解析

放这些：

```
UART逐字节解析
AA 55 LEN CMD DATA CRC
TLV
AT命令
Magic Header
大小端协议转换
CRC8
CRC16
checksum
```

目标：

```
能写状态机 parser。
```

---

## 7. 裸机与实时性

放这些：

```
tick超时判断
非阻塞LED
按键消抖
软件定时器
裸机scheduler
软件PWM
watchdog任务表
事件标志位
```

目标：

```
从 delay 思维转成非阻塞任务思维。
```

---

## 8. ISR / DMA / 并发

放这些：

```
UART ISR
volatile flag
临界区
main 和 ISR 共享变量
DMA buffer
IDLE interrupt
Ping-Pong Buffer
生产者消费者
无阻塞 UART 发送
```

目标：

```
知道 ISR 里不能做重活，知道共享数据要考虑原子性。
```

---

# 二十九、最终复习优先级

如果你时间有限，按这个顺序来：

```
第一优先级：
strlen / strcpy / strcmp / memcpy / memmove
位操作
指针
volatile
RingBuffer
UART Parser
状态机
ISR 改错
二分查找
链表反转
```

```
第二优先级：
atoi / itoa
结构体对齐
callback
命令表
TLV
checksum / CRC
软件定时器
按键消抖
裸机调度
```

```
第三优先级：
快排
二叉树
哈希表
复杂递归
高级数据结构
```

---

# 三十、你可以把这份内容命名为

推荐标题：

```
嵌入式秋招 C语言手撕代码体系
```

或者：

```
Embedded C Interview Coding Roadmap
```

章节目录建议：

```
01_C语言基础
02_指针与内存
03_字符串与数组
04_位运算与寄存器
05_结构体与宏
06_链表栈队列
07_RingBuffer
08_排序查找
09_Timer与状态机
10_UART协议解析
11_ISR与并发
12_DMA与缓冲区
13_Flash与IAP
14_综合串口通信模块
```

你现在最应该做的不是继续扩题，而是把每一道题固定成这个模板：

```
题目
考点
标准代码
边界条件
常见错误
面试追问
工程应用场景
```

这样整理之后，这份材料就不是散乱题库，而是一套真正能用于秋招复习的 **嵌入式 C 知识体系**。