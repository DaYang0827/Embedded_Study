# 指针第一轮
## 第一组：`&` 和 `*` 基础

### 第1题

```
int a = 10;
int *p = &a;

printf("%d\n", a);
printf("%d\n", *p);
```

问：

1. `a` 是什么？
2. `&a` 是什么？
3. `p` 里面保存什么？
4. `*p` 是什么？
5. 两个 `printf` 输出什么？
```text
1.a 是定义的int类型的变量
2.&a 是a这个变量所在的地址
3.p里面保存的是a变量的地址
4.*p 代表把这个地址对应的内容取出来
5. 第一个输出10，第二个输出10
```

---

### 第2题

```
int a = 10;
int *p = &a;

*p = 20;

printf("%d\n", a);
```

为什么最后 `a` 会发生变化？
```text
a也变成了20，通过把a的地址转给指针p，在修改p对应的值，会直接修改原地址里面的内容
```

---

### 第3题

```
int a = 10;
int *p = &a;

printf("%p\n", (void *)&a);
printf("%p\n", (void *)p);
printf("%p\n", (void *)&p);
```

问：

```
&a
p
&p
```

这三个东西有什么区别？

这一题非常重要。
```text
&a代表的是变量a的地址
p代表指针，里面存放a的地址
&p代表p这个指针的地址
```
---

### 第4题

```
int a = 10;
int *p = &a;
int **pp = &p;
```

分别解释：

```
p
*p
pp
*pp
**pp
```

最终：

```
**pp
```

是多少？
```text
p代表里面存放a变量的地址
*p代表取出地址对应的值即为10
pp里面存放的是p这个指针的地址
*pp代表取出这个地址里面的内容   是p对应的地址
**pp代表取出p对应地址的内容    即为10
```
---

## 第二组：指针类型和 `p++`

### 第5题(出错点)

假设：

```
uint8_t *p = (uint8_t *)0x1000;
```

执行：

```
p++;
```

`p` 变成多少？
```text
p = 0x1000;
p ++ 代表这个指针向前走一步，步长为8个比特，1个字节长度   p++是0x1001

地址 +1 → 移动 1 byte → 这个 byte 里面有 8 bit
所以针对uint8_t来说，指针加一代表跨过了一个字节的长度，对应地址+1，从0x1000->0x1001
```

---

### 第6题

假设：

```
uint32_t *p = (uint32_t *)0x1000;
```

执行：

```
p++;
```

`p` 变成多少？

为什么不是 `0x1001`？
```text
p = 0x1000;
p指针对应的是uint32_t,即4个字节
p++ 对应为0x1000+4 = 0x1004
```

==`p++` 的步长由指针所指向类型的 `sizeof` 决定==

---

### 第7题

比较：

```
uint8_t *p1;
uint16_t *p2;
uint32_t *p3;
```

问：

```
p1++ 移动多少字节？
p2++ 移动多少字节？
p3++ 移动多少字节？
```

然后总结：

> `p++` 到底由什么决定？
```text
p1++移动一个字节
p2++移动两个字节
p3++移动四个字节
p++是由对应的变量类型决定移动几位的
```

---

## 第三组：数组与指针

### 第8题

```
int arr[3] = {10, 20, 30};
int *p = arr;
```

分别求：

```
*p
*(p + 1)
*(p + 2)
```

```text
*p = 10
*(p+1) = 20
*(p+2) = 30
```
---

### 第9题

还是：

```
int arr[3] = {10, 20, 30};
int *p = arr;
```

执行：

```
p++;
```

现在：

```
*p
```

是多少？

`p` 的地址相比原来增加了多少字节？

```text
p++之后*p是20
p地址增加四个字节
```

---

### 第10题（出错点）

为什么：

```
arr[2]
```

可以写成：

```
*(arr + 2)
```

你不要只记等价关系，要自己解释：

```
arr
→ ?
arr + 2
→ ?
*(arr + 2)
→ ?
```

```text
arr代表这个数组
arr+2 代表
*（arr+2）代表
```

---

## 第四组：字符串

### 第11题

```
char str[] = "ABC";
char *p = str;
```

问：

```
*p
*(p + 1)
*(p + 2)
*(p + 3)
```

分别是什么？

注意最后一个。
```text
*p代表 A
*(p +1)代表B
*(p +2)代表C
*(p +3)代表'\0'
```

---

### 第12题

```
char str[] = "ABC";
char *p = str;

while (*p != '\0')
{
    printf("%c\n", *p);
    p++;
}
```

解释为什么这里：

```
p++;
```

能够一个字符一个字符移动。

不要回答“因为这是字符串”。

真正原因是什么？
```text
因为p指针对应的类型是char，代表一个字节，p++的时候会跨过一个字节的长度，地址+1，而str也是char类型，可以与指针对应    就可以变成单字符移动
```

---

### 第13题

比较：

```
char str[] = "hello";
```

和：

```
char *str = "hello";
```

回答：

1. `str[]` 版本中字符通常存在哪里？
2. 指针版本中的字符串字面量通常存在哪里？
3. 哪一个可以安全执行：

```
str[0] = 'H';
```

```text
str[]中的字符通常存在栈中
指针版本的字符串字面量存在只读区
第一个版本可以安全执行修改操作     指针版本禁止更改内部内容
```
---

## 第五组：你刚刚卡住的 `uint32_t`

### 第14题（大小端）

```
uint32_t x = 0x12345678;
uint8_t *p = (uint8_t *)&x;
```

解释下面每一部分：

```
x
&x
(uint8_t *)&x
p
*p
```

重点回答：

> 为什么一定是 `&x`，而不能写 `(uint8_t *)x`？

```text
x是uint32_t类型的变量    里面存放0x12345678
&x代表的x这个变量的地址
(uint8_t *)&x  代表把x这个变量所在的地址强转成uint8_t的指针类型 
p  里面存放的是x这个变量的地址
*p  = 0x12
```
---

### 第15题（大小端）

在常见小端机器上：

```
uint32_t x = 0x12345678;
uint8_t *p = (uint8_t *)&x;
```

预测：

```
printf("%02X\n", p[0]);
printf("%02X\n", p[1]);
printf("%02X\n", p[2]);
printf("%02X\n", p[3]);
```

输出什么？
```text
12
34
56
78
```

---

### 第16题

还是：

```
uint32_t x = 0x12345678;
uint8_t *p = (uint8_t *)&x;
```

执行：

```
p++;
```

问：

`p` 是移动到了：

A. `x` 的下一个 bit  
B. `x` 的下一个 byte  
C. 下一个 `uint32_t`  
D. 下一块随机内存

并解释原因。

```text
p++移动到了x的下一个byte    因为指针p是uint8_t类型的    步长是一个字节，对应地址+1，而x是uint32_t类型的变量    对应4个地址才能装下
```
---

## 第六组：指针与 bit 的区别

### 第17题

```
uint32_t x = 13;
```

二进制：

```
00000000 00000000 00000000 00001101
```

为什么不能写：

```
uint8_t *p = (uint8_t *)&x;

while (*p)
{
    p++;
}
```

来统计其中有多少个 `1`？

指出至少两个原因。
```text
uint8_t *p对标x的地址，将x的地址强转成指针的类型赋值给p
p++对应的是移动一个字节的长度，一个字节对应8个比特，会把八比特的内容一起判断
`*p` 判断的是整个 byte 是否为0，不能统计里面有几个1。
```

---

### 第18题

下面代码是什么意思：

```
if (x & (1U << 3))
{
    printf("bit3 = 1");
}
```

请逐步解释：

```
1U
↓
1U << 3
↓
形成什么？
↓
x & mask
↓
为什么能判断 bit3？
```

```text
1U代表无符号数的1，就是标准的1
1U<<3代表1左移三位即1000

x & mask代表的是x与掩码进行与操作，掩码是制定在某一位为1，进行与操作之后全1置1  1和0置0，可以判断出来bit3的值
```
---

## 第七组：`void *` 与类型转换

### 第19题

```
uint32_t x = 100;

void *p = &x;
```

为什么不能直接：

```
printf("%u", *p);
```

而通常需要：

```
printf("%u", *(uint32_t *)p);
```

这里：

```
(uint32_t *)p
```

到底是在做什么？
```text
void* 代表的是通用指针，具体执行的时候会不知道对应的步长    所以需要进行强转成别的类型的指针才可以进行正常输出
```

---

## 第八组：综合题

### 第20题（大小端）

认真分析下面代码：

```
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    uint32_t x = 0x12345678;

    uint8_t *p = (uint8_t *)&x;

    printf("%02X\n", *p);

    p++;

    printf("%02X\n", *p);

    *p = 0xAA;

    printf("%08X\n", x);

    return 0;
}
```

假设机器是小端。

你要自己画成：

```
地址        内容

0x1000      ??
0x1001      ??
0x1002      ??
0x1003      ??
```

然后一步一步追踪：

```
p最开始指哪里？
*p是多少？
p++以后指哪里？
*p = 0xAA修改了谁？
x最后是多少？
```

```text
地址            内容

0x08000000      0x78
0x08000001      0x56
0x08000002      0x34
0x08000003      0x12

p最开始指向0x08000000
printf("%02X\n", *p);   0x78
    p++;

    printf("%02X\n", *p);  0x56

    *p = 0xAA;       把56改成了0xAA

    printf("%08X\n", x);    输出0x1234AA78

```

---
# 指针第二轮
## 第1题：数组名能不能移动？

```
int arr[4] = {10, 20, 30, 40};

printf("%d\n", *arr);
```

问：

1. `arr` 在这里代表什么？
2. `*arr` 是多少？
3. 能不能写：

```
arr++;
```

为什么？

```text
arr退化成数组第一个元素对应的地址指针
*arr代表10
不能进行arr++ 因为arr相当于一个常量指针     禁止更改他的位置
```

---

## 第2题：为什么额外定义 `p`

```
int arr[4] = {10, 20, 30, 40};
int *p = arr;

p++;
p++;

printf("%d\n", *p);
printf("%d\n", arr[0]);
```

问：

1. `*p` 是多少？
2. `arr[0]` 是多少？
3. 为什么 `p` 移动了，但是 `arr` 没变？
```text
*p = 30
arr[0] = 10
因为arr是对应数组的起始位置    是禁止进行移动的     只能对p进行移动

```

---

## 第3题：`arr + 1`

假设：

```
int arr[3] = {10, 20, 30};
```

并假设：

```
&arr[0] = 0x1000
sizeof(int) = 4
```

求：

```
arr
arr + 1
arr + 2
```

分别是什么地址？
```text
arr 地址是0x1000
arr + 1 因为int是4个字节的大小，所以指针自增代表偏移了4个字节的大小    对应4个地址  应该为0x1004
arr + 2 对应0x1008
```

---

## 第4题：数组索引本质

```
int arr[4] = {5, 10, 15, 20};
```

分别求：

```
*(arr + 0)
*(arr + 1)
*(arr + 3)
```

然后解释为什么：

```
arr[3]
```

和：

```
*(arr + 3)
```

等价。
```text

*(arr + 0) = 5
*(arr + 1) = 10
*(arr + 3) = 20
arr[3]代表的是数组的第三个元素的值     arr相当于首地址，+3就是偏移3*4 个字节的大小对应的地址 再用*取出来    就是数组的第三个元素的值
```

---

## 第5题：`sizeof`

假设64位机器，`int = 4 byte`：

```
int arr[5];
int *p = arr;
```

预测：

```
sizeof(arr)
sizeof(p)
sizeof(arr[0])
sizeof(*p)
```

分别是多少？

这题非常重要。
```text
sizeof(arr)   arr数组的大小，数组有五个元素，每个元素4字节    总共20个字节
sizeof(p)   p是指针，对应的64位  是8个字节
sizeof(arr[0])   arr数组的第一个元素的大小    元素是int类型   4字节
sizeof(*p)    对应的也是arr数组元素的大小    4字节
```

---

## 第6题：字符串游标

```
char str[] = "HELLO";
char *p = str;

p += 2;

printf("%c\n", *p);
printf("%s\n", str);
```

问：

1. `*p` 输出什么？
2. `str` 输出什么？
3. 为什么移动 `p` 不影响 `str` 的起始位置？

```text
*p = 'L'
str 输出HELLO
str已经赋值给了指针p    p进行移动的时候不会影响str的位置，只有通过p更改字符串的时候才会影响str里面的内容
```

---

## 第7题：修改字符串

```
char str[] = "ABC";
char *p = str;

p++;
*p = 'X';

printf("%s\n", str);
```

输出什么？

画出修改前后：

```
str[0]
str[1]
str[2]
str[3]
```

的内容。
```text
str[0]  A
str[1]  X
str[2]  C
str[3]  '\0'
```

---

## 第8题：小端基础

假设小端机器：

```
uint32_t x = 0xA1B2C3D4;
uint8_t *p = (uint8_t *)&x;
```

预测：

```
p[0]
p[1]
p[2]
p[3]
```

分别是多少？

这一题不要按“人读十六进制数字”的顺序写，要按**内存低地址到高地址**写。
```text
p[0]  D4
p[1]  C3
p[2]  B2
p[3]  A1
```

---

## 第9题：小端 + `p++`

还是：

```
uint32_t x = 0xA1B2C3D4;
uint8_t *p = (uint8_t *)&x;
```

然后：

```
p++;
p++;
```

此时：

```
*p
```

是多少？

`p` 相比最开始地址增加了多少 byte？
```text
*p = B2
p想比最开始地址增加了两个byte
```
---

## 第10题：修改 uint32_t 内部 byte

假设小端：

```
uint32_t x = 0x12345678;
uint8_t *p = (uint8_t *)&x;

p[0] = 0xAA;
```

问：

```
printf("%08X\n", x);
```

输出什么？
```text
0x123456AA
```

---

## 第11题

还是小端：

```
uint32_t x = 0x12345678;
uint8_t *p = (uint8_t *)&x;

p[3] = 0xFF;
```

最后：

```
x = ?
```

```text
x = FF345678
```
---

## 第12题：指针类型改变步长

假设：

```
地址从0x1000开始
```

分别：

```
uint8_t  *p1 = (uint8_t *)0x1000;
uint16_t *p2 = (uint16_t *)0x1000;
uint32_t *p3 = (uint32_t *)0x1000;
```

执行：

```
p1 += 3;
p2 += 3;
p3 += 3;
```

分别变成什么地址？

注意这次不是 `++`，而是 `+= 3`。
```text
p1 += 3;   1000 + 3 =0x1003  
p2 += 3;   1000 + 3*2 = 0x1006
p3 += 3;   1000 + 3*4 = 0x100C
```

---

## 第13题：最容易犯错的一题

```
uint32_t x = 0x12345678;
uint32_t *p = &x;
```

执行：

```
p++;
```

问：

> 此时 `p` 是否还指向 `x` 内部的第二个 byte？

如果不是，它指到哪里去了？
```text
假设x的地址是0x08000000   p的步长应该为4   p = 0x08000000
p ++ = 0x08000004   p指向的已经不是x的内部
```

---

## 第14题：为什么必须换成 `uint8_t *`

```
uint32_t x = 0x12345678;
```

如果你想一个 byte 一个 byte 查看 `x`，为什么：

```
uint32_t *p = &x;
```

不合适，而：

```
uint8_t *p = (uint8_t *)&x;
```

合适？

从下面两个角度解释：

```
*p读取多少byte
p++移动多少byte
```

```text
*p读取1byte
p++移动1byte    对应的就是查看x内的每个字节
```

---

## 第15题：综合

假设小端：

```
uint32_t data[2] = {
    0x12345678,
    0xAABBCCDD
};

uint8_t *p = (uint8_t *)data;
```

假设：

```
data地址 = 0x1000
```

回答：

```
*p
*(p + 1)
*(p + 3)
*(p + 4)
*(p + 7)
```

分别是多少？

并画：

```
地址       内容

0x1000     ??
0x1001     ??
0x1002     ??
0x1003     ??
0x1004     ??
0x1005     ??
0x1006     ??
0x1007     ??
```

如果这15题你能做对 12～13 题，而且隔两三天重新做还能做出来，那么你的“指针 + 内存”这一层就已经开始真正稳定了。

```text
0x1000     78
0x1001     56
0x1002     34
0x1003     12
0x1004     DD
0x1005     CC
0x1006     BB
0x1007     AA

*p         78
*(p + 1)   56
*(p + 3)   12
*(p + 4)   DD
*(p + 7)   AA

```

# 指针第三轮

## 第一部分：基础指针运算

## 第1题：`p++` 到底改变谁？

```
int a[] = {10, 20, 30, 40};

int *p = a;

printf("%d\n", *p);

p++;

printf("%d\n", *p);
printf("%d\n", *a);
```

问：

1. 第一次打印什么？
2. 第二次打印什么？
3. 第三次打印什么？
4. `p++` 有没有改变数组 `a`？
5. `p++` 真正改变的是什么？
```text
printf("%d\n", *p);    10

p++;

printf("%d\n", *p);   20
printf("%d\n", *a);   10
p++没有改变数组a
p++改变的只是本身指针指向的地址
```

---

## 第2题：`*p++`

```
int a[] = {5, 10, 15};

int *p = a;

printf("%d\n", *p++);
printf("%d\n", *p);
```

问：

1. 第一行输出什么？
2. 第二行输出什么？
3. `*p++` 等价于：

```
(*p)++;
```

还是：

```
*(p++);
```

4. 为什么？
```text
printf("%d\n", *p++);    5
printf("%d\n", *p);   10
`*p++`等价于*(p++)
```

---

## 第3题：`(*p)++`（❌）

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

*p++是*(p++)但是是先取出地址中的内容，指针再自增
(*p)++   取出地址中的内容，再把对应的内容加1
```

---

## 第二部分：数组和指针

### 第4题：数组下标本质（❌）

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
dizhi                 
0x1000     0x0B    
0x1001     0x00
0x1002     0x00
0x1003     0x00

dizhi
0x1004     0x16
0x1005     0x00
0x1006     0x00
0x1007     0x00

printf("%d\n", p[2]);   33
printf("%d\n", *(p + 2));    1,441,792
printf("%d\n", a[2]);    33
```

---

### 第5题：这个能不能改？

```
int a[] = {1, 2, 3};

int *p = a;

p++;
a++;
```

问：

1. `p++` 合法吗？
2. `a++` 合法吗？
3. 为什么一个可以，一个不可以？

这里不要只回答：

> 数组不能++

我要你从**“变量里到底存了什么”**的角度解释。

```text
p++ 合法，因为p就是个指针，里面存放的是a的第一个元素的地址，所以可以自增
a++不可以自增    因为a的本质还是数组名只是可以退化成a第一个元素的地址，所以还是不可以进行自增
```

---

## 第三部分：函数与指针

### 第6题：为什么函数能改外面的变量？

```
void change(int *p)
{
    *p = 100;
}

int main()
{
    int x = 10;

    change(&x);

    printf("%d\n", x);
}
```

问：

1. 输出什么？
2. `change()` 得到的是 `x` 本身吗？
3. `p` 里面保存的是什么？
4. `*p = 100` 为什么可以改变 `x`？

```text
输出100
不是x本身，而是x对应的地址
p里面保存的是形参地址对应的内容为100
通过指针可以改变变量值，因为是直接进入到地址里面去改写
```

---

### 第7题：为什么这个改不了指针？

```
void change(int *p)
{
    p = NULL;
}

int main()
{
    int x = 10;
    int *p = &x;

    change(p);

    printf("%d\n", *p);
}
```

问：

1. 最终打印什么？
2. `main()` 里的 `p` 会不会变成 `NULL`？
3. 为什么？

这道题非常重要。
```text

```

---

### 第8题：二级指针

如果我真的想让函数把：

```
p = NULL;
```

修改成功：

```
void change(???)
{
    ???
}

int main()
{
    int x = 10;
    int *p = &x;

    change(???);
}
```

请你自己补完整。

目标是执行完：

```
change(...);
```

之后：

```
p == NULL
```

成立。

---

## 第四部分：字符串指针

### 第9题：两种字符串定义

比较：

```
char *p = "hello";
```

和：

```
char a[] = "hello";
```

问：

1. `"hello"` 分别存在哪里？
2. `p` 本身存在哪里？
3. `a` 的字符一般存在哪里？
4. 下面哪个可以正常执行？

```
p[0] = 'H';
```

```
a[0] = 'H';
```

5. 为什么？

```text
char *p = "hello"; 存在只读区
char a[] = "hello";  存在ram中
p本身存在栈中
a的字符一般存在ram中
a[0] = 'H';可以正常执行
```

---

## 第10题：字符串指针移动

```
char str[] = "Hello";

char *p = str;

while (*p != '\0')
{
    p++;
}

printf("%d\n", *p);
```

问：

1. 循环结束以后 `p` 指向哪里？
2. `*p` 是什么？
3. 为什么打印整数时通常会看到 `0`？
4. 此时：

```
p == str
```

成立吗？
```text
p指向'\0'
*p是'\0'
因为字符串的最后是以'\0'结尾的    所以打印整数的时候可以看到0
p == str不成立    p已经指向了字符串的结尾    str还在字符串的开头
```

---

## 第五部分：const 指针

### 第11题：三个 `const`

分别解释：

```
const int *p;
```

```
int * const p = &x;
```

```
const int * const p = &x;
```

分别回答：

- 能不能修改 `p` 指向的位置？
- 能不能通过 `p` 修改指向的数据？

可以整理成：

|定义|`p++`|`*p = 10`|
|---|---|---|
|`const int *p`|?|?|
|`int * const p`|?|?|
|`const int * const p`|?|?|

---

## 第六部分：指针数组

### 第12题

```
char a[] = "ABC";
char b[] = "DEF";

char *message[] = {a, b};

printf("%c\n", message[0][0]);
printf("%c\n", message[1][1]);
```

问：

1. `message` 是什么？
2. `message[0]` 是什么？
3. `message[0]` 保存的是 `a[0]` 的字符 `'A'`，还是 `a[0]` 的地址？
4. 第一行输出什么？
5. 第二行输出什么？

---

## 第七部分：综合题

### 第13题：这一串代码发生了什么？

```
int a[] = {10, 20, 30};

int *p = a;
int **pp = &p;

(*pp)++;

printf("%d\n", *p);
printf("%d\n", **pp);
printf("%d\n", a[0]);
```

给出三个输出，并解释：

```
(*pp)++;
```

到底修改的是谁。

---

### 第14题：注意运算符优先级

```
int a[] = {10, 20, 30};

int *p = a;

int x = *p++;
int y = ++*p;

printf("%d %d %d %d\n",
       x,
       y,
       a[0],
       a[1]);
```

问最终输出是什么。

这道题必须把每一步写出来。

---

### 第15题：找 Bug

下面代码想复制字符串：

```
void copy(char *dest, const char *src)
{
    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }
}

int main()
{
    char a[20];

    copy(a, "hello");

    printf("%s\n", a);
}
```

代码有什么问题？

应该加什么？

为什么？

---

## 最后一题：真正检测你有没有理解指针

不要运行代码，直接回答：

```
int x = 10;
int *p = &x;
int **pp = &p;
```

此时下面这些分别是什么：

```
x
&x

p
&p
*p

pp
&pp
*pp
**pp
```

你不用给我具体十六进制地址。

只需要写成这种形式：

> `p` = x 的地址  
> `*p` = x 的值  
> `&p` = p 自己的地址

把剩下的全部写出来。

# 指针第四轮
## 第 1 题：最基础的取地址与解引用

```
int a = 10;
int *p = &a;

*p = 30;

printf("%d\n", a);
printf("%d\n", *p);
```

回答：

1. `p` 保存的是什么？
2. `*p` 表示什么？
3. 最后 `a` 是多少？
4. 最后 `*p` 是多少？

```text
p保存的是a的地址
*p表示的是a的地址对应的内容
最后的a 是30
最后的*p是30
```

---

## 第 2 题：修改指针本身

```
int a = 10;
int b = 20;

int *p = &a;

p = &b;
*p = 50;
```

最后：

```
a = ?
b = ?
*p = ?
```

并回答：

```
p = &b;
```

修改的是谁？

而：

```
*p = 50;
```

修改的又是谁？
```text
a = 10
b = 50
*p = 50
p =&b  值得是p里面存放的是b的地址
*p修改的是b的值   因为p里面已经是b的地址了
```

---

## 第 3 题：`p++` 到底修改什么

```
int a = 10;
int *p = &a;

p++;
```

回答：

1. `a` 有没有变化？
2. `p` 有没有变化？
3. `p++` 修改的是地址还是地址中的数据？
4. 此时再执行 `printf("%d", *p);` 是否安全？为什么？

```text
a没有变化
p有变化     p对应的地址是a的地址+4
p++ 修改的是地址
执行printf("%d", *p)不安全    因为指针不知道a地址+4   指向的是什么
```

---

## 第二部分：指针和函数

## 第 4 题：为什么普通参数不能修改外部变量

```
void change(int x)
{
    x = 100;
}

int main()
{
    int a = 10;

    change(a);

    printf("%d\n", a);
}
```

输出：

```
?
```

解释为什么。
```text
输出是10
因为change把a传进去    只是把a的值复制了一份发到函数内部     并不是把a的地址传进去    所以最后不会影响外部的a
```
---

## 第 5 题：为什么指针可以修改外部变量

```
void change(int *p)
{
    *p = 100;
}

int main()
{
    int a = 10;

    change(&a);

    printf("%d\n", a);
}
```

回答：

1. 输出多少？
2. `change(&a)` 传进去的是什么？
3. `p` 保存的是什么？
4. `*p = 100` 为什么能够修改 `a`？
```text
输出是100
change（&a）传进去的是a的地址
p保存的就是a对应的地址
*p = 100是直接通过地址进行修改的可以直接影响到外部变量
```
---

## 第 6 题：函数里修改指针本身（❗）

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
---

## 第三部分：二级指针

## 第 7 题：二级指针最基础

```
int a = 10;

int *p = &a;
int **pp = &p;
```

填写：

```
a   = ?
*p  = ?
**pp = ?

p   保存的是谁的地址？
pp  保存的是谁的地址？
*pp 是什么？
**pp 是什么？
```
```text
a = 10
*p = 10
**pp = 10
p保存的是a的地址
pp保存的是指针p的地址
*pp   是p
**pp 是*p即a
```
---

## 第 8 题：通过二级指针修改数据

```
int a = 10;

int *p = &a;
int **pp = &p;

**pp = 50;
```

最后：

```
a = ?
*p = ?
**pp = ?
```

解释：

```
**pp = 50;
```

为什么实际上修改的是 `a`？
```text
a = 50
*p = 50
**pp = 50
因为**pp实际表示的就是a的内容    *pp就表示p=&a  **pp就是a的值
```
---

## 第 9 题：二级指针真正修改一级指针

```
int a = 10;
int b = 20;

int *p = &a;
int **pp = &p;

*pp = &b;
```

最后：

```
*p = ?
**pp = ?
a = ?
b = ?
```

重点回答：

```
*pp = &b;
```

修改的是：

A. `a`

B. `b`

C. `p` 中保存的地址

D. `pp` 中保存的地址
```text
*p = 20
**pp = 20
a = 10
b = 20
*pp = &b; 修改的是p中保存的地址
```
---

## 第 10 题：为什么需要二级指针

```
void change(int **pp, int *new_addr)
{
    *pp = new_addr;
}

int main()
{
    int a = 10;
    int b = 20;

    int *p = &a;

    change(&p, &b);

    printf("%d\n", *p);
}
```

回答：

1. 最后输出什么？
2. `&p` 是什么类型？
3. `pp` 是什么类型？
4. `*pp` 对应 main 中的谁？
5. 为什么这里必须用 `int **pp`，不能只用 `int *p`？
```text
最后输出*p = 20
&p对应的是p的地址    二级指针
pp对应的是指针p的地址
*pp对应main中的a的地址
必须使用**pp     想改变谁就要把谁的地址传入到函数中     这里想改变的是一级指针保存的地址  所以要把一级指针的地址传入函数中     就是二级指针
```
---

## 第四部分：特别容易混淆的 `*` 和 `++`

## 第 11 题

```
int a = 10;
int *p = &a;

(*p)++;
```

最后：

```
a = ?
*p = ?
```

这里移动指针了吗？
```text
a = 11
*p = 11
这里没有移动指针     是先对指针取值   再对取得值自增一
```
---

## 第 12 题

假设：

```
int data[] = {10, 20, 30};

int *p = data;

printf("%d\n", *p++);
printf("%d\n", *p);
```

回答：

第一次打印：

```
?
```

第二次打印：

```
?
```

并解释：

```
*p++
```

实际上等价于：

```
*(p++)
```

还是：

```
(*p)++
```

> 这里虽然用了普通数组作为数据，但**不涉及数组指针类型**。
```text
第一次打印10
第二次打印20
*p++  实际上等价于：*(p++)   这个表示的是先对指针取值   之后再对指针自增
```
---

## 第 13 题：再区分一次

```
int a = 10;
int *p = &a;

int x = (*p)++;
```

最后：

```
x = ?
a = ?
*p = ?
```

为什么 `x` 和最后的 `*p` 不一样？
```text
x = 10
a = 11
*p = 11
int x =（*p）++   执行的操作是先对指针p取值   赋值给x   最后再把之个值自增1
```
---

## 第五部分：`const` 指针

## 第 14 题

```
int a = 10;
int b = 20;

const int *p = &a;
```

下面哪些合法？

```
*p = 50;
p = &b;
```

分别解释为什么。
```text
第二个合法
const在*的左边    限制的是内容     不允许更改指针指向的内容   但是可以更改指针指向的地址
```
---

## 第 15 题

```
int a = 10;
int b = 20;

int *const p = &a;
```

下面哪些合法？

```
*p = 50;
p = &b;
```

并总结：

```
const int *p
```

到底是谁不能改？

```
int *const p
```

到底是谁不能改？
```text
第一个合法
因为const在*的右侧    限制的是地址     即不允许更改指针指向的地址    但是可以更改对应的内容
```
---

## 第六部分：结构体指针

## 第 16 题

```
struct Student
{
    int age;
    int score;
};

struct Student s = {20, 90};

struct Student *p = &s;
```

下面两个表达式分别是多少？

```
p->age = ?
p->score = ?
```

以及：

```
p->age
```

等价于哪个表达式？

A.

```
*p.age
```

B.

```
(*p).age
```

C.

```
*(p.age)
```

```text
p->age = 20 
p->score = 90
等价于B
```
---

## 第 17 题：结构体内部再放指针

```
struct A
{
    int value;
};

struct B
{
    struct A *ptr;
};

struct A a = {100};
struct B b;

b.ptr = &a;
```

回答：

```
b.ptr->value
```

是多少？

然后执行：

```
b.ptr->value = 200;
```

此时：

```
a.value = ?
```

重点解释：

```
b.ptr = &a;
```

到底是在干什么。
```text
b.ptr->value是100

执行b.ptr->value = 200;后a.value = 200
因为struct B中是嵌套的结构体指针    所以赋值的时候需要把结构体的地址传给b.ptr，同时在修改b.ptr->value的时候   因为直接是通过地址更改的所以a.value的值也会发生变化
```
---

## 第七部分：动态内存和二级指针

## 第 18 题

```
void create(int **pp)
{
    *pp = malloc(sizeof(int));

    **pp = 100;
}

int main()
{
    int *p = NULL;

    create(&p);

    printf("%d\n", *p);

    free(p);
}
```

回答：

1. `p` 最开始是什么？
2. 为什么传入 `&p`？
3. `*pp = malloc(...)` 修改的是谁？
4. `**pp = 100` 修改的是什么？
5. 最后输出什么？

这一题非常重要。
```text
p最开始是空指针
传入&p是想改变p指向的地址内容
*pp = malloc(...)修改的p的内容     即p指向的地址
**pp = 100 修改的是p指向地址的内容
最后输出的是100
```
---

## 第八部分：找 Bug

## 第 19 题

下面代码有什么问题？

```
void create(int *p)
{
    p = malloc(sizeof(int));

    *p = 100;
}

int main()
{
    int *p = NULL;

    create(p);

    printf("%d\n", *p);
}
```

不要只说「会报错」。

你需要解释：

```
p = malloc(...)
```

到底修改的是：

- `main()` 的 `p`
- 还是 `create()` 自己的局部变量 `p`

然后说明应该怎么改。
```text
函数设置应该传入参数是二级指针
因为这个函数想要更改的是一级指针保存的地址    所以需要引入二级指针进行修改

p = malloc(...)修改的create() 自己的局部变量 p
```
---

## 第 20 题：终极关系题

```
int a = 10;
int b = 20;

int *p = &a;
int **pp = &p;

**pp = 30;
*pp = &b;
**pp = 40;
```

不要运行代码，直接推：

最终：

```
a = ?
b = ?
*p = ?
**pp = ?
```

并逐句解释：

```
**pp = 30;
```

改的是谁？

```
*pp = &b;
```

改的是谁？

```
**pp = 40;
```

又改的是谁？

```text
a = 30
b = 40
*p = 40
**pp = 40
**pp = 30;修改的是a的值
*pp = &b;修改的p的值
**pp = 40;修改的是b的值
```
# 二级指针
## 第 1 题：指针到底存什么

```
int a = 10;

int *p = &a;

printf("%d\n", a);
printf("%d\n", *p);
```

回答：

1. `p` 里面存的是什么？
2. `*p` 是什么？
3. 如果执行：

```
*p = 20;
```

那么：

```
a = ?
*p = ?
```

```text
p里面存的是a的地址
*p 是a地址对应的数据
*p = 20   a = 20
```
---

## 第 2 题：修改指针还是修改变量

```
int a = 10;
int b = 20;

int *p = &a;

p = &b;
```

问：

现在：

```
*p = ?
a = ?
b = ?
```

以及最重要的一问：

```
p = &b;
```

到底修改的是：

A. `a`

B. `b`

C. 指针 `p` 自己保存的地址

```text
*p现在是b的值   20
a = 10
b = 20
p = &b;修改的是p自己保存的地址
```

---

## 第 3 题：`p++`

```
int arr[] = {10, 20, 30, 40};

int *p = arr;

p++;

printf("%d\n", *p);
```

问：

1. `*p` 输出多少？
2. 假设最开始 `p = 0x1000`，`sizeof(int)=4`，那么 `p++` 后地址是多少？
3. 为什么不是 `0x1001`？

```text
*p 输出是20
p++后地址是0x1004
因为p是int类型的指针，对应的步长是sizeof(int)=4   所以p进行自增加4
```

---

## 第 4 题：这三个到底有什么区别

```
int arr[] = {10, 20, 30};

int *p = arr;
```

分别判断执行下面三句之后：

 A`p++;`

 B`(*p)++;`

C`*p++;`

回答每一种：

1. 指针 `p` 有没有移动？
2. `arr[0]` 有没有变化？
3. 如果之后执行：

```
printf("%d\n", *p);
```

分别输出多少？

这题是你现阶段**非常重要的一题**。
```text
A是对地址进行自增，  输出20     指针p进行了移动   arr[0]没有改变
B是先取值    取值之后再自增    输出11    指针p没有进行移动     arr[0]进行了改变
C是先取值    再对指针进行自增  输出20  指针p进行了移动     parr[0]没有改变
```

---

## 第 5 题：数组名和指针

```
int arr[] = {1, 2, 3};

int *p = arr;
```

判断下面代码是否合法：

```
p++;
```

```
arr++;
```

并解释：

为什么：

```
p++;
```

可以，

但是：

```
arr++;
```

不可以？
```text
p++是合法的     arr++是非法的     因为p就是指针，是可以对指针进行自增操作的，而arr只是可以退化成指向第一个元素的地址    但本身依然是代表数组的名字的     是不可以自增更改地址的
```

---

## 第 6 题：`p[i]`

```
int arr[] = {10, 20, 30, 40};

int *p = arr;
```

回答：

```
p[2]
```

是多少？

以及：

```
p[2]
```

本质上等价于下面哪个？

A.`*(p + 2)`

B.`*p + 2`

C.`*(p) + sizeof(int) * 2`
```text
p[2] = 30
本质上等价于A    *(p + 2)
```
---

## 第 7 题：函数为什么能修改外面的变量

```
void change(int *p)
{
    *p = 100;
}

int main()
{
    int a = 10;

    change(&a);

    printf("%d\n", a);
}
```

问：

1. 输出什么？
2. `change()` 函数有没有直接拿到 `a` 这个变量？
3. 函数真正收到的是什么？
4. 为什么函数结束以后，`a` 依然被修改了？

```text
输出是100
函数并没有直接拿到a这个变量     
拿到的是a的地址
因为函数的这个操作是通过地址去更改对应的值的     直接从内存的角度去更改数据了
```
---

## 第 8 题：这一版为什么改不了

```
void change(int x)
{
    x = 100;
}

int main()
{
    int a = 10;

    change(a);

    printf("%d\n", a);
}
```

问：

输出：

```
?
```

然后解释为什么：

```
change(a);
```

和：

```
change(&a);
```

效果完全不同。
```text
输出是10
因为change(a);  收到的就只是a这个数据    只是把a的值10  传递进函数x了     不会改变外部的a
```

---

## 第 9 题：函数内部重新定义一个指针

这是你之前经常问的问题。

```
void print_array(int *arr, int n)
{
    int *p = arr;

    while (p < arr + n)
    {
        printf("%d ", *p);
        p++;
    }
}
```

问：

为什么这里不直接：

```
arr++;
```

而是：

```
int *p = arr;
```

然后移动 `p`？

从：

- 是否必须
- 可读性
- 保留原始地址

三个角度回答。
```text
因为如果直接使用arr++  进行操作的话    这个函数执行完成之后会把arr保存的地址指向数组的最后一位  额外定义 p 不是为了保护外面的数组，而是为了在函数内部保留原始地址    所以在函数内部还要再设一个指针，去保存原本数组的初始地址
```

---

## 第 10 题：字符串指针

```
char *p = "hello";
```

回答：

1. `p` 保存的是什么？
2. `"hello"` 通常存在哪里？
3. 能不能：

```
p++;
```

4. 能不能：

```
*p = 'H';
```

5. 为什么这两个问题答案不一样？
```text
p保存的是hello这个字符串对应的第一个元素的地址
hello通常保存在只读取   ROM  FLASH中
可以进行p++   但是不能*p = 'H'   因为这个字符串实际上还在只读取    不可以进行更改
```

---

## 第 11 题：字符串数组

对比：

```
char *p = "hello";
```

和：

```
char str[] = "hello";
```

判断：

 第一种`p[0] = 'H';`是否合法？

第二种`str[0] = 'H';`是否合法？

然后解释：

虽然：

```
*p
```

和：

```
str[0]
```

都可以得到 `'h'`，

为什么它们的内存性质完全不同？
```text
第一种的不合法
第二种合法
因为第一种的字符串是保存在只读区的     是不可以通过指针进行修改
而第二种指针指向的字符串是保存在ram中  是可以通过指针的方式对字符串进行更改
```

---

## 第 12 题：const 指针内容

```
const char *p = "hello";
```

下面哪些可以？

A

```
p++;
```

B

```
*p = 'H';
```

C

```
printf("%c\n", *p);
```

重点解释：

```
const char *p;
```

这个 `const` 到底限制的是：

**指针 p**

还是

**p 指向的数据**？
```text
const char *p;   const在*的左边   是对内容进行限制    可以对指向的地址进行更改   但是不能对地址对应的内容进行更改  所以可以进行A和C但是不能进行B
```

---

## 第 13 题：结构体指针

```
struct Student
{
    int age;
    int score;
};

struct Student s = {20, 90};

struct Student *p = &s;
```

回答：

下面哪些写法正确？

```
p.age
```

```
p->age
```

```
(*p).age
```

以及：

```
p->age
```

为什么实际上就是：

```
(*p).age
```

的简写？
```text
p->age和(*p).age是正确的     因为p已经是指针了，想要获取内部的内容需要使用->，或者使用(*p).age这种表达方式
```

---

## 第 14 题：找 Bug

下面代码想把数组全部加 1：

```
void add_one(int *arr, int n)
{
    int *p = arr;

    for (int i = 0; i < n; i++)
    {
        p++;
        (*p)++;
    }
}
```

主函数：

```
int arr[] = {1, 2, 3, 4};

add_one(arr, 4);
```

目标：

```
2 3 4 5
```

问：

代码有什么问题？

应该怎么修改？
```text
在add_one的函数中先指针自增，再让对应的内容自增   会跳过元素    并且发生数组越界
应该是
    int *p = arr;

    for (int i = 0; i < n; i++)
    {
        (*p)++;
        p++;
    }
```

---

## 第 15 题：指针版删除元素

不给你答案，自己写。

实现：

```
int remove_value(int *arr, int n, int value);
```

例如：

```
int arr[] = {1, 2, 3, 2, 4};
```

删除：

```
value = 2;
```

要求：

最终有效数据变成：

```
1 3 4
```

返回：

```
3
```

但这一次要求你**不要写**：

```
arr[read]
arr[write]
```

尽量使用：

```
int *read;
int *write;
```

完成。
```c
int remove_value(int *arr, int n, int value)
{
	if(arr == NULL || n <= 0)
	{
		return 0;
	}
	
	int* read = arr;
	int* write = arr;
	
	for(int i = 0; i < n; i++)
	{
		if(*read != value)
		{
			*write = *read;
			write ++;
		}
		read ++;
	}
	
	return write - arr;
}

```

---

## 第 16 题：内存函数思维

自己实现：

```
void my_memset(void *dest, int value, size_t n);
```

要求：

把 `dest` 开始的连续 `n` 个**字节**设置成 `value`。

你需要自己思考：

```
void *
```

为什么不能直接：

```
*p
p++
```

应该先转换为什么类型？

提示只有一个：

> 一次处理一个 byte。

```c
//因为一次处理一个byte   所以应该设置int类型或者uint8_t的类型
void my_memset(void *dest, int value, size_t n)
{
	if(dest == NULL || n == 0)
	{
		return 0;
	}
	
	uint8_t* p = (uint8_t *)dest;
	
	for (int i = 0; i < n; i++)
	{
		*p++ = value;
	}
}

```

# 指针专项练习（二）


## 第一组：先练 `pp / *pp / **pp`

### 第 1 题

```
int a = 10;
int *p = &a;
int **pp = &p;
```

回答：

```
1. p 存的是什么？
2. *p 是什么？
3. pp 存的是什么？
4. *pp 是什么？
5. **pp 是什么？
```

```text
1. p 存的是 a的地址
2. *p 是a地址对应的内容    10
3. pp 存的是p的地址
4. *pp 是p的地址对应的内容   即a的地址
5. **pp 是a的值   即10
```

---

### 第 2 题

```
int a = 10;
int *p = &a;
int **pp = &p;

**pp = 50;
```

执行之后：

```
a = ?
*p = ?
**pp = ?
```

并解释：

```
**pp = 50;
```

到底修改的是谁？

```text
a = 50
*p = 50
**pp = 50
**pp 直接修改了a变量地址对应的内容     把10修改成了50
```

---

### 第 3 题

```
int a = 10;
int b = 20;

int *p = &a;
int **pp = &p;

*pp = &b;
```

执行之后：

```
p 指向谁？
*p = ?
a = ?
b = ?
```

最重要的一问：

```
*pp = &b;
```

修改的是：

A. `a`

B. `b`

C. `p` 自己保存的地址

D. `pp` 自己保存的地址

```text
*pp = p = &b 
p指向了b的地址
*p = 20
a = 10
b = 20
```

---

## 第二组：区分 `*pp` 和 `**pp`

### 第 4 题

```
int a = 10;
int b = 20;

int *p = &a;
int **pp = &p;

**pp = 30;
*pp = &b;
```

最后：

```
a = ?
b = ?
p 指向谁？
*p = ?
**pp = ?
```

注意一定要按顺序执行。

```text

```

---

### 第 5 题

```
int a = 5;
int b = 8;

int *p = &a;
int **pp = &p;

*pp = &b;
**pp = 100;
```

最后：

```
a = ?
b = ?
*p = ?
```

并解释为什么 `100` 最后改的是这个变量。

---

## 第三组：函数为什么需要二级指针

### 第 6 题

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

问：

```
最终打印多少？
```

以及：

```
函数里的 p = &b;
为什么没有修改 main 里的 p？
```

先只考虑指针传递逻辑，不用纠结 `b` 生命周期。

---

### 第 7 题

```
void change(int **pp, int *new_p)
{
    *pp = new_p;
}

int main()
{
    int a = 10;
    int b = 20;

    int *p = &a;

    change(&p, &b);

    printf("%d\n", *p);
}
```

问：

```
1. &p 的类型是什么？
2. pp 的类型是什么？
3. *pp 代表谁？
4. 执行 *pp = new_p 后，main 中的 p 指向谁？
5. 最终打印多少？
```

---

### 第 8 题

```
void change_value(int **pp)
{
    **pp = 100;
}

int main()
{
    int a = 10;
    int *p = &a;

    change_value(&p);

    printf("%d\n", a);
}
```

问：

```
最终 a = ?
```

重点解释：

```
**pp = 100;
```

为什么修改的是 `a`，而不是 `p`。

---

## 第四组：稍微综合一点

### 第 9 题

```
int a = 10;
int b = 20;
int c = 30;

int *p = &a;
int **pp = &p;

**pp = 100;
*pp = &b;
**pp = 200;
p = &c;
```

最后：

```
a = ?
b = ?
c = ?

p 指向谁？
*p = ?
**pp = ?
```

这题很适合检查你是否真正区分：

```
*pp
和
**pp
```

---

## 第五组：链表版

先定义：

```
struct Node
{
    int data;
    struct Node *next;
};
```

### 第 10 题

```
struct Node node1 = {10, NULL};
struct Node *head = &node1;
struct Node **ppHead = &head;
```

回答：

```
1. head 的类型是什么？
2. head 里面存什么？
3. *head 是什么？
4. ppHead 的类型是什么？
5. ppHead 里面存什么？
6. *ppHead 是什么？
7. **ppHead 是什么？
8. (*ppHead)->data 是多少？
```

---

### 第 11 题

```
struct Node node1 = {10, NULL};
struct Node node2 = {20, NULL};

struct Node *head = &node1;
struct Node **ppHead = &head;

*ppHead = &node2;
```

最后：

```
head 指向谁？
head->data = ?
node1.data = ?
node2.data = ?
```

并解释：

```
*ppHead = &node2;
```

为什么等价于：

```
head = &node2;
```

---

## 最后一题：真正的头插思维

### 第 12 题

```
struct Node node1 = {10, NULL};
struct Node node2 = {20, NULL};

struct Node *head = &node1;
struct Node **ppHead = &head;

node2.next = *ppHead;
*ppHead = &node2;
```

问最后链表结构是什么：

```
head
 ↓
?
```

并回答：

```
1. node2.next 指向谁？
2. head 最终指向谁？
3. 链表顺序是什么？
```

# 结构体嵌套
## 第 1 题：最基础

```
struct A
{
    int x;
};

struct A a;
```

问：

1. 访问 `x` 应该写什么？
2. 为什么用 `.` 而不是 `->`？
```
访问x应该写   a.x     因为a是结构体类型     并不是结构体指针类型
```

---

## 第 2 题：结构体指针

```
struct A
{
    int x;
};

struct A a;
struct A *p = &a;
```

问：

1. 通过 `p` 访问 `x` 应该写什么？
2. `p.x` 对不对？
3. `p->x` 等价于什么表达式？

```text
p -> x
p.x不正确     因为p已经是结构体指针    p里面装着是结构体a的地址
p -> x等价于(*p).x
```

---

## 第 3 题：结构体嵌套结构体

```
struct Date
{
    int year;
};

struct Student
{
    int age;
    struct Date birthday;
};

struct Student stu;
```

问：

访问：

```
year
```

应该写：

```
?
```

并说明为什么中间用什么符号。
```text
访问year应该写  stu.birthday.year
中间都用.     在student结构体中嵌套的是date这个结构体不是结构体指针
```

---

## 第 4 题：外层变成指针

还是：

```
struct Date
{
    int year;
};

struct Student
{
    int age;
    struct Date birthday;
};

struct Student stu;
struct Student *p = &stu;
```

问：

通过 `p` 修改出生年份为 `2000`，应该写：

```
?
```

你需要解释：

```
p 是什么类型？
p->birthday 是什么类型？
```

```text
通过p修改出生年份应该写     p->birthday.year
p是结构体指针类型
p->birthday是结构体类型
```

---

## 第 5 题：里面变成指针

```
struct Date
{
    int year;
};

struct Student
{
    int age;
    struct Date *birthday;
};

struct Date d;
struct Student stu;

stu.birthday = &d;
```

问：

把 `year` 设置成：

```
2001
```

应该写什么？

A.

```
stu.birthday.year = 2001;
```

B.

```
stu->birthday.year = 2001;
```

C.

```
stu.birthday->year = 2001;
```

D.

```
stu->birthday->year = 2001;
```

同时说出为什么。
`
```text
C    struct Student stu是结构体类型，所以用stu.birthday进行访问，而stu.birthday = &d;  又把birthday定义为了指针的类型   继续访问就是指针类型
```

---

## 第 6 题：外层和内层都是指针

```
struct Date
{
    int year;
};

struct Student
{
    struct Date *birthday;
};

struct Date d;

struct Student stu;
stu.birthday = &d;

struct Student *p = &stu;
```

问：

通过 `p` 将年份修改成：

```
2026
```

应该写什么？

你可以按下面格式分析：

```
p                  类型 = 结构体指针
p->birthday        类型 = 结构体指针
最终表达式          = p->birthday->year = 2026;
```

---

## 第 7 题：嵌入式风格

```
typedef struct
{
    int temperature;
    int humidity;
} SensorData;

typedef struct
{
    SensorData data;
    int status;
} Sensor;

Sensor sensor;
Sensor *p = &sensor;
```

分别写出：

1. 通过 `sensor` 修改 `temperature = 25`
2. 通过 `p` 修改 `temperature = 30`
3. 通过 `p` 修改 `status = 1`

```text
sensor.data.temperature = 25
p -> data.temperature = 30
p -> status = 1
```

---

## 第 8 题：稍微难一点

```
typedef struct
{
    char data[100];
} Buffer;

typedef struct
{
    Buffer *rx_buffer;
} UART;

typedef struct
{
    UART uart;
} Device;

Device dev;
Device *p = &dev;
Buffer buf;

dev.uart.rx_buffer = &buf;
```

问：

通过 `p` 修改：

```
buf.data[0] = 'A';
```

对应的完整表达式应该是什么？

```text
p -> uart.rx_buffer -> data[0] = 'A'
```

不要直接猜。

按层分析：

```
p
↓
?
↓
?
↓
?
↓
data[0]
```

# Callback
## 第 1 题：最基础的函数指针

```
#include <stdio.h>

void hello(void)
{
    printf("Hello\n");
}

int main(void)
{
    void (*p)(void);

    p = hello;

    p();

    return 0;
}
```

回答：

1. `p` 是什么类型？
2. `p = hello;` 存进去的是什么？
3. `p();` 实际执行的是谁？
4. 能不能写成：

```
p = &hello;
```

5. 能不能写成：

```
(*p)();
```

```text
p函数指针的类型
p = hello存进去的是hello的地址
p（）实际执行的是hello函数
可以写成p = &hello
(*p)();也可以正常调用函数指针
```

---

## 第 2 题：带参数的 callback

```c
#include <stdio.h>

void print_num(int x)
{
    printf("%d\n", x);
}

void run(void (*callback)(int), int data)
{
    callback(data);
}

int main(void)
{
    run(print_num, 100);

    return 0;
}
```

回答：

1. `callback` 是什么？
2. `run(print_num, 100)` 中，真正传给 `run()` 的是什么？
3. `callback(data)` 最终相当于执行哪一句？
4. 最终打印什么？

重点解释：

```
void (*callback)(int)
```

每一部分分别代表什么。
```text
callback是定义的函数指针     对应的类型是输入一个int的参数   没有返回值
run(print_num, 100) 中真正传给run的是print_num 和100
callback(data)相当于执行printf("%d\n", data)
最终打印的是100
```

---

## 第 3 题：判断类型是否匹配

下面有三个函数：

```c
void func1(int x)
{
}

int func2(int x)
{
    return x;
}

void func3(void)
{
}
```

现在定义：

```c
void (*callback)(int);
```

问下面哪些正确？

```
callback = func1;
callback = func2;
callback = func3;
```

分别说明原因。
```text
只有callback = func1是正确的
因为定义这个函数指针的时候   就规定了输入参数是一个int类型    没有输出
在三个函数中只有第一个符合
```
---

## 第 4 题：真正的 callback 思维

```c
#include <stdio.h>

void success(void)
{
    printf("Success\n");
}

void error(void)
{
    printf("Error\n");
}

void process(int status, void (*callback)(void))
{
    if (status == 1)
    {
        callback();
    }
}

int main(void)
{
    process(1, success);
    process(0, error);

    return 0;
}
```

问：

1. 第一条 `process()` 会不会调用 `success()`？
2. 第二条会不会调用 `error()`？
3. 最终输出什么？
4. 为什么这里叫 callback，而不只是“调用函数”？
```text
第一条会调用到success   
第二条不会调用到error
最终输出是Success
因为在process中规定了第二个输入参数是一个函数指针的类型     对应的函数没有输入参数也没有输出
```
---

## 第 5 题：两个 callback

补全下面代码：

```c
void on_success(void)
{
    printf("OK\n");
}

void on_error(void)
{
    printf("ERROR\n");
}

void check_value(int value,
                 ___________________,
                 ___________________)
{
    if (value >= 0)
    {
        ___________________;
    }
    else
    {
        ___________________;
    }
}

int main(void)
{
    check_value(10, on_success, on_error);
}
```

要求：

- `value >= 0` 调用 `on_success`
- `value < 0` 调用 `on_error`

你自己填写所有空格。
```text
void check_value(int value,
                 void (*callback1)(void),
                 void (*callback2)(void))
{
    if (value >= 0)
    {
        callback1();
    }
    else
    {
        callback2();
    }
}
```
---

## 第 6 题：callback 注册

这一题开始很像 STM32 工程。

```c
#include <stdio.h>

void (*uart_callback)(void) = NULL;

void UART_RegisterCallback(void (*cb)(void))
{
    uart_callback = cb;
}

void UART_IRQHandler(void)
{
    if (uart_callback != NULL)
    {
        uart_callback();
    }
}

void uart_rx_complete(void)
{
    printf("UART RX complete\n");
}

int main(void)
{
    UART_RegisterCallback(uart_rx_complete);

    UART_IRQHandler();

    return 0;
}
```

回答：

1. `uart_callback` 是什么？
2. 刚开始它里面是什么？
3. 执行：

```
UART_RegisterCallback(uart_rx_complete);
```

以后，`uart_callback` 里面保存什么？

4. `UART_IRQHandler()` 为什么不直接写：

```
uart_rx_complete();
```

而是写：

```
uart_callback();
```

5. 这种写法最大的工程意义是什么？
```text
uart_callback是一个函数指针     对应的是没有输入和输出参数的函数

刚开始里面是NULL代表空指针

执行UART_RegisterCallback(uart_rx_complete);之后uart_callback里面保存的是uart_rx_complete

UART_IRQHandler() 写uart_callback();  是为了进行函数回调    这样未来如果想修改中断中执行的函数就可以直接从UART_RegisterCallback中进行修改   这样可以做到解耦合
```
---

## 第 7 题：为什么必须判断 NULL？

假设改成：

```c
void UART_IRQHandler(void)
{
    uart_callback();
}
```

而程序没有执行：

```c
UART_RegisterCallback(...);
```

那么：

```c
uart_callback == NULL
```

此时直接：

```c
uart_callback();
```

可能发生什么？

A. 什么也不发生  
B. 自动执行默认函数  
C. 尝试跳转到非法地址  
D. 编译器自动忽略

解释原因。
```text
C 会尝试跳转到非法地址   uart_callback 这个函数指针目前没有保存任何有效函数地址。
```
---

## 第 8 题：带数据的 UART callback（❌）

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

void UART_RegisterCallback(void (*cb)(uint8_t)) //注意这里输入参数的定义
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
---

## 第 9 题：typedef callback（

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
---

## 第 10 题：结构体 + callback

这一题非常重要，已经接近驱动层写法。

```c
typedef void (*Callback_t)(uint8_t data);

typedef struct
{
    uint8_t id;
    Callback_t rx_callback;

} UART_Device;
```

然后：

```c
void app_rx(uint8_t data)
{
    printf("%d\n", data);
}

int main(void)
{
    UART_Device uart1;

    uart1.id = 1;
    uart1.rx_callback = app_rx;

    uart1.rx_callback(100);

    return 0;
}
```

回答：

1. `rx_callback` 是普通变量还是函数指针？
2. `uart1.rx_callback = app_rx;` 在干什么？
3. `uart1.rx_callback(100);` 等价于什么？
4. 为什么 callback 经常会放进结构体？
5. 如果有：

```
UART_Device uart1;
UART_Device uart2;
```

是不是可以给它们注册不同的 callback？
```text
rx_callback是普通变量    这里用作函数地址使用
uart1.rx_callback = app_rx;是把app_rx放到结构体中    结构体的第二个元素就变成了app_rx
uart1.rx_callback(100);等价于app_rx（100）
callback经常放进结构体是因为可以通过结构体很方便的进行函数指针赋值
可以注册不同的callback
```
---

## 第 11 题：找 bug

```c
typedef void (*Callback_t)(int);

void register_callback(Callback_t cb);

void my_callback(void)
{
    printf("callback\n");
}

int main(void)
{
    register_callback(my_callback);
}
```

这段代码有没有问题？

如果有，问题在哪里？

应该如何修改？
```text
有问题
指针函数在定义的时候    输入参数是int    输出是void
但在赋值函数的时候    my_Callback输入参数也是void类型的     造成了函数类型不匹配
```
---

## 第 12 题：非常容易混淆的一题

比较：

```c
callback;
```

```c
callback();
```

```c
*callback;
```

```c
(*callback)();
```

假设：

```c
void (*callback)(void);
```

分别解释这四个表达式是什么意思。
```text
callback;  代表的是函数指针的名称
callback();  是调用函数
*callback;    也是在调用函数
(*callback)();  也是调用函数
```
---

## 第 13 题：手写一个 callback 系统()

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
---

## 第 14 题：STM32 风格思考题

假设底层驱动只负责：

```
void UART_IRQHandler(void)
{
    // 获取数据
    // 清除标志位
    // 调 callback
}
```

而应用层决定：

```
void bluetooth_receive(uint8_t data);
```

或者：

```
void gps_receive(uint8_t data);
```

那么我们可以做到：

```
UART_RegisterCallback(bluetooth_receive);
```

或者：

```
UART_RegisterCallback(gps_receive);
```

问：

为什么这种设计比直接在 UART 驱动里面写：

```
bluetooth_receive(data);
```

更好？

重点从这几个词思考：

**解耦、复用、驱动层、应用层。**

```text
使用Callback这种方式    可以做到解耦合    让程序在发生中断的时候可以更加自由的进入不同的执行操作中    而不是每次都去函数的中断中进行修改  
```

# 混合部分(阶段一)
## 第一部分：普通指针基本功

### 第 1 题：地址和值

```
int a = 10;
int *p = &a;

*p = 20;
p = NULL;
```

回答：

1. 执行 `*p = 20` 后，`a` 是多少？
2. `p = NULL` 修改的是 `a` 还是 `p`？
3. 执行 `p = NULL` 后，`a` 是多少？
4. 此时还能执行 `*p = 30` 吗？为什么？

---

### 第 2 题：指针重新指向

```
int a = 10;
int b = 20;

int *p = &a;

*p = 30;
p = &b;
*p = 40;
```

最终：

```
a = ?
b = ?
*p = ?
```

并解释：

```
p = &b;
```

和：

```
*p = b;
```

本质上有什么区别？

---

## 第二部分：你之前容易混的自增

### 第 3 题：`p++` 与 `(*p)++`

```
int arr[] = {10, 20, 30};

int *p = arr;

(*p)++;
p++;

printf("%d\n", *p);
printf("%d\n", arr[0]);
```

问：

1. 最终 `*p` 是多少？
2. `arr[0]` 是多少？
3. `(*p)++` 修改的是谁？
4. `p++` 修改的是谁？

---

### 第 4 题：`*p++`

```
int arr[] = {10, 20, 30};

int *p = arr;

int x = *p++;

printf("%d\n", x);
printf("%d\n", *p);
```

回答：

```
x = ?
*p = ?
```

并把：

```
*p++
```

加括号写成真正的运算顺序。

---

### 第 5 题：三个表达式必须区分

假设：

```
int a = 10;
int *p = &a;
```

解释：

```
p++;
```

```
(*p)++;
```

```
*p++;
```

分别修改什么？

这一题不用运行代码，重点解释语义。

---

## 第三部分：函数与指针参数

### 第 6 题：为什么能修改外部变量？

```
void change(int *p)
{
    *p = 100;
}

int main(void)
{
    int a = 10;

    change(&a);

    printf("%d\n", a);
}
```

回答：

1. `change(&a)` 真正传进去的是什么？
2. `p` 和 `a` 是同一个变量吗？
3. 为什么修改 `*p` 可以修改 `a`？
4. 如果函数里面写：

```
p = NULL;
```

会不会把 `main()` 中的 `a` 删除？

---

### 第 7 题：这个函数为什么修改不了指针？

```
void change_pointer(int *p)
{
    static int b = 100;

    p = &b;
}

int main(void)
{
    int a = 10;
    int *p = &a;

    change_pointer(p);

    printf("%d\n", *p);
}
```

问：

最终打印多少？

重点解释：

> 为什么函数里面明明执行了 `p = &b`，main 中的 `p` 却没有被修改？

---

## 第四部分：二级指针

### 第 8 题：二级指针基本关系

```
int a = 10;

int *p = &a;

int **pp = &p;
```

回答：

```
p 保存什么？

*p 是什么？

pp 保存什么？

*pp 是什么？

**pp 是什么？
```

然后执行：

```
**pp = 50;
```

此时：

```
a = ?
```

---

### 第 9 题：为什么这里需要二级指针？

```
void change_pointer(int **pp)
{
    static int b = 100;

    *pp = &b;
}

int main(void)
{
    int a = 10;
    int *p = &a;

    change_pointer(&p);

    printf("%d\n", *p);
}
```

回答：

1. `&p` 是什么？
2. `pp` 保存什么？
3. `*pp` 对应 main 中的什么？
4. 为什么：

```
*pp = &b;
```

真的能够修改 main 中 `p` 保存的地址？

---

## 第五部分：字符串指针——你之前反复卡过的地方

### 第 10 题：`char *` 与 `char[]`

判断下面两个：

```
char *p = "hello";
```

和：

```
char str[] = "hello";
```

回答：

1. `"hello"` 分别存在哪里？
2. `p` 保存什么？
3. `str` 本身是什么？
4. 哪一个通常可以安全执行：

```
xxx[0] = 'H';
```

5. 为什么？

---

### 第 11 题：函数返回指针

判断：

```
char *func(void)
{
    char str[] = "hello";

    return str;
}
```

有没有问题？

然后判断：

```
char *func(void)
{
    return "hello";
}
```

有没有同样的问题？

重点解释：

> 为什么两段代码虽然都返回 `char *`，生命周期却不一样？

---

## 第六部分：结构体指针

### 第 12 题：`.` 和 `->`

```
typedef struct
{
    int id;
    int value;
} Device;

Device dev = {1, 100};

Device *p = &dev;
```

请分别用：

```
dev
```

和：

```
p
```

把 `value` 修改成 `200`。

然后解释为什么：

```
dev.value
```

使用 `.`，

而：

```
p->value
```

使用 `->`。

---

### 第 13 题：结构体内部的指针

```
typedef struct
{
    int value;
} Data;

typedef struct
{
    int id;
    Data *data;
} Device;

Data d = {100};

Device dev;

dev.id = 1;
```

请补代码，让：

```
dev.data
```

指向：

```
d
```

然后通过：

```
dev
```

把 `d.value` 修改成 `200`。

这一题正好练你之前“结构体里面套结构体指针怎么赋值”的薄弱点。

---

## 第七部分：函数指针——近期重点

### 第 14 题：先区分三个人

```
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

---

### 第 15 题：这一题专门打你最近的混淆

比较：

```
int *p;
```

与：

```
typedef void (*Callback_t)(int);

Callback_t callback;
```

填写对应关系：

```
int *          ↔ ?
p              ↔ ?

p 保存变量地址
callback 保存 ?
```

然后回答：

> `Callback_t` 在这个类比中更接近 `int` 还是 `int *`？

说明原因。

---

## 第八部分：函数指针调用

### 第 16 题

```
void hello(void)
{
    printf("Hello\n");
}

void (*callback)(void) = hello;
```

分别解释：

```
callback;
```

```
*callback;
```

```
callback();
```

```
(*callback)();
```

哪几个**真正调用了 `hello()`**？

这一题你上一次错过，所以这次再做。

---

## 第九部分：callback 注册流程

### 第 17 题

```
typedef void (*Callback_t)(int);

Callback_t callback = NULL;

void register_callback(Callback_t cb)
{
    callback = cb;
}

void print_data(int data)
{
    printf("%d\n", data);
}

int main(void)
{
    register_callback(print_data);

    callback(100);

    return 0;
}
```

不要只说结果。

按照地址流向回答：

```
① print_data 在 register_callback(print_data) 中代表什么？

② cb 里面保存什么？

③ callback = cb 后，callback 里面保存什么？

④ cb 和 callback 此时保存的地址是否相同？

⑤ callback(100) 为什么最终执行 print_data(100)？
```

---

## 第十部分：找 Bug——非常适合你现在

### 第 18 题

找出所有问题：

```
typedef void (*Callback_t)(int);

Callback_t callback = NULL;

void register_callback(Callback_t cb)
{
    callback = cb;
}

void app_callback(void)
{
    printf("Hello\n");
}

int main(void)
{
    register_callback(app_callback);

    callback;

    return 0;
}
```

至少有 **两个问题**。

分别说明：

1. 问题是什么？
2. 为什么？
3. 怎么改？

---

## 第 19 题：自己写 callback

要求实现：

```
sensor_register_callback(...)
```

注册：

```
void sensor_data_ready(int data)
{
    printf("data = %d\n", data);
}
```

然后：

```
sensor_irq_handler()
```

模拟传感器产生数据 `100`。

要求包含：

- `typedef` 定义 callback 类型
- callback 全局变量
- 初始化 `NULL`
- register 函数
- `NULL` 检查
- callback 调用
- callback 带一个 `int` 参数

最终：

```
int main(void)
{
    sensor_register_callback(sensor_data_ready);

    sensor_irq_handler();

    return 0;
}
```

输出：

```
data = 100
```

---

## 第 20 题：综合题

看下面代码：

```
typedef void (*RxCallback_t)(char data);

typedef struct
{
    int id;
    RxCallback_t callback;
} UART_Device;

void bluetooth_rx(char data)
{
    printf("Bluetooth: %c\n", data);
}

void UART_Register(UART_Device *dev, RxCallback_t cb)
{
    dev->callback = cb;
}

void UART_IRQHandler(UART_Device *dev, char data)
{
    if (dev != NULL && dev->callback != NULL)
    {
        dev->callback(data);
    }
}
```

回答：

1. `RxCallback_t` 是什么？
2. `UART_Device *dev` 中 `dev` 保存什么？
3. `dev->callback` 是什么类型？
4. 执行：

```
UART_Register(&uart1, bluetooth_rx);
```

后，`uart1.callback` 保存什么？  
5.：

```
dev->callback(data);
```

为什么可以像函数一样调用？  
6. 这段代码同时用了哪些知识？

从下面选择并解释：

```
普通指针
结构体指针
typedef
函数指针
callback
NULL
二级指针
```

# 链表
## 第一部分：基础概念

### 第 1 题：节点和节点指针

```
typedef struct Node
{
    int data;
    struct Node *next;
} Node;

Node node1;
Node *p = &node1;

node1.data = 10;
node1.next = NULL;
```

回答：

1. `node1` 是什么类型？
2. `p` 是什么类型？
3. `p` 里面保存的是什么？
4. `*p` 代表什么？
5. `p->data` 等于多少？
6. `p->next` 等于什么？

---

### 第 2 题：`->` 的本质

```
Node node1;
Node *p = &node1;

p->data = 20;
```

问：

```
p->data
```

和下面哪个完全等价？

A.

```
p.data
```

B.

```
(*p).data
```

C.

```
*p.data
```

D.

```
&p.data
```

并解释为什么。

---

## 第二部分：节点之间如何连接

### 第 3 题：两个节点

```
Node node1;
Node node2;

node1.data = 10;
node2.data = 20;

node1.next = &node2;
node2.next = NULL;

Node *head = &node1;
```

回答：

```
head->data = ?
head->next = ?
head->next->data = ?
head->next->next = ?
```

并画出：

```
head → ?
```

对应的链表结构。

---

### 第 4 题：三个节点

```
Node a;
Node b;
Node c;

a.data = 1;
b.data = 2;
c.data = 3;

a.next = &b;
b.next = &c;
c.next = NULL;

Node *head = &a;
```

问：

```
head->next
```

保存的是谁的地址？

```
head->next->next
```

保存的是谁的地址？

```
head->next->next->data
```

是多少？

```
head->next->next->next
```

是多少？

---

## 第三部分：链表遍历

### 第 5 题：工作指针

链表：

```
head
 ↓
10 → 20 → 30 → NULL
```

代码：

```
Node *p = head;

p = p->next;
```

执行之后：

```
head 指向谁？
p 指向谁？
```

A.

```
head → 10
p    → 20
```

B.

```
head → 20
p    → 20
```

C.

```
head → 10
p    → 10
```

解释为什么。

---

### 第 6 题：继续移动

```
Node *p = head;

p = p->next;
p = p->next;
```

原链表：

```
10 → 20 → 30 → NULL
```

现在：

```
p->data = ?
p->next = ?
```

---

### 第 7 题：遍历代码

```
Node *p = head;

while (p != NULL)
{
    printf("%d ", p->data);
    p = p->next;
}
```

链表：

```
5 → 8 → 12 → NULL
```

问：

1. 输出是什么？
2. 循环结束后 `p` 等于什么？
3. 循环结束后 `head` 发生变化了吗？
4. 为什么一般不直接用：

```
head = head->next;
```

来遍历？

---

## 第四部分：NULL 判断

### 第 8 题：哪里会出错？

```
Node *head = NULL;

printf("%d\n", head->data);
```

会发生什么？

A. 输出0

B. 输出随机数

C. 非法访问内存，可能程序崩溃

D. 编译器自动跳过

解释原因。

---

### 第 9 题：最后一个节点

链表：

```
10 → 20 → 30 → NULL
```

代码：

```
Node *p = head;

while (p->next != NULL)
{
    p = p->next;
}
```

循环结束后：

```
p 指向谁？
p->data = ?
p->next = ?
```

再回答：

为什么这里使用：

```
p->next != NULL
```

而不是：

```
p != NULL
```

---

## 第五部分：头插法

### 第 10 题：手推头插

原链表：

```
head
 ↓
10 → 20 → NULL
```

现在有：

```
new_node
 ↓
5 → NULL
```

执行：

```
new_node->next = head;
```

此时链表关系是什么？

然后执行：

```
head = new_node;
```

最终链表是什么？

请分别画出**两步之后的结构**。

---

### 第 11 题：顺序能不能反？

正确头插是：

```
new_node->next = head;
head = new_node;
```

如果写成：

```
head = new_node;
new_node->next = head;
```

会出现什么问题？

提示：

第二句执行时：

```
head == new_node
```

那么：

```
new_node->next = ?
```

最后会形成什么结构？

---

## 第六部分：函数和 head

### 第 12 题：为什么外面的 head 没变？

```
void test(Node *head)
{
    head = head->next;
}
```

主函数：

```
Node *head = &node1;

test(head);
```

假设：

```
node1 → node2 → NULL
```

执行完以后，主函数里的：

```
head
```

指向谁？

A. node1

B. node2

C. NULL

并解释：

> 为什么函数内部的 `head = head->next` 没有修改主函数里的 `head`？

---

### 第 13 题：返回新 head

```
Node *delete_head(Node *head)
{
    if (head == NULL)
    {
        return NULL;
    }

    Node *temp = head;

    head = head->next;

    free(temp);

    return head;
}
```

原链表：

```
10 → 20 → 30 → NULL
```

调用：

```
head = delete_head(head);
```

回答：

1. `temp` 最开始指向谁？
2. `head = head->next` 后 `head` 指向谁？
3. `free(temp)` 释放谁？
4. 最终链表是什么？
5. 为什么必须写：

```
head = delete_head(head);
```

而不能只写：

```
delete_head(head);
```

---

## 第七部分：malloc

### 第 14 题：创建节点

```
Node *new_node;

new_node = malloc(sizeof(Node));

new_node->data = 100;
new_node->next = NULL;
```

问：

1. `new_node` 本身在哪里存？
2. `new_node` 里面保存什么？
3. `malloc(sizeof(Node))` 创建的是什么？
4. `new_node->data` 修改的是指针本身，还是堆内存里的数据？

---

### 第 15 题：判断 malloc 失败

下面代码有什么问题？

```
Node *new_node = malloc(sizeof(Node));

new_node->data = 10;
new_node->next = NULL;
```

为什么更规范应该写：

```
Node *new_node = malloc(sizeof(Node));

if (new_node == NULL)
{
    return NULL;
}
```

---

## 第八部分：内存泄漏

### 第 16 题

```
Node *p = malloc(sizeof(Node));

p->data = 10;

p = NULL;
```

问：

这有没有内存泄漏？

如果有，原因是什么？

重点解释：

```
p = NULL
```

是不是等于：

```
free(p)
```

？

---

## 第九部分：free

### 第 17 题：释放节点

```
Node *p = malloc(sizeof(Node));

p->data = 10;

free(p);

printf("%d\n", p->data);
```

有什么问题？

A. 没问题

B. `p` 自动变成NULL

C. use-after-free

D. 编译错误

并解释：

> `free(p)` 之后，`p` 这个指针变量本身还存在吗？

---

## 第十部分：删除中间节点

### 第 18 题

原链表：

```
10 → 20 → 30 → 40 → NULL
```

现在：

```
prev → 20
curr → 30
```

执行：

```
prev->next = curr->next;
```

问：

执行后：

```
20 的 next 指向谁？
```

链表从 `head` 看起来变成什么？

此时 `30` 是否已经被释放？

还需要执行什么？

---

### 第 19 题：为什么要两个指针？

如果：

```
curr → 30
```

你想删除30。

为什么只知道 `curr` 不够？

为什么通常还需要：

```
Node *prev;
```

回答时尽量用：

```
谁的 next 需要被修改
```

来解释。

---

## 第十一部分：找 bug

### 第 20 题

下面的删除头节点代码：

```
head = head->next;
free(head);
```

假设原链表：

```
10 → 20 → 30 → NULL
```

问：

1. 第一行之后 `head` 指向谁？
2. 第二行释放的是谁？
3. 本来真正想释放的是谁？
4. 10节点会出现什么问题？

请写出正确代码。

---

## 第十二部分：简单手写

### 第 21 题：写遍历函数

补全：

```
void print_list(Node *head)
{
    Node *p = ______;

    while (__________)
    {
        printf("%d\n", __________);

        p = __________;
    }
}
```

---

### 第 22 题：写头插

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

---

## 第十三部分：二级指针

### 第 23 题

```
Node *head = NULL;

Node **pp = &head;
```

回答：

1. `head` 的类型是什么？
2. `&head` 的类型是什么？
3. `pp` 的类型是什么？
4. `pp` 里面保存什么？
5. `*pp` 是什么？
6. 如果：

```
*pp = new_node;
```

实际上修改的是谁？

这题对你非常重要。

---

### 第 24 题：二级指针头插

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

---

## 第十四部分：稍微提高

### 第 25 题：预测输出

```
Node a;
Node b;
Node c;

a.data = 10;
b.data = 20;
c.data = 30;

a.next = &b;
b.next = &c;
c.next = NULL;

Node *head = &a;
Node *p = head;

printf("%d\n", p->data);

p = p->next;

printf("%d\n", p->data);

p->data = 100;

printf("%d\n", b.data);
```

最终输出是什么？

重点解释最后为什么：

```
b.data
```

也发生了变化。

---

### 第 26 题：修改 next

原来：

```
a → b → c → NULL
```

执行：

```
a.next = &c;
```

现在从 `a` 开始遍历得到什么？

```
a → ?
```

那 `b` 节点：

- 被释放了吗？
- 还存在吗？
- 还能通过 `a` 找到它吗？

---

### 第 27 题：很重要的综合题

假设：

```
Node *head = NULL;

head = insert_head(head, 10);
head = insert_head(head, 20);
head = insert_head(head, 30);
```

最终链表是：

A.

```
10 → 20 → 30 → NULL
```

B.

```
30 → 20 → 10 → NULL
```

C.

```
30 → 10 → 20 → NULL
```

解释每一次 `insert_head()` 后链表分别是什么。
