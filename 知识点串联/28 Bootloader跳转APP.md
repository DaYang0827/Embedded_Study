# 28 Bootloader跳转APP

#node/Bootloader #node/核心 #node/应用

## 这是什么

关中断、停 SysTick、设 MSP、改 VTOR，用函数指针跳 Reset_Handler。

## 串联位置

函数指针 + MSP + VTOR + Reset_Handler -> APP启动

## 直接相关

- [[10 函数指针]]
- [[22 CRC]]
- [[23 Flash]]
- [[24 向量表]]
- [[25 Reset_Handler]]
- [[26 MSP VTOR]]
- [[27 IAP]]
- [[33 面试应用]]

## 后续应用

- Bootloader 项目最重要的面试讲解点之一。

## 回看原笔记

- [[Eight-Stock Recitation Draft#91. Bootloader 如何跳转 APP？ ★★★★★|Bootloader 跳 APP]]

## 入口

- [[00 小知识点串联图]]

