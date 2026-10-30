# 1 分类

BSP、Driver、Middleware、Application，可以这样区分：

|      层      |    负责什么     |             示例              |
| :---------: | :---------: | :-------------------------: |
| Application |    业务行为     |     OTA流程、Bootloader逻辑      |
| Middleware  |   通用中间功能    |      FreeRTOS、协议栈、文件系统      |
|   Service   |   业务能力封装    | Update Manager、UART Service |
|     BSP     | 这块板子怎么访问硬件  |    LED、UART、Flash、Sensor    |
|   Driver    |  某类硬件怎么驱动   |   USART Driver、SPI Driver   |
|  HAL/CMSIS  | MCU/CPU底层抽象 |     STM32 HAL、NVIC、SCB      |

实际项目不一定六层全部都有。重点是**职责不要混在一起。**

# 2 BSP
## 2.1 概念

 **BSP = Board Support Package，板级支持包。**  它的作用是把“这块具体硬件板子怎么用”封装起来，**让上层业务代码尽量不直接碰芯片寄存器和板级细节**。

可以把整个工程理解成：

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

## 2.2 BSP

假设写：

```c
GPIO_SetBits(GPIOA, GPIO_Pin_5);
```

这段代码上层直接知道LED接在GPIOA、Pin5，以后换板子LED从 PA5，换成 PB12，所有业务代码里如果都写死了：

```c
GPIOA
GPIO_Pin_5
```

就很麻烦。所以可以做 BSP：

```c
void BSP_LED_On(void);
void BSP_LED_Off(void);
```

上层 `BSP_LED_On();`，至于底层到底 `GPIO_SetBits(GPIOA, GPIO_Pin_5);` 还是 `GPIO_SetBits(GPIOB, GPIO_Pin_12);`上层不用知道。

这就是 BSP 最核心的价值**隔离板级硬件差异。**

## 2.3 BSP 和 Driver 

这个特别容易混。可以这样理解：

```text
Driver → 驱动“某一种硬件外设”

BSP → 描述“这块板子具体用了哪些硬件，以及怎么组合”
```

比如 USART Driver：

```c
uart_init();
uart_send();
uart_receive();
```

它关注 USART 本身怎么工作，BSP UART：

```c
BSP_DebugUart_Init();
BSP_DebugUart_Send();
```

它关注这块板子的Debug串口到底是哪一个USART，用了哪些GPIO，波特率是多少，DMA是哪一路。比如 STM32F4 项目里：

```c
USART1
PA9 TX
PA10 RX
115200
DMA2某个Stream
```

这些其实都属于很典型的板级信息。

### 2.3.1 项目举例

项目中可能有：

```text
USART1
LED
Flash
DMA
Button
```

那 BSP 可以组织成：

```text
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

```c
void BSP_Init(void)
{
    BSP_LED_Init();
    BSP_UART_Init();
    BSP_Button_Init();
}
```

然后 `main()`：

```c
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

```c
RCC_AHB1PeriphClockCmd(...)
GPIO_Init(...)
USART_Init(...)
DMA_Init(...)
NVIC_Init(...)
```

## 2.4 BSP 内容

常见可以放：

```text
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

```c
#define BSP_LED_PORT GPIOA
#define BSP_LED_PIN  GPIO_Pin_5
```

或者：

```c
void BSP_LED_Init(void);
void BSP_LED_On(void);
void BSP_LED_Off(void);
void BSP_LED_Toggle(void);
```

### 2.4.1 不应该放什么

BSP **一般不应该塞太多业务逻辑**。比如：

```c
void BSP_LED_On_When_CRC_Failed(void);
```

这种就很奇怪。因为 CRC 失败是业务逻辑。BSP 只应该提供 `BSP_LED_On();` 然后上层：

```c
if (crc_failed)
{
    BSP_LED_On();
}
```

所以 BSP 应该“提供硬件能力”，而不是“决定业务行为”。

## 2.5 BSP 和 HAL 

HAL（Hardware Abstraction Layer）**硬件抽象层**，比如 STM32 HAL：

```c
HAL_UART_Transmit()
HAL_GPIO_WritePin()
HAL_SPI_Transmit()
```

这些 API 是 ST 给的，面向 MCU 外设。BSP 可以在 HAL 上再封一层：

```c
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

上层只写 `BSP_Debug_Print(...);`，所以：

```text
Application
↓
BSP
↓
HAL
↓
Registers
```

是很常见的关系。

## 2.6 BSP 和 RTOS 

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

## 2.7 一个完整的项目结构

 Bootloader + FreeRTOS 项目可以逐渐整理成：

```text
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

这样以后面试的时候就不只是说写了很多 `.c` 文件。而是可以说把板级驱动、协议解析、升级管理和 RTOS Task 做了分层。这就是架构能力。

## 2.8 面试问“什么是 BSP”

可以直接回答：BSP 是 Board Support Package，也就是板级支持包。它**位于上层应用和底层硬件驱动之间，主要封装一块具体板子的硬件资源**，比如 LED、UART、SPI、Flash 和传感器等。它的作用是隔离板级硬件差异，让上层代码尽量不直接依赖具体 GPIO、USART 实例或寄存器。比如上层调用 `BSP_LED_On()`，并不需要知道 LED 实际接在 PA5 还是 PB12。这样以后换板子时主要修改 BSP，而上层业务逻辑可以尽量保持不变。

如果再结合项目在 Bootloader 里，可以把 USART 初始化、Flash 擦写这些底层操作放在 BSP，而 Parser、CRC 校验、Metadata 和升级状态机放在更上层，这样以后把 UART 升级换成 BLE OTA 时，Update Manager 可以继续复用。

这个回答就很完整。

对 BSP 最值得记住的就是：

```text
BSP
= Board Support Package
= “这块板子的硬件到底怎么用”
```

然后：

```text
上层：
要发数据
→ BSP_UART_Send()

BSP：
这块板子用USART1 PA9/PA10
→ 操作具体硬件
```

