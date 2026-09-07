# 1 简介

DMA （Direct Memory Access）直接存储器存取

DMA可以提供外设和存储器或者存储器和存储器之间的高速数据传输，无须CPU干预，节省了CPU的资源

如果把单片机的中央处理器（CPU）比作公司的 **“总经理”** ，那么 DMA 就是总经理亲自提拔的 **“物流主管”**。

- **没有 DMA 时**：公司的所有物资搬运（比如把串口收到的 1000 字节数据搬运到内存数组里），都必须由总经理（CPU）**亲自动手，一字节一字节地搬**。搬运期间，总经理什么重要业务都干不了，公司直接陷入卡顿（阻塞）。
- **有了 DMA 后**：总经理（CPU）只需要给物流主管（DMA）下一行命令：“把串口外设里的 1000 字节，搬到内存指定的数组里去，搬完叫我。” 随后，**DMA 就会完全接管系统总线，在硬件层面自动把数据搬完**，而 CPU 在这期间可以开心地去跑别的高级业务逻辑。

12个独立可配置的通道： DMA1（7个通道）， DMA2（5个通道）

每个通道都支持软件触发和特定的硬件触发（选择硬件触发都有对应的特定的通道）

STM32F103C8T6 DMA资源：DMA1（7个通道）

## 1.1  存储器映像

| 类型  |    起始地址     |    存储器     |         用途          |
| :-: | :---------: | :--------: | :-----------------: |
| ROM | 0x0800 0000 | 程序存储器Flash |    存储C语言编译后的程序代码    |
|     | 0x1FFF F000 |   系统存储器    | 存储BootLoader，用于串口下载 |
|     | 0x1FFF F800 |    选项字节    |  存储一些独立于程序代码的配置参数   |
| RAM | 0x2000 0000 |  运行内存SRAM  |    存储运行过程中的临时变量     |
|     | 0x4000 0000 |   外设寄存器    |     存储各个外设的配置参数     |
|     | 0xE000 0000 |  内核外设寄存器   |    存储内核各个外设的配置参数    |

## 1.2 DMA基本结构

![](file:///C:\Users\10548\AppData\Local\Temp\ksohtml12852\wps1.jpg) 

## 1.3  实际例子

1. 数据转运

![](file:///C:\Users\10548\AppData\Local\Temp\ksohtml12852\wps2.jpg) 

2. ADC扫描模式

![](file:///C:\Users\10548\AppData\Local\Temp\ksohtml12852\wps3.jpg)

## 1.4 核心参数
```c
DMA_Config.Source_Address = (uint32_t)&UART1->DR;     // 1. 源地址 

DMA_Config.Destination_Address = (uint32_t)Rx_Buffer; // 2. 目的地址 

DMA_Config.Data_Length = 100;                         // 3. 传输长度 

DMA_Config.Direction = DMA_PERIPHERAL_TO_MEMORY;      // 4. 传输方向
```

1. 传输方向（Direction）

   根据数据的来源和去向，硬件上主要分为三类流向：

- **外设到内存 (P2M)**：最常见。比如串口接收、ADC 采样、SPI 接收数据并存入数组。
- **内存到外设 (M2P)**：比如把内存里的图片数据发给 LCD 屏幕显示，或者通过串口发送一段很长的字符串。
- **内存到内存 (M2M)**：相当于 C 语言里的 `memcpy`。把 SRAM 的 A 块数据克隆到 B 块，纯硬件搬运，速度极快。

2. 基地址（Base Address）

- **源基地址（Source Address）**：数据从哪里来。
- **目的基地址（Destination Address）**：数据到哪里去。

3. 地址自增属性（Increment Mode）

   这是 DMA 自动数数的关键。你可以选择让地址 **“固定”** 还是 **“自动加 1”**：

- **外设地址通常选择【固定】**：因为外设的数据寄存器通常只有**唯一的一个物理窗口地址**（比如串口的接收寄存器就是固定的那个物理内存点，所有来的数据都从这个窗口漏下来）。
- **内存地址通常选择【自增】**：因为你要把收到的连续数据依次存入 `Buffer[0]`、`Buffer[1]`、`Buffer[2]`，每搬完一个字节，目的内存地址必须物理往前挪一步。

4. 数据位宽（Data Size）

- 必须明确规定每一次搬运的“箱子体积”。通常可选：**Byte（8位）**、**Half-Word（16位）**、**Word（32位）**。
- 避坑物理规律：源端和目的端的位宽最好保持一致。如果用 32 位位宽去搬 8 位的串口数据，会导致内存数据错位或溢出。

## 1.5 结构体配置

```c
typedef struct
{
  uint32_t DMA_Channel;               // 1. 硬件通道选择
  uint32_t DMA_PeripheralBaseAddr;     // 2. 外设基地址
  uint32_t DMA_Memory0BaseAddr;       // 3. 内存基地址（大仓库）
  uint32_t DMA_DIR;                   // 4. 数据传输方向
  uint32_t DMA_BufferSize;            // 5. 传输数据大小（计数器）
  uint32_t DMA_PeripheralInc;         // 6. 外设地址是否自增
  uint32_t DMA_MemoryInc;             // 7. 内存地址是否自增
  uint32_t DMA_PeripheralDataSize;    // 8. 外设数据宽度（单次搬运量）
  uint32_t DMA_MemoryDataSize;        // 9. 内存数据宽度（单次存入量）
  uint32_t DMA_Mode;                  // 10. 传输模式（单次/循环）
  uint32_t DMA_Priority;              // 11. 软件通道优先级
  uint32_t DMA_FIFOMode;              // 12. 是否开启 FIFO 缓存
  uint32_t DMA_FIFOThreshold;         // 13. FIFO 触发阈值
  uint32_t DMA_PeripheralBurst;       // 14. 外设突发传输配置
  uint32_t DMA_MemoryBurst;           // 15. 内存突发传输配置
} DMA_InitTypeDef;
```

### 1.5.1 `DMA_Channel`

```c
DMA_InitStructure.DMA_Channel = DMA_Channel_4;
```

这个是 **DMA 通道选择**。`Stream` 是调用 `DMA_Init()` 时选的，`Channel` 是结构体成员。可以理解成`DMA2_Stream2` 决定用哪个 DMA 搬运通道实例。`DMA_Channel_4`决定：**这个 Stream 接收哪个外设的 DMA 请求。** 这个不能随便写，必须查芯片手册里的 **DMA request mapping table**。

### 1.5.2 `DMA_PeripheralBaseAddr`

这个非常重要。

```c
DMA_InitStructure.DMA_PeripheralBaseAddr =
    (uint32_t)&USART1->DR;
```

意思是**外设端地址是多少？**

`USART RX` 时：

```text
USART1接收到数据
↓
数据进入 USART1->DR
↓
DMA 从 DR 搬走
```

所以`DMA_PeripheralBaseAddr = &USART1->DR;` 对于 ADC:`DMA_PeripheralBaseAddr = &ADC1->DR;`  对于 SPI：`DMA_PeripheralBaseAddr = &SPI1->DR;`

所以可以直接记`PeripheralBaseAddr` 通常就是某个外设的 **数据寄存器地址**。注意它存的是**地址**，不是数据。比如`USART1->DR`表示寄存器里的值。而`&USART1->DR`表示这个寄存器所在的地址。DMA 需要的是地址。
### 1.5.3 `DMA_Memory0BaseAddr`

```c
DMA_InitStructure.DMA_Memory0BaseAddr =
    (uint32_t)rx_buffer;
```

这个就是**内存端地址。

例如`uint8_t rx_buffer[8];` 那么`rx_buffer`本身在表达式中基本就等价于`&rx_buffer[0]` 所以
`DMA_Memory0BaseAddr = (uint32_t)rx_buffer;`意思就是：

```text
把数据搬到：

rx_buffer[0]
rx_buffer[1]
rx_buffer[2]
...
```

结合前面两个地址：

```text
DMA_PeripheralBaseAddr
          ↓
     USART1->DR
          ↓
         DMA
          ↓
DMA_Memory0BaseAddr
          ↓
      rx_buffer
```

这两个成员基本就是 DMA 最核心的：**源地址 / 目的地址** 不过谁是源、谁是目的，还得结合 `DMA_DIR` 看。

### 1.5.4 `DMA_DIR`

```c
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
```

意思是数据搬运方向。

STM32F4 常见三个方向。

1. 外设 → 内存

`USART RX`:`DMA_DIR_PeripheralToMemory`

数据流：

```text
USART1->DR
↓
rx_buffer[]
```

2. 内存 → 外设

`USART TX：` `DMA_DIR_MemoryToPeripheral`

数据流：

```text
tx_buffer[]
↓
USART1->DR
```

 3. 内存 → 内存

`DMA_DIR_MemoryToMemory`

比如：

```text
array_a[]
↓
DMA
↓
array_b[]
```

但通常 MCU 外设 DMA 最常见的还是前两个。

### 1.5.5 `DMA_BufferSize`

```c
DMA_InitStructure.DMA_BufferSize = 8;
```

表示**这一次 DMA 要完成多少个“数据单位”的搬运**。它跟

```
DMA_PeripheralDataSize
DMA_MemoryDataSize
```

有关。

如果配置`DMA_MemoryDataSize_Byte`那么`DMA_BufferSize = 8;`就是`8 × 1 Byte = 8 Byte`如果是`DMA_MemoryDataSize_HalfWord` 那一个单位是 2 Byte。所以`DMA_BufferSize = 8` 实际就是 16 Byte。

对于USART`uint8_t dma_buffer[8];`通常就是`DMA_BufferSize = 8;`初始时`NDTR = 8`

每成功搬一个字节

```text
8
↓
7
↓
6
↓
5
...
↓
0
```

到 0：` DMA Transfer Complete。`

### 1.5.6 `DMA_PeripheralInc`

```c
DMA_InitStructure.DMA_PeripheralInc =
    DMA_PeripheralInc_Disable;
```

这个表示每搬完一次，**外设地址要不要自动加**。外设数据寄存器通常是固定地址，因此 `PeripheralInc` 通常关闭。

### 1.5.7 `DMA_MemoryInc`

USART RX 通常：

```c
DMA_InitStructure.DMA_MemoryInc =
    DMA_MemoryInc_Enable;
```

意思是**每搬一个数据后，内存地址自动往后移动**。假如：`uint8_t rx_buffer[8];` 首地址`0x20000000`, 第一次`USART DR → 0x20000000`,第二次`USART DR → 0x20000001`,第三次`USART DR → 0x20000002`.最终：

```text
'H' → rx_buffer[0]
'E' → rx_buffer[1]
'L' → rx_buffer[2]
'L' → rx_buffer[3]
'O' → rx_buffer[4]
```

所以 UART 接收数组`DMA_MemoryInc_Enable`几乎是必然的。

如果你关闭：

```
DMA_MemoryInc_Disable
```

那么：

```
H写buffer[0]
E覆盖buffer[0]
L覆盖buffer[0]
L覆盖buffer[0]
O覆盖buffer[0]
```

最后只剩：`buffer[0] = 'O'`这个现象很重要。

---

### 1.5.8 `DMA_PeripheralDataSize`

```c
DMA_InitStructure.DMA_PeripheralDataSize =
    DMA_PeripheralDataSize_Byte;
```

表示**DMA 每次从外设端读取多大的数据。**

一般有：

```c
DMA_PeripheralDataSize_Byte
DMA_PeripheralDataSize_HalfWord
DMA_PeripheralDataSize_Word
```

对应：

```text
Byte     = 8 bit
HalfWord = 16 bit
Word     = 32 bit
```

USART 普通 8 位数据`DMA_PeripheralDataSize_Byte`

ADC 有时可能配置`DMA_PeripheralDataSize_HalfWord`

因为 ADC 数据通常是 12 bit，但装在 16 bit 数据单元里处理。

---

### 1.5.9 `DMA_MemoryDataSize`

```c
DMA_InitStructure.DMA_MemoryDataSize =
    DMA_MemoryDataSize_Byte;
```

意思类似**内存里每次存多少位。** 对于`uint8_t rx_buffer[8];`自然是`DMA_MemoryDataSize_Byte`

如果有`uint16_t adc_buffer[128];`就常见`DMA_MemoryDataSize_HalfWord`

所以可以这样对应：

```
uint8_t  → Byte
uint16_t → HalfWord
uint32_t → Word
```

不绝对，但大多数情况下非常好用。

### 1.5.10 `DMA_Mode`

这个就是：

```c
DMA_Mode_Normal
```

或者：

```c
DMA_Mode_Circular
```

1. Normal

例如：

```
NDTR = 8
↓
收1字节
7
↓
...
↓
0
```

到了 0 DMA 停止这次传输,要继续接收，CPU 需要重新配置：

```text
Disable DMA
↓
重新写 NDTR
↓
Enable DMA
```

---

2. Circular

到 0 之后：

```text
NDTR = 0
↓
自动重新加载
↓
NDTR = 8
```

继续搬。

例如：

```text
buffer[0]
buffer[1]
...
buffer[7]
↓
重新回到
buffer[0]
```

特别适合：

```text
ADC连续采样
UART连续数据流
音频
传感器流
```

### 1.5.11 `DMA_Priority`

```c
DMA_InitStructure.DMA_Priority =
    DMA_Priority_High;
```

DMA Stream 之间可能同时请求总线。比如：

```text
ADC DMA
USART DMA
SPI DMA
```

大家都想搬数据。那么谁优先？靠

```text
DMA_Priority_Low
DMA_Priority_Medium
DMA_Priority_High
DMA_Priority_VeryHigh
```

比如`DMA_Priority_High`,表示当出现 DMA 资源竞争时，这一路优先级比较高。注意：**这个和 NVIC 中断优先级不是一回事。**

DMA Priority：

```
谁优先获得 DMA / 总线服务
```

NVIC Priority：

```
CPU先响应哪个中断
```

### 1.5.12 `DMA_FIFOMode`

STM32F4 DMA 比早期 DMA 多了一个 FIFO。

例如：

```c
DMA_InitStructure.DMA_FIFOMode =
    DMA_FIFOMode_Disable;
```

DMA 有两种工作方式。

1. Direct Mode

FIFO Disable：

```
外设
↓
DMA
↓
内存
```

数据比较直接地搬。USART 这种一个字节一个字节来的数据，通常`DMA_FIFOMode_Disable`

---

2. FIFO Mode

FIFO Enable：

```text
外设
↓
DMA内部FIFO
↓
攒一部分
↓
再往内存搬
```

FIFO 可以配合 burst，提高某些高吞吐场景效率。

### 1.5.13 `DMA_FIFOThreshold`

只有开启 FIFO 时才真正重要：

```text
DMA_FIFOThreshold_1QuarterFull
DMA_FIFOThreshold_HalfFull
DMA_FIFOThreshold_3QuartersFull
DMA_FIFOThreshold_Full
```

意思是 **FIFO 里面攒到多少程度后进行相关传输。**

例如`DMA_FIFOThreshold_Full`表示 FIFO 达到完整阈值。但如果`DMA_FIFOMode_Disable`这个参数对你当前 USART DMA 基本不用操心。

---

### 1.5.14 `DMA_MemoryBurst`

例如：

```c
DMA_InitStructure.DMA_MemoryBurst =
    DMA_MemoryBurst_Single;
```

表示 **DMA 每次访问内存，是一次搬一个，还是连续 burst 搬多个。** 有：

```text
DMA_MemoryBurst_Single
DMA_MemoryBurst_INC4
DMA_MemoryBurst_INC8
DMA_MemoryBurst_INC16
```

例如`Single`就是一次一个,`INC4`就是连续4次。UART 收一个字节一个字节来的场景，通常`DMA_MemoryBurst_Single`

### 1.5.15 `DMA_PeripheralBurst`

同样：

```c
DMA_InitStructure.DMA_PeripheralBurst =
    DMA_PeripheralBurst_Single;
```

表示**对外设侧一次进行多少个连续传输。** 有：

```
DMA_PeripheralBurst_Single
DMA_PeripheralBurst_INC4
DMA_PeripheralBurst_INC8
DMA_PeripheralBurst_INC16
```

最重要的是这几个核心成员

|            成员            |  你脑子里要问的问题   |
| :----------------------: | :----------: |
|      `DMA_Channel`       | 哪个外设 DMA 请求？ |
| `DMA_PeripheralBaseAddr` |   外设数据在哪？    |
|  `DMA_Memory0BaseAddr`   |  RAM 数据放哪？   |
|        `DMA_DIR`         |     往哪搬？     |
|     `DMA_BufferSize`     |     搬几个？     |
|   `DMA_PeripheralInc`    |   外设地址动不动？   |
|     `DMA_MemoryInc`      |   内存地址动不动？   |
|        `DMA_Mode`        |  搬完停止还是循环？   |

```text
使用 Channel 4
        ↓
外设地址 = USART1 数据寄存器
        ↓
内存地址 = dma_buffer
        ↓
数据方向 = USART → RAM
        ↓
总共搬 DmaBufferSize 个数据
        ↓
USART DR 地址不变
        ↓
buffer 地址每次+1
        ↓
USART端每次8bit
        ↓
RAM端每次8bit
        ↓
Normal模式
        ↓
高优先级
        ↓
不用FIFO
        ↓
每次单次搬运
```
