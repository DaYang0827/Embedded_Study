# 1 与字符串相关的函数

## 1.1 `strlen`

```c
size_t strlen(const char *str);
```

`strlen` 是处理字符串（以 `\0` 结尾的字符数组）最基础的函数

- **工作原理**：`strlen` 的内部实现非常简单粗暴。它接收一个内存地址，然后从这个地址开始**逐字节**向后扫描，直到遇到空字符 `\0`（ASCII 码为 0）为止。
- **返回值**：返回 `\0` 之前的所有字符个数。**注意：计算结果不包含 `\0` 自身。**
- **时间复杂度**：\(O(N)\)。因为必须遍历到 `\0`，字符串越长，耗时越久。

需要注意的是：返回值是 `size_t`（无符号数）

```c
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

`sizeof`和`strlen`两个操作在概念上有本质区别：

|      特性       |     `strlen()`     |        `sizeof`        |
| :-----------: | :----------------: | :--------------------: |
|    **本质**     | 是一个**函数**（运行时计算）。  | 是一个**运算符/关键字**（编译时计算）。 |
|   **计算对象**    | 只能计算字符串的**实际字符数**。 | 计算变量或类型占用的**总内存字节数**。  |
| **是否包含 `\0`** |      **不包含**。      |   **包含**（如果是字符数组的话）。   |

```c
char arr[100] = "Hello";

printf("%zu\n", strlen(arr)); // 输出: 5 （实际字符有 H-e-l-l-o 共 5 个）
printf("%zu\n", sizeof(arr)); // 输出: 100（数组在内存中占用了 100 个字节）

```

## 1.2 `strcpy`
```c
char* strcpy(char* dest, const char* src);
```

`strcpy`是**字符串拷贝**的函数

- **`strcpy` 机制**：它内部利用了类似 `strlen` 的机制，从 `src` (source源地址)逐个字符复制到 `dest`(destination目的地址)，**直到遇到 `\0` 才会停下来**（连同 `\0` 一起拷过去）。如果 `dest` 空间不够，会直接造成缓冲区溢出崩溃。
- **`strncpy` 机制**：最多只拷贝 `n` 个字符。如果 `src` 的长度小于 `n`，剩余部分用 `\0` 填充；如果大于等于 `n`，则**不会**在 `dest` 结尾添加 `\0`（需要手动补 `\0`）。

```c
char* my_strcpy(char *dest, const char *src)
{
    char *ret = dest;

    if (dest == NULL || src == NULL) 
    {
        return NULL;
    }

    while ((*dest++ = *src++) != '\0') 
    {
    
    }

    return ret;
}
```

## 1.3 `strcmp`

   进行**字符串比较**
```c
int strcmp(const char *str1, const char *str2);
```
- **机制**：也是从首地址开始，逐个字符比较 ASCII 码值，直到遇到不同的字符或者遇到 `\0` 为止。如果完全相同返回 `0`

```c
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

**考点：**
- 返回 <0、=0、>0
- 不一定返回 -1 / 0 / 1
- 用 unsigned char 避免 char 符号问题

## 1.4 `strcat`

   进行**字符串拼接**
```c
char* strcat(char* dest, const char* src);
```

**机制**：它首先会利用 `strlen` 的原理找到 `dest` 字符串结尾的 `\0`，然后从该位置开始，将 `src` 的内容覆盖复制过去，直到遇到 `src` 的 `\0` 为止。

```c
char* my_strcat(char *dest, const char *src)
{
    char *ret = dest;

    if (dest == NULL || src == NULL) {
        return NULL;
    }

    while (*dest != '\0') {
        dest++;
    }
	/*要注意运算顺序，！=的运算优先级要比赋值(=)的优先级高，如果不加括号会变成先判断*src++!='\0',值变成1再赋值给*dest++
	*/
    while ((*dest++ = *src++) != '\0')  
	{
	
	}

    return ret;
}
```

风险：和 `strcpy` 一样，无法知道 `dest `剩余空间，**容易溢出**。

## 1.5 `strstr`

   是标准库中极其高频的**字符串查找函数**
```c
char* strstr(const char *str, const char *substr);
```
- **参数命名直译**：
    - `str`：要在其中进行查找的**原字符串**（大字符串）。
    - `substr`：想寻找的**子字符串**（小字符串）。
- **返回值机制（非常核心）**：
    - **找到了**：返回子串 `substr` 在原串 `str` 中**第一次出现的位置的指针**。
    - **没找到**：返回 `NULL`。
    - **特例**：如果 `needle` 是一个空字符串 `" "`，函数会直接返回 `haystack` 的首地址。

```c
char* my_strstr(const char *str, const char *substr)
{
	//判断是否为空指针
    if (str == NULL || substr == NULL) {
        return NULL;
    }
    
	//判断子字符串是否为空
    if (*substr == '\0') {
        return (char *)str;
    }
    
	/*
	for 函数是为了游历str字符串的所有内容，把str的每一个字符都当作“可能的子字符串起点”
	
	"hello world" 里是不是从 h 开始匹配 world？
	"ello world"  里是不是从 e 开始匹配 world？
	"llo world"   里是不是从 l 开始匹配 world？
	...
	"world"       里是不是从 w 开始匹配 world？
	*/
    for (; *str != '\0'; str++) 
    {
    
    /*
      str指针是记录当前准备尝试匹配的位置
      p指针是负责向后移动进行比较
      substr是保存子字符串的开头
      q指针是负责在子字符串里面移动
    */
        const char *p = str;
        const char *q = substr;
        
        /*
        同时符合三个条件，
        条件1：主字符串还没结束
        条件2：子字符串还没结束
        条件3：当前两个字符相同，
        这三个条件是为了确保子字符串里面的所有字符在主字符串里面都存在
        */
        while (*p != '\0' && *q != '\0' && *p == *q) 
        {
            p++;
            q++;
        }
		
		//表示当前这一轮已经完整的匹配成功
        if (*q == '\0') 
        {
            return (char *)str;
        }
    }
    
	//如果所有位置都试完，还是没有找到就返回NULL
    return NULL;
}
```

这是字符串匹配基础题，嵌入式里常用于 **AT 命令、串口响应解析。**

# 2 字符串转换

## 2.1 字符串逆序

```c
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

## 2.2 判断回文

```c
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

## 2.3 `atoi`

是**字符串转整数**（ASCII to Integer）的函数，标准写法是

```c
int atoi(const char *str);
```

工作机制：`atoi` 会扫描输入的字符串，跳过前面的**空格字符**，直到遇到第一个非空格字符。

- 如果第一个非空格字符是 **正号（`+`）或 负号（`-`）**，它会记录符号。
- 接着开始处理后面的**数字字符**，将其累加转换为对应的整数。
- 如果遇到**非数字字符**（如字母、特殊符号），它会**立即停止**扫描，并返回前面已经转换好的部分。
- 如果字符串开头就是非法字符，则返回 `0`。

```c
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
## 2.4 `itoa`

是**整数转字符串**（Integer to ASCII）的函数，标准写法为

```c
char* itoa(int value, char* str, int base);
```

- `value`：要转换的整数。
- `str`：目标字符串缓冲区（**必须由调用者分配好足够的空间**）。
- `base`：转换的**进制**（如 `10` 表示十进制，`2` 表示二进制，`16` 表示十六进制）。


```c
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

## 2.5 十六进制字符串转整数

```c
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

# 3 内存函数

## 3.1 `memcpy`
   
   进行高效的**块拷贝**

```c
void* memcpy(void* dest, const void* src, size_t n);
```

与`strcpy`的区别：
-  **驱动机制不同**：字符串函数（如 `strcpy`）是 **“数据驱动”** 的，它们遇到 `\0` 就会提前终止；内存函数是 **“计数驱动”** 的，它们完全不管内存里装的是什么，不看 `\0`，只看你指定的**字节数**（ `n`）。
-  **处理对象不同**：字符串函数只能处理文本；内存函数可以处理**任何二进制数据**（如结构体、整型数组、图片像素、网络原始报文等）。

```c
void *my_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    if (dst == NULL || src == NULL) 
    {
        return NULL;
    }

    while (n--) 
    {
        *d++ = *s++;
    }

    return dst;
}
```

核心回答：`memcpy` **用于不重叠内存区域之间的复制**。如果源和目标区域重叠，结果未定义，应该使用 `memmove`。

## 3.2 `memmove`

   是**安全的带重叠拷贝**
```c
   void* memmove(void* dest, const void* src, size_t n);
```

- **核心点**：它的功能和 `memcpy` 一模一样，但它**允许并能完美处理内存重叠**。
- **代价**：在拷贝前需要进行一次指针位置的逻辑判断，因此性能理论上比 `memcpy` 稍慢一点点。

```c
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

如果从前往后复制，原始数据会被覆盖，所以**必须倒着拷贝**。

---
## 3.3 `memset`

   是**内存高效初始化**
```c
void* memset(void* str, int c, size_t n);
```
- **核心点**：将指定的内存区域前 `n` 个字节全部设置为字符/数值 `c`。
- **最常见用途**：清空结构体或数组（`memset(&my_struct, 0, sizeof(my_struct));`）。

```c
void *my_memset(void *ptr, int value, size_t n)
{
    unsigned char *p = ptr;

    if (ptr == NULL) 
    {
        return NULL;
    }

    while (n--) 
    {
        *p++ = (unsigned char)value;
    }

    return ptr;
}
```
考点：memset **是按字节写，不是按 int 写**。

所以：

```c
int arr[10];
memset(arr, 1, sizeof(arr));
```

不会得到每个元素都是整数 `1`，而是每个字节都是 `0x01`。

# 4 数组与边界

## 4.1 最大值

```c
int find_max(const int *arr, int n)
{
    int max;

    if (arr == NULL || n <= 0) 
    {
        return 0;
    }

    max = arr[0];

    for (int i = 1; i < n; i++) 
    {
        if (arr[i] > max) 
        {
            max = arr[i];
        }
    }

    return max;
}
```

---
## 4.2 最小值
```c
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
## 4.3 求和
```c
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

---
## 4.4 平均值
```c
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

重点：**不能直接 int / int，否则是整数除法**。

--- 
## 4.5 数组逆序

```c
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
考点：双指针

## 4.6 删除指定元素

```c
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

## 4.7 有序数组去重
```c
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
要求数组有序
## 4.8 合并两个有序数组
```c
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

## 4.9 第二大元素
```c
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
## 4.10 找唯一出现一次的数
```c
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

```text
a ^ a = 0
a ^ 0 = a
异或满足交换律和结合律
```

## 4.11 找缺失数字

```c
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
