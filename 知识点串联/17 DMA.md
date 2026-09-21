# 17 DMA

#node/外设 #node/核心

## 这是什么

在外设和内存之间搬运数据，减少 CPU 逐字节处理。

## 串联位置

USART/SPI/ADC -> DMA -> Buffer -> Ring Buffer

## 直接相关

- [[04 const volatile]]
- [[05 指针]]
- [[13 寄存器映射]]
- [[16 USART]]
- [[18 Buffer]]
- [[19 Ring Buffer]]

## 后续应用

- USART + IDLE 接收不定长数据、ADC 连续采样、SPI 大块传输。

## 回看原笔记

- [[Peripheral/DMA Knowledge|DMA Knowledge]]

## 入口

- [[00 小知识点串联图]]

