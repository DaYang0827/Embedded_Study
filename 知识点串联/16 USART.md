# 16 USART

#node/外设 #node/核心

## 这是什么

串口字节流通信，常用于调试和固件下载。

## 串联位置

USART -> 中断/DMA -> Buffer -> 数据帧 -> Bootloader

## 直接相关

- [[11 回调函数]]
- [[15 中断]]
- [[17 DMA]]
- [[18 Buffer]]
- [[20 数据帧]]
- [[27 IAP]]

## 后续应用

- 日志输出、上位机通信、Bootloader 串口升级。

## 回看原笔记

- [[Peripheral/USART Knowledge|USART Knowledge]]

## 入口

- [[00 小知识点串联图]]

