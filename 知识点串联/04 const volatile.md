# 04 const volatile

#node/C基础 #node/嵌入式C

## 这是什么

const 管只读语义，volatile 管硬件/中断/DMA 可能改变的值。

## 串联位置

const/volatile -> 指针声明 -> 寄存器映射 -> 中断共享变量

## 直接相关

- [[05 指针]]
- [[13 寄存器映射]]
- [[15 中断]]
- [[17 DMA]]

## 后续应用

- volatile uint32_t * 访问外设寄存器，中断标志和 DMA 状态不能被优化掉。

## 回看原笔记

- [[Program C#1.5 `const`|const]]
- [[Program C#1.6 `volatile`|volatile]]

## 入口

- [[00 小知识点串联图]]

