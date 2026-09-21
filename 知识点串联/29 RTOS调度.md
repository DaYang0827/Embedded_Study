# 29 RTOS调度

#node/RTOS #node/应用

## 这是什么

任务调度、上下文切换和同步通信构成 RTOS 使用主线。

## 串联位置

SysTick -> 调度 -> 上下文切换 -> Semaphore/Mutex/Queue

## 直接相关

- [[11 回调函数]]
- [[15 中断]]
- [[31 Semaphore Mutex Queue]]
- [[32 看门狗]]
- [[33 面试应用]]

## 后续应用

- ISR 通知任务处理数据，多任务划分，任务健康检查。

## 回看原笔记

- [[Free RTOS/Free RTOS|FreeRTOS]]
- [[Eight-Stock Recitation Draft#第六章 RTOS 与 Zephyr|RTOS 与 Zephyr]]

## 入口

- [[00 小知识点串联图]]

