# USART编写逻辑以及代码理解
# 1 USART Setting
## 1.1 USART Establish
1. 开 GPIO 时钟
2. 开 USART 时钟
3. 配置 GPIO 为复用模式
4. 配置 GPIO 复用功能 AF
5. 配置 USART 参数          常用的配置是115200,8N1（8位数据位，No parity无校验位，1位停止位）   所以发送一个字节线路上将会传输10个bite
6. 开 USART 接收中断 
7. 配置 NVIC
8. 使能 USART
9. 接收不同类型字符的函数需要自己写，发送函数不需要自己写

## 1.2 USART的部分组成

GPIO复用 + USART参数 + NVIC中断 + 回调函数

## 1.3 在stm32f407中常见的USART搭配

USART1：PA9 / PA10，挂在 APB2

USART2：PA2 / PA3，挂在 APB1

GPIOA：挂在AHB1

## 1.4 字节与比特

**1个字节（byte）= 8个比特（bite）**；字符 'A' 在 ASCII 码中是：十进制：65；二进制：0100 0001；十六进制：0x41

## 1.5 RXNE 和TXE

**RXNE看接收：有没有数据可以读；TXE 看发送：有没有空位可以写**
```c
//接收数据寄存器非空，也就是USART 已经收到了一个字节，这个字节现在放在接收数据寄存器里，还没有被你读走
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);						        //在读取到这个字节之后，RXNE会自动清除
    }
```
```c
//发送数据寄存器为空，也就是说：USART 的发送数据寄存器现在是空的，可以写入新的数据。
    void usart_send_byte(typedef_usart *usart, uint8_t data)
    {
        while (USART_GetFlagStatus(usart->USARTx, USART_FLAG_TXE) == RESET);
    
        USART_SendData(usart->USARTx, data);
    }
```

