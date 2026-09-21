# 27 IAP

#node/Bootloader #node/项目

## 这是什么

应用内编程：运行中接收新固件并写入 Flash。

## 串联位置

USART/DMA -> 数据帧/CRC -> Flash写入 -> APP跳转

## 直接相关

- [[16 USART]]
- [[17 DMA]]
- [[20 数据帧]]
- [[22 CRC]]
- [[23 Flash]]
- [[28 Bootloader跳转APP]]

## 后续应用

- 串口升级、OTA 的底层思想、项目面试核心链路。

## 回看原笔记

- [[Bootloader#1.3 IAP 模式|Bootloader - IAP]]

## 入口

- [[00 小知识点串联图]]

