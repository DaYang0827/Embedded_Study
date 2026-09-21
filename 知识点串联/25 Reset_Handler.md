# 25 Reset_Handler

#node/启动

## 这是什么

复位后的入口函数，初始化运行环境后进入 main。

## 串联位置

Reset_Handler -> SystemInit/内存初始化 -> main

## 直接相关

- [[10 函数指针]]
- [[24 向量表]]
- [[26 MSP VTOR]]
- [[28 Bootloader跳转APP]]

## 后续应用

- APP 合法性检查、Bootloader 不直接跳 main 的原因。

## 回看原笔记

- [[Eight-Stock Recitation Draft#34. `Reset_Handler` 做什么？ ★★★★★|Reset_Handler]]

## 入口

- [[00 小知识点串联图]]

