# 概念

 **BSP = Board Support Package，板级支持包。**  它的作用是把“这块具体硬件板子怎么用”封装起来，**让上层业务代码尽量不直接碰芯片寄存器和板级细节**。

可以先把整个工程理解成：

```text
Application / Business Logic
        ↓
Middleware / RTOS / Protocol
        ↓
BSP
        ↓
HAL / LL / Driver
        ↓
MCU Registers
        ↓
Hardware
```

但实际项目里 BSP、HAL、Driver 的边界会因公司和项目而不同，没有一个全行业绝对统一的划分。

# 1.  BSP

假设写：

```c
GPIO_SetBits(GPIOA, GPIO_Pin_5);
```

这段代码上层直接知道LED接在GPIOA、Pin5，以后换板子LED从 PA5，换成 PB12，所有业务代码里如果都写死了：

```
GPIOA
GPIO_Pin_5
```

就很麻烦。

所以可以做 BSP：

```
void BSP_LED_On(void);
void BSP_LED_Off(void);
```

上层：

```
BSP_LED_On();
```

至于底层到底：

```
GPIO_SetBits(GPIOA, GPIO_Pin_5);
```

还是：

```
GPIO_SetBits(GPIOB, GPIO_Pin_12);
```

上层不用知道。

这就是 BSP 最核心的价值：

> **隔离板级硬件差异。**

---

### 2. BSP 和 Driver 有什么区别

这个特别容易混。

你可以先这样理解：

```
Driver
→ 驱动“某一种硬件外设”

BSP
→ 描述“这块板子具体用了哪些硬件，以及怎么组合”
```

比如 USART Driver：

```
uart_init();
uart_send();
uart_receive();
```

它关注：

```
USART本身怎么工作
```

BSP UART：

```
BSP_DebugUart_Init();
BSP_DebugUart_Send();
```

它关注：

```
这块板子的Debug串口到底是哪一个USART
用了哪些GPIO
波特率是多少
DMA是哪一路
```

比如你 STM32F4 项目里：

```
USART1
PA9 TX
PA10 RX
115200
DMA2某个Stream
```

这些其实都属于很典型的板级信息。

---

### 3. 用你的项目举例

你现在的项目可能有：

```
USART1
LED
Flash
DMA
Button
```

那 BSP 可以组织成：

```
BSP/
├── bsp_led.c
├── bsp_led.h
├── bsp_uart.c
├── bsp_uart.h
├── bsp_flash.c
├── bsp_button.c
└── bsp_board.c
```

比如：

```
void BSP_Init(void)
{
    BSP_LED_Init();
    BSP_UART_Init();
    BSP_Button_Init();
}
```

然后 `main()`：

```
int main(void)
{
    BSP_Init();

    while (1)
    {
        app_process();
    }
}
```

这样 `main()` 就不会塞满：

```
RCC_AHB1PeriphClockCmd(...)
GPIO_Init(...)
USART_Init(...)
DMA_Init(...)
NVIC_Init(...)
```

---

### 4. BSP 里面一般放什么

常见可以放：

```
板级GPIO定义
LED
Button
UART
SPI
I2C
Sensor
Display
External Flash
EEPROM
SD Card
Power Control
Board Init
```

例如：

```
#define BSP_LED_PORT GPIOA
#define BSP_LED_PIN  GPIO_Pin_5
```

或者：

```
void BSP_LED_Init(void);
void BSP_LED_On(void);
void BSP_LED_Off(void);
void BSP_LED_Toggle(void);
```

---

### 5. BSP 不应该放什么

BSP 一般不应该塞太多业务逻辑。

比如：

```
void BSP_LED_On_When_CRC_Failed(void);
```

这种就很奇怪。

因为：

```
CRC失败
```

是业务逻辑。

BSP 只应该提供：

```
BSP_LED_On();
```

然后上层：

```
if (crc_failed)
{
    BSP_LED_On();
}
```

所以：

> BSP 应该“提供硬件能力”，而不是“决定业务行为”。

---

### 6. BSP 和 HAL 的关系

HAL：

```
Hardware Abstraction Layer
硬件抽象层
```

比如 STM32 HAL：

```
HAL_UART_Transmit()
HAL_GPIO_WritePin()
HAL_SPI_Transmit()
```

这些 API 是 ST 给你的，面向 MCU 外设。

BSP 可以在 HAL 上再封一层：

```
void BSP_Debug_Print(const uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(
        &huart1,
        data,
        len,
        100
    );
}
```

上层只写：

```
BSP_Debug_Print(...);
```

所以：

```
Application
↓
BSP
↓
HAL
↓
Registers
```

是很常见的关系。

---

### 7. 但有时候 BSP 会直接操作寄存器

完全可以。

比如：

```
void BSP_LED_On(void)
{
    GPIOA->BSRR = (1U << 5);
}
```

所以 BSP 不等于：

> 必须依赖 HAL。

BSP 是架构层概念。

它下面可以用：

```
HAL
LL
SPL
Registers
Vendor Driver
```

都可以。

---

### 8. BSP 和 CMSIS 又是什么关系

你 STM32F4 里还有：

```
CMSIS
```

CMSIS 更底层。

大致可以理解：

```
Application
↓
BSP
↓
HAL / LL / SPL
↓
CMSIS
↓
Cortex-M / MCU Registers
```

CMSIS 提供：

```
NVIC
SCB
SysTick
Core Register定义
MCU寄存器结构
```

比如：

```
SCB->VTOR
NVIC_EnableIRQ()
```

属于更底层的 Cortex-M 接口。

---

### 9. BSP 和 RTOS 的关系

有了 FreeRTOS 后可以变成：

```
Application Tasks
        ↓
Service / Middleware
        ↓
BSP
        ↓
HAL / Driver
        ↓
Hardware
```

例如：

```
ParserTask
↓
BSP_UART_Send()
↓
UART Driver
↓
USART1
```

或者：

```
SensorTask
↓
BSP_IMU_Read()
↓
I2C Driver
↓
LSM6DSL
```

---

### 10. BSP 里面要不要放 Mutex

这个问题非常工程化。

有两种做法。

一种：

```
BSP_UART_Send()
```

内部自己保护：

```
xSemaphoreTake(uart_mutex, ...);
HAL_UART_Transmit(...);
xSemaphoreGive(uart_mutex);
```

好处：

```
所有调用者天然线程安全
```

另一种：

```
BSP只做硬件访问
Mutex交给上层Service管理
```

好处：

```
BSP不依赖RTOS
更容易裸机复用
```

对于你现在学习阶段，我更建议：

> **先让 BSP 尽量不依赖 FreeRTOS。**

比如：

```
BSP_UART_SendRaw();
```

然后 RTOS 层：

```
UART_Service_Send();
```

负责 Mutex。

这样分层更清楚。

---

### 11. 一个更完整的项目结构

你以后 Bootloader + FreeRTOS 项目可以逐渐整理成：

```
Project/
│
├── BSP/
│   ├── bsp_uart.c
│   ├── bsp_flash.c
│   ├── bsp_led.c
│   └── bsp_board.c
│
├── Drivers/
│   ├── ringbuffer.c
│   └── crc.c
│
├── Protocol/
│   ├── parser.c
│   └── protocol.c
│
├── Update/
│   ├── update_manager.c
│   └── metadata.c
│
├── RTOS/
│   ├── parser_task.c
│   └── cmd_task.c
│
└── App/
    └── main.c
```

这样以后你面试的时候就不只是说：

> 我写了很多 `.c` 文件。

而是可以说：

> 我把板级驱动、协议解析、升级管理和 RTOS Task 做了分层。

这就是架构能力。

---

### 12. 你的 USART 怎么放进 BSP

比如你现在 USART1：

```
PA9 TX
PA10 RX
115200
DMA
IDLE
```

可以：

```
void BSP_UART_Init(void);
void BSP_UART_StartRxDMA(void);
void BSP_UART_Send(const uint8_t *data, uint16_t len);
uint16_t BSP_UART_GetRxPos(void);
```

而：

```
RingBuffer
Parser
Protocol
```

不应该塞进：

```
bsp_uart.c
```

因为这些已经是更上层的软件逻辑。

所以：

```
USART寄存器/DMA
→ BSP

RingBuffer
→ Driver/Data Structure

Parser
→ Protocol Layer
```

---

### 13. Flash BSP 怎么做

比如：

```
bool BSP_Flash_EraseSector(uint32_t sector);
bool BSP_Flash_WriteWord(uint32_t addr, uint32_t data);
bool BSP_Flash_Read(uint32_t addr, void *buf, uint32_t len);
```

而：

```
“擦除APP”
“保存Metadata”
“执行OTA”
```

不要放 BSP。

应该：

```
BSP_Flash
↓
Update Manager
```

例如：

```
bool Update_EraseApp(void)
{
    BSP_Flash_EraseSector(...);
    BSP_Flash_EraseSector(...);
    BSP_Flash_EraseSector(...);

    return true;
}
```

这样：

```
BSP
→ 我会擦Flash

Update Manager
→ 我知道升级应该擦哪些区域
```

职责不同。

---

### 14. BSP 和 Bootloader 的关系

你的 Bootloader 后面可以做成：

```
Bootloader Application
        │
        ▼
Update Manager
        │
        ├── Protocol
        ├── CRC
        ├── Metadata
        │
        ▼
       BSP
    ┌────┼────┐
    ▼    ▼    ▼
   UART Flash LED
```

Bootloader 的：

```
升级状态
CRC验证
APP合法性
Jump逻辑
```

属于：

```
Bootloader / Update层
```

而：

```
Flash怎么解锁
Sector怎么擦
USART怎么初始化
LED在哪个Pin
```

属于：

```
BSP / Driver层
```

---

### 15. BSP 和 OTA 的关系

以后 OTA：

```
BLE
Wi-Fi
UART
```

其实只是不同的数据来源。

理想架构：

```
UART Transport ─┐
BLE Transport  ──┼──> Update Manager
WiFi Transport ──┘
                     │
                     ▼
                  BSP Flash
```

Update Manager 不应该知道：

```
Flash寄存器怎么操作
```

它只调用：

```
BSP_Flash_Write(...);
```

同时它也不应该特别依赖 USART：

```
Update Manager
只接收：
offset
data
length
```

这样以后：

```
USART升级
↓
复用Update Manager

BLE OTA
↓
还是复用Update Manager
```

这就是你后面 OTA 架构最重要的价值之一。

---

### 16. BSP 和“可移植性”

比如你现在：

```
STM32F407
```

以后换：

```
STM32H7
```

如果业务层大量出现：

```
USART1->SR
FLASH->CR
GPIOA->MODER
```

那整个项目都得改。

如果上层只知道：

```
BSP_UART_Send();
BSP_Flash_Write();
BSP_LED_On();
```

那么你主要替换：

```
BSP
```

上层：

```
Parser
Update Manager
OTA State Machine
```

可以尽量不改。

所以：

> **BSP 是硬件移植边界。**

---

### 17. BSP、Driver、Middleware、Application

你可以先这样区分：

|层|负责什么|示例|
|---|---|---|
|Application|业务行为|OTA流程、Bootloader逻辑|
|Middleware|通用中间功能|FreeRTOS、协议栈、文件系统|
|Service|业务能力封装|Update Manager、UART Service|
|BSP|这块板子怎么访问硬件|LED、UART、Flash、Sensor|
|Driver|某类硬件怎么驱动|USART Driver、SPI Driver|
|HAL/CMSIS|MCU/CPU底层抽象|STM32 HAL、NVIC、SCB|

实际项目不一定六层全部都有。

重点是：

> **职责不要混在一起。**

---

### 18. 你最容易犯的架构问题

比如写成：

```
void USART1_IRQHandler(void)
{
    // DMA
    // RingBuffer
    // Parser
    // CRC
    // Flash
    // Jump APP
}
```

这就是：

```
硬件层
协议层
业务层
升级层
```

全部混在一起。

更好的：

```
USART ISR
↓
BSP/Driver处理硬件事件
↓
RingBuffer / Notification
↓
ParserTask
↓
Protocol
↓
Update Manager
↓
BSP Flash
```

这就是分层。

---

### 19. 面试问“什么是 BSP”怎么答

你可以直接回答：

> BSP 是 Board Support Package，也就是板级支持包。它位于上层应用和底层硬件驱动之间，主要封装一块具体板子的硬件资源，比如 LED、UART、SPI、Flash 和传感器等。它的作用是隔离板级硬件差异，让上层代码尽量不直接依赖具体 GPIO、USART 实例或寄存器。比如上层调用 `BSP_LED_On()`，并不需要知道 LED 实际接在 PA5 还是 PB12。这样以后换板子时主要修改 BSP，而上层业务逻辑可以尽量保持不变。

如果再结合项目：

> 在我的 Bootloader 里，我可以把 USART 初始化、Flash 擦写这些底层操作放在 BSP，而 Parser、CRC 校验、Metadata 和升级状态机放在更上层，这样以后把 UART 升级换成 BLE OTA 时，Update Manager 可以继续复用。

这个回答就很完整。

---

你现在对 BSP 最值得记住的就是：

```
BSP
= Board Support Package
= “这块板子的硬件到底怎么用”
```

然后：

```
上层：
我要发数据
→ BSP_UART_Send()

BSP：
这块板子用USART1 PA9/PA10
→ 操作具体硬件
```