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

## 1.4 状态标志位
| 中断/标志  |      含义      |     什么时候触发      |     常见用途      |
| :----: | :----------: | :-------------: | :-----------: |
| `RXNE` |  接收数据寄存器非空   |     收到一个字节      |    普通逐字节接收    |
| `IDLE` |     总线空闲     |  接收完一段数据后出现空闲   |   DMA 不定长接收   |
| `TXE`  |   发送数据寄存器空   |   可以继续放下一个字节    |   中断方式连续发送    |
|  `TC`  |    整帧发送完成    |  最后一个字节连停止位都发完  | RS485 切换收发方向  |
| `ORE`  |     接收溢出     | 上个字节没读走，下个字节又来了 |     错误检测      |
|  `FE`  |     帧错误      |     停止位不正确      | 波特率不匹配、线路干扰检测 |
|  `NE`  |     噪声错误     |    接收采样检测到噪声    |    通信质量检测     |
|  `PE`  |    奇偶校验错误    |     校验结果错误      |  开启奇偶校验时检测错误  |
| `CTS`  |   CTS 状态变化   |    硬件流控状态变化     | RTS/CTS 硬件流控  |
| `LBD`  | LIN Break 检测 |  检测到 LIN Break  |   LIN 总线通信    |

**RXNE看接收：有没有数据可以读；TXE 看发送：有没有空位可以写**
```c
//接收数据寄存器非空，也就是USART 已经收到了一个字节，这个字节现在放在接收数据寄存器里，还没有被你读走
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);	//在读取到这个字节之后，RXNE会自动清除
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


## 1.5 中断以及对应的清除标志位
### 1.5.1 定义

单片机的硬件逻辑是：**只要中断标志位为 1，就会疯狂触发中断。如果不在中断服务函数里手动或自动把它清零（清除标志位），CPU 就会永远卡在中断里，无法执行 `main` 函数的其他代码。**

核心寄存器：
1. **状态寄存器**（在 F1/F4 系列叫 `SR`，在 F7/H7/G4 等新系列叫 `ISR`）：里面存放着各种标志位（比如 RXNE、IDLE）。
2. **数据寄存器**（在 F1/F4 叫 `DR`，在新系列分成了 `RDR` 接收和 `TDR` 发送）：里面存放着串口接收到的字节。

### 1.5.2 普通接收读取
- **含义**：串口硬件每收到 1 个字节，该位自动置 1。
- **如何触发中断**：开启了 `USART_IT_RXNE`。
- **如何清除**：**不需要特意去写清除指令**。只要 CPU 或 DMA **读取了数据寄存器（DR 或 RDR）**，硬件就会**自动将 RXNE 清零**。

```c
void USART1_IRQHandler(void)
{
    uint8_t data = 0;

    if(USART_GetITStatus(USART1, USART_FLAG_RXNE))
    {
        data = USART_ReceiveData(USART1);
        rb_write(&rb, data);
    }
}
```

### 1.5.3 IDLE

**USART IDLE（串口空闲中断） + DMA（直接内存访问）** 是一种被誉为“黄金搭档”的接收方式。它的核心优势在于：**高效、省 CPU、能接收不定长的数据。**

- **含义**：总线上超过 1 个字节的时间没有数据波动，该位自动置 1。
- **如何触发中断**：开启了 `USART_IT_IDLE`。
- **如何清除**：**它的清除方式在不同系列的 STM32 芯片上是不一样的！**（这也是很多人写代码卡死的原因）。

早期芯片（如 STM32F1 / F4 系列）：

- **清除规则**：必须通过 **“组合拳”** 来清除：**先读取 SR 寄存器，再读取 DR 寄存器**。
```c
uint32_t temp;
temp = USART1->SR; // 读状态寄存器
temp = USART1->DR; // 读数据寄存器（此时 IDLE 标志位自动清零）
```
