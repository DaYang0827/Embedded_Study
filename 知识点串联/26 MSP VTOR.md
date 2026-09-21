# 26 MSP VTOR

#node/启动 #node/Bootloader

## 这是什么

MSP 是主栈指针，VTOR 指向当前向量表基地址。

## 串联位置

MSP + VTOR -> 正确使用 APP 栈和中断表

## 直接相关

- [[24 向量表]]
- [[25 Reset_Handler]]
- [[28 Bootloader跳转APP]]

## 后续应用

- Bootloader 跳转前设置 MSP，修改 VTOR 到 APP 向量表。

## 回看原笔记

- [[Eight-Stock Recitation Draft#31. MSP 是什么？ ★★★★★|MSP]]
- [[Eight-Stock Recitation Draft#42. VTOR 是什么？ ★★★★★|VTOR]]

## 入口

- [[00 小知识点串联图]]

