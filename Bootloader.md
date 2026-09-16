# 1 嵌入式固件固化与更新方式

## 1.1 ICP
（ In-Circuit Programming，在电路编程）

- **技术定义**：通过外部专用的硬件烧录器/调试器，直接对芯片上的专用调试引脚进行物理操作，将程序强制写入 Flash。
- **执行主体**：**外部硬件工具**（如`J-Link、ST-Link、DAP-Link` 等）。
- **芯片状态**：芯片内的 CPU 处于内核复位或暂停状态，不运行任何用户代码。
- **工作原理**：烧录器利用 **SWD** 或 **JTAG** 协议，通过芯片的仿真引脚接管总线控制权，直接向内部 Flash 的物理地址写入数据。
- **核心特点**：
    - **优点**：速度最快；支持在线硬件断点调试（仿真）；即使芯片内部程序彻底崩塌（变砖），也能强制擦除重烧。
    - **缺点**：必须引出 SWD/JTAG 物理引脚；无法做到远程或无线操作。
- **应用场景**：研发阶段的**日常代码调试**、工厂流水线上的**首次程序盲烧**。

## 1.2 ISP 模式
（In-System Programming，在系统编程）

- **技术定义**：利用芯片厂家在出厂时就**固化在 ROM 中且不可修改的引导程序（BootLoader）**，通过特定的外设接口接收并烧录程序。
- **执行主体**：**芯片厂家自带的内建引导代码**（不可擦除） 电子工程专辑。
- **芯片状态**：在系统复位时，通过调整特定引脚（如 `STM32 的 BOOT0/BOOT1 引脚`）的电平，强行让 CPU 在开机时跳转到厂家自带的 ROM 区运行。
- **工作原理**：芯片启动后运行厂家引导代码，开启指定的通讯接口（通常是 **USART1 串口**、USB 或 CAN），配合电脑端的厂家烧录软件（如 `STM32CubeProgrammer`），通过通信协议将固件写入用户 Flash 区 电子工程专辑。
- **核心特点**：
    - **优点**：不需要昂贵的硬件调试器，只需一根廉价的 USB 转串口线即可烧录；由于引导代码在 ROM 中，绝对不会损坏。
    - **缺点**：需要人为手动切换 BOOT 引脚的电平状态。
- **应用场景**：产品量产时的流水线烧录、以及产品因用户代码死锁（变砖）导致 ICP 无法连接时的**底层紧急救砖**。

## 1.3 IAP 模式
（In-Application Programming，在应用编程）

- **技术定义**：在产品正常运行过程中，由**用户自己编写的 Bootloader 软件**，通过任意通用的通信接口接收新固件，并调用芯片内部的 Flash 读写驱动库实现固件的自我擦除与更新。
- **执行主体**：**开发者自己编写的 Bootloader 程序** 电子工程专辑。
- **芯片状态**：芯片处于正常的业务运行状态。Flash 空间通常被划分为 **Bootloader 区**和 **Application（应用）区**。
- **工作原理**：
    1. 运行在 Application 区的程序通过无线（Wi-Fi/4G/蓝牙）或有线（RS485/CAN）网络收到升级指令。
    2. 系统将新固件下载到指定的备份区，或者直接重启进入 Bootloader 区 电子工程专辑。
    3. Bootloader 程序接管系统，对收到的固件进行 CRC 校验或加密解密 电子工程专辑。
    4. 校验成功后，调用芯片内部的 `Flash_Erase` 和 `Flash_Write` 函数，将新固件覆盖写入主应用区，最后跳转执行 电子工程专辑。
- **核心特点**：
    - **优点**：升级接口完全由软件定义（支持云端 OTA、U盘、手机App等）；无需拆卸设备，无需改动任何物理引脚。
    - **缺点**：占用额外的 Flash 存储空间（用来存 Bootloader 甚至备份区） 电子工程专辑；若 Bootloader 代码有逻辑漏洞，可能导致设备彻底变砖。
- **应用场景**：消费电子、物联网设备、汽车电子等所有出厂后需要**日常远程固件升级（OTA）** 或售后免拆机维护的场景。

|   特性维度    |    ICP (在电路编程)     |   ISP (在系统编程)    |        IAP (在应用编程)        |
| :-------: | :----------------: | :--------------: | :-----------------------: |
| **操作时机**  |     研发调试、生产初烧      |    量产烧录、紧急救砖     |        售后远程升级、OTA         |
| **硬件工具**  | J-Link / ST-Link 等 |  USB转串口线 / 串口板   |     无需额外工具（利用现有通信链路）      |
| **软件驱动者** |      电脑端调试器软件      |  芯片厂家内置 ROM 代码   | 开发者自己写的 Bootloader 电子工程专辑 |
| **物理切换**  |    否（软件或调试器控制）     | 是（需更改 BOOT 引脚电平） |       否（纯软件逻辑或命令触发）       |
| **核心目的**  |     代码仿真与首次固化      |   成本敏感型烧录与挽救系统   | 实现产品自身的无线/远程自我迭代 CNBLOGS  |

## 1.4 OTA
### 1.4.1 简介

OTA全称"Over-The-Air"，即空中下载技术，早期被广泛应用在手机行业中，中介了手机软件升级需要连接电脑、下载软件、再安装更新的反复操作。今年来，随着汽车网联技术不断发展，汽车OTA也成为了行业热词

OTA升级：通过OTA方式**实现固件或软件的升级**。只要是通过无线通信方式实现升级的，都可以叫OTA升级，比如网络/蓝牙。通过有线方式进行升级，叫本地升级，比如通过UART，USB或者SPl通信接口来升级设备固件。

### 1.4.2 OTA流程
1. 制作升级包
2. 下载升级包
3. 验签升级包
4. 更新程序

---

# 2 固件升级

 固件是指**嵌入在硬件设备中的软件，负责设备的基本操作和控制**。固件升级是指通过更新设备的固件，以修复错误，提升性能活增加新的功能

## 2.1 核心作用

- **修复漏洞**：解决安全隐患、程序死机或硬件不兼容等已知 Bug。
- **优化性能**：改进底层算法，提升设备的运行速度、功耗控制或信号质量。
- **新增功能**：无需购买新硬件，即可让旧设备支持全新的软件特性或协议标准。

## 2.2 常见升级方式

- **OTA 空中下载技术 (Over-the-Air)**：设备在联网状态下（如智能手机、智能家居、智能汽车），直接通过无线网络自动或提示用户下载并安装更新。
- **Web 界面或 APP 升级**：用户登录设备的管理界面（如路由器控制后台、小米之家 APP）点击检测并一键更新。
- **手动本地升级**：在断网或特定需求下，从厂商官网下载固件包到 U 盘、SD 卡或通过专用电脑工具（如闪存烧录软件），连接设备进行强刷。

## 2.3 STM32 固件升级

1. 本地直接烧录 (工厂/开发阶段)

- **ICP (In-Circuit Programming)**：通过 SWD 或 JTAG 接口，使用 ST-Link 等下载器直接擦写内部 Flash。
- **ISP (In-System Programming)**：利用芯片出厂内置的 Bootloader，通过 UART、USB、CAN 或 I²C 接口，配合上位机（如 STM32CubeProgrammer）进行升级。需通过控制 BOOT 引脚进入该模式。

2. 用户应用内升级 (产品上市后)

- **IAP (In-Application Programming)**：
  **核心原理**：将 Flash 划分为 Bootloader 程序区和 Application 应用区。系统运行后先进入 Bootloader，判断是否需要升级。
- **在线升级 (OTA - Over-the-Air)**：通过 Wi-Fi、蓝牙、4G 等无线模块接收固件，由 Bootloader 将新固件写入应用区。
- **本地有线升级**：通过 USB 虚拟串口、RS485、CAN 总线等有线接口读取更新文件并写入。
- **外部存储升级**：将固件存入 SD 卡或外部 `SPI Flash，Bootloader` 读取该文件并刷入内部 Flash。

## 2.4 STM32 复位流程

1. 硬件初始化与采样

当复位事件（上电、硬件复位引脚、看门狗等）发生时，芯片内部进行硬件初始化：

- **释放复位信号**：内部复位电路释放。
- **启动内部时钟**：系统默认使用内部低功耗时钟源（如 HSI 内部高速RC振荡器）作为系统时钟。
- **采样 BOOT 引脚**：在系统复位后的第4个时钟周期，内核自动采样 `BOOT0` 和 `BOOT1`（或用户选项字节 Option Bytes）的电平状态，以此决定**启动映射地址**（主闪存、系统存储器或内置SRAM）。

2. 读取堆栈指针与复位向量

根据启动映射的基地址（默认主闪存为 `0x08000000`），Cortex-M 内核自动执行以下硬件操作：

- **读取 MSP**：从启动地址的第一个字（地址 `0x08000000`）读取**主堆栈指针（Main Stack Pointer** 的初始值，并将其存入 `SP` 寄存器。
- **读取复位向量**：从启动地址的第二个字（地址 `0x08000004`）读取**复位中断服务程序（Reset_Handler）的入口地址**，并将其存入程序计数器 `PC`。

3. 执行复位中断服务程序

程序跳转至 `Reset_Handler` 汇编函数处开始执行。该阶段主要完成底层硬件环境的配置：

- **调用 `SystemInit()`**：**清除时钟寄存器，配置并切换系统时钟**（如开启 HSE 并通过 PLL 锁相环倍频至最高主频），设置向量表偏移寄存器（VTOR）。
- **运行环境初始化**：将 Flash 中的只读数据转存到 SRAM（初始化 `.data` 段），并将未初始化的全局/静态变量区域清零（清空 `.bss` 段）。

4. 进入用户主程序

- **调用 __main()**：跳转至 C 语言运行库的初始化。
- **进入 main()**：最终调用用户编写的 `main()` 函数，正式开始执行用户应用逻辑。

5. 复位流程总结

硬件复位-采样 BOOT-读取 MSP - 读取 PC(`Reset_Handler`) - 执行`SystemInit()`-进入 `main()`

## 2.5 Bootloader

Bootloader（**引导加载程序**）是计算机或嵌入式系统上电后运行的第一段软件代码。

### 2.5.1 核心功能

- **硬件初始化**：设置 CPU 时钟、初始化内存（RAM）、闪存（Flash）及必要的外部设备。
- **加载内核**：将操作系统内核从存储介质（如 `Flash、SD 卡`）复制到运行内存中。
- **跳转执行**：将控制权正式移交给操作系统，随后 Bootloader 退出运行。
- **固件更新**：支持通过串口、网络或 USB 接收新固件，实现系统升级。

### 2.5.2 常见类型

- **嵌入式系统**：**U-Boot**（最通用、支持 ARM/MIPS/X86）、`Barebox`。
- **PC 端**：**GRUB**（Linux 常用）、Windows Boot Manager。
- **移动端**：LK (Little Kernel)、XBL（高通平台常见）。

### 2.5.3 流程步骤
 
第一阶段：上电初始化与模式判定

1. **硬件复位与向量表重定向**
    - 芯片上电，CPU 从固定起始地址（如 0x08000000）读取**栈顶指针（SP）** 和**复位向量（PC）** 。
    - 开始执行 Bootloader 的汇编启动代码，并将中断向量表临时指向 Bootloader 区域。
2. **基础硬件初始化**
    - 关闭全局中断，防止初始化阶段被异常干扰。
    - 清除看门狗定时器（或进行喂狗配置）。
    - 配置系统时钟（RCC），使芯片运行在最高主频。
    - 初始化必要的运行内存（SRAM）。
3. **读取并判定启动标志**
    - **软件标志检查**：读取 `EEPROM、Flash` 特定扇区或备份寄存器（RTC Backup）中的升级状态位（如 `0x55AA` 表示有新固件需更新）。
    - **硬件引脚检查**：检测特定按键或 GPIO 引脚的电平（例如：按下 Boot 键上电则进入升级模式）。
    - **超时等待检查**：开启定时器，通过串口或 USB 监听 2~3 秒。若收到特定握手指令（如 `0x7F`）则进入升级，超时未收到则准备跳转。

第二阶段：正常启动（跳转至 App 流程）

若判定不需要升级，Bootloader 将执行“交接仪式”，把 CPU 控制权完全让给主应用程序（App）：

1. **检查 App 合法性**
    - 读取 App 存放区起始地址（如 0x08008000）的数据。
    - 验证栈顶地址是否合法（通常检查该数据是否落在有效 SRAM 地址范围内）。
    - （可选）对整个 App 区域进行 CRC 校验或数字签名验证，确保固件完整、未损坏 电子工程专辑。
2. **反初始化（De-initialize）外设**
    - 关闭 Bootloader 中开启的所有外设（如定时器、串口、GPIO）。
    - 恢复时钟配置为默认状态。
    - **这一步至关重要**，必须保证给 App 一个干净、纯洁的硬件初始状态，避免外设冲突。
3. **关闭并清理中断**
    - 显式关闭所有使能的 NVIC 中断通道。
    - 清除所有待处理的中断标志位。
4. **执行控制权跳转**
    - 设置主堆栈指针（MSR 指令）：将 CPU 的 SP 指针重置为 App 的栈顶地址。
    - 强制跳转（通过函数指针调用）：将 App 的复位向量地址赋值给程序计数器（PC） 知乎。
    - CPU 正式开始执行 App 代码，App 第一步会重新配置自身的**中断向量表偏移（SCB->VTOR）** CSDN博客。

第三阶段：在线升级启动（IAP 流程）

若第一阶段判定需要升级，Bootloader 将化身为一个“下载器兼烧录器”：

1. **初始化通信外设**
    - 开启通信接口（如 UART、CAN、USB、以太网或 Wi-Fi 模块）。
    - 与上位机（PC 或云端）建立握手连接。
2. **接收擦除指令并执行**
    - 校验下载指令的安全权限。
    - 根据新固件的大小，擦除（Erase）Application 区域对应的 Flash 扇区 电子工程专辑。
3. **分包接收与循环写入**
    - **数据接收**：通过通信接口分包（如每包 1KB 或 256 字节）接收固件数据。
    - **数据校验**：对每包数据进行校验（如校验和、CRC16）。若报错则要求重发。
    - **Flash 写入**：校验无误后，将数据写入到指定的 Flash 地址中 电子工程专辑。
4. **整包校验与状态更新**
    - 固件接收完毕后，对整个 App 区域执行全局 CRC 校验。
    - 校验通过后，修改 EEPROM 或 Flash 中的“升级标志位”为“已完成（Normal）” CNBLOGS。
5. **软复位系统**
    - 触发 CPU 软件复位（如使用 `NVIC_SystemReset`）。系统重启后将回到【第一阶段】，并因为升级标志已清除而直接跳转进新 App 运行 CNBLOGS。

### 2.5.4 `BootLoader`跳转

- 确认主程序固件存在（SP和PC的存在）
- 获取到主程序固件的前2个字
- （关闭所有开启的用户中断） 设置中断向量表
- 分别给SP寄存器赋值，给PC寄存器赋值（没法用C语言实现）
- 通过构造地址函数实现跳转Reset_Handle  

## 2.6 Flash 分区

使用 **STM32F407VGT6（1 MB Flash）**，Flash 从：

```text
0x0800 0000
```

开始，Sector 分布是：

```text
Sector 0   0x0800 0000   16 KB
Sector 1   0x0800 4000   16 KB
Sector 2   0x0800 8000   16 KB
Sector 3   0x0800 C000   16 KB
Sector 4   0x0801 0000   64 KB
Sector 5   0x0802 0000   128 KB
Sector 6   0x0804 0000   128 KB
Sector 7   0x0806 0000   128 KB
Sector 8   0x0808 0000   128 KB
Sector 9   0x080A 0000   128 KB
Sector 10  0x080C 0000   128 KB
Sector 11  0x080E 0000   128 KB
```

整个 1 MB Flash：

```text
0x0800 0000
      ↓
0x080F FFFF
```

需要考虑第一个分区**16 KB 的 Bootloader 到底够不够？** Bootloader 现在已经有：

```text
USART
DMA
RingBuffer
Parser
CMD
Flash操作
```

如果代码越来越多，16 KB 未必宽裕。所以实际学习项目里，给 Bootloader 留 **前 4 个小 Sector**：

```text
Sector 0  16 KB
Sector 1  16 KB
Sector 2  16 KB
Sector 3  16 KB
----------------
总共 64 KB
```

于是：

```text
Bootloader：
0x08000000 ~ 0x0800FFFF

APP：
从 Sector 4 开始
0x08010000
```

也就是：

```text
#define BOOT_START_ADDR  0x08000000U
#define APP_START_ADDR   0x08010000U
```

因为非常清楚：

```text
Sector 0~3
──────────────
永远属于 Bootloader

Sector 4~11
──────────────
属于 APP
```

以后收到CMD_ERASE_APP,Bootloader 就知道我只能擦 Sector 4 往后的区域，绝对不能碰 Sector 0~3。

```text
STM32F407 Flash

0x08000000
┌──────────────────┐
│ Sector 0  16KB   │
├──────────────────┤
│ Sector 1  16KB   │
├──────────────────┤
│ Sector 2  16KB   │
├──────────────────┤
│ Sector 3  16KB   │
├──────────────────┤ ← 0x08010000
│ Sector 4  64KB   │
├──────────────────┤
│ Sector 5 128KB   │
├──────────────────┤
│ Sector 6 128KB   │
├──────────────────┤
│ ...              │
├──────────────────┤
│ Sector 11 128KB  │
└──────────────────┘

Sector 0~3 → Bootloader
Sector 4~11 → APP
```

## 2.7 MSP和中断向量表

在编译/链接 STM32 工程时，**启动文件和链接脚本已经把这些内容放进固件镜像里了**；烧录只是把这个镜像写进 Flash。典型 STM32 固件开头就是中断向量表。最前面两个 32 位数据通常是

```text
Flash 起始地址 + 0x00  → 初始 MSP 值
Flash 起始地址 + 0x04  → Reset_Handler 地址
```

比如 APP 放在0x08010000，那么：

```c
uint32_t app_msp   = *(uint32_t *)0x08010000;
uint32_t app_reset = *(uint32_t *)0x08010004;
```

读出来的就是：

```c
app_msp   → APP 启动后应该使用的主栈顶地址
app_reset → APP 的 Reset_Handler 入口地址
```

在 `startup_stm32f4xx.s` 里一般已经定义好了向量表，逻辑类似：

```c
__Vectors

    DCD     __initial_sp
    DCD     Reset_Handler
    DCD     NMI_Handler
    DCD     HardFault_Handler
    ...
```

这里`DCD __initial_sp`，就是把“初始 MSP 值”放在向量表第 0 项。然后DCD Reset_Handler就是把 Reset_Handler 的地址放在第 1 项。所以真正流程是

```text
写 C 代码
↓
编译
↓
startup 文件 + 代码一起链接
↓
生成完整固件
↓
固件里已经包含向量表
↓
烧录到 Flash
```

CPU 上电复位以后，不是先执行 `main()`，而是硬件先自动干两件事：

```text
1. 读取向量表第0项 → 装入 MSP
2. 读取向量表第1项 → 装入 PC
```

于是 PC 指向Reset_Handler，然后：

```text
Reset_Handler
↓
初始化 .data
↓
清零 .bss
↓
SystemInit()
↓
__main / C runtime
↓
main()
```

在做 Bootloader 跳 APP，本质上就是在**手动模拟 MCU 刚复位时做的关键动作**：

```c
__set_MSP(app_msp);

AppEntry_t app_entry = (AppEntry_t)app_reset;

app_entry();
```

也就是：

```text
把 MSP 换成 APP 的
↓
跳到 APP 的 Reset_Handler
```

所以一句话记**只要这个程序是一个正常的 STM32 可启动镜像，它的向量表里通常就已经带有 MSP 初值和 Reset_Handler 地址。不是烧录时生成，而是链接固件时就已经生成好了。**

## 2.8 修改APP固件的Flash映射地址

**修改 APP 的 Flash 映射地址，以及让它的 Reset_Handler / 向量表跟着新的 APP 起始地址工作**，那核心不是去“改 Reset_Handler 函数本身”，而是改 **APP 的链接地址和向量表重定位**。做Bootloader，最典型的是

```text
Bootloader: 0x08000000 开始
APP:        0x08010000 开始
```

那么 APP 工程要做两件事：

1. 让链接器把 APP 编译到 `0x08010000`
2. 让 CPU 使用 APP 自己的向量表，而不是默认 `0x08000000` 的向量表

在 Keil 里，最直观的是改这里：

```
Options for Target
→ Target
→ IROM1
```

把`Start: 0x08000000`，改成`Start: 0x08010000`。Size 则改成你给 APP 剩下的 Flash 大小。
<img width="782" height="587" alt="image" src="https://github.com/user-attachments/assets/085585a3-c487-4f14-88fe-91998add0ad0" />


例如 STM32F407 1 MB Flash，APP 从 `0x08010000` 开始，那么 APP 可用区域大致是`0x08010000 ~ 0x080FFFFF`。这样链接器就会把:

```text
向量表
Reset_Handler
代码
常量
.data 初值
```

全部按照 `0x08010000` 这个地址重新安排。注意**Reset_Handler 的代码通常不用自己改。** 因为 startup 文件里是DCD Reset_Handler 链接器看到 APP 被链接到新地址后，会自动把 `Reset_Handler` 的实际地址写进 APP 向量表的第二项。所以最终看到的可能是：

```c
0x08010000 -> 0x200xxxxx     MSP
0x08010004 -> 0x08010xxx     Reset_Handler
```

这就是正常的。然后还需要处理向量表重定位。Cortex-M 默认中断向量表基地址通常是`0x08000000`但APP 在`0x08010000`所以 APP 启动后要把`SCB->VTOR`改成 APP 的向量表地址`SCB->VTOR = 0x08010000;`

很多 STM32 工程是在SystemInit();里面处理这个事情。例如可能会看到`#define VECT_TAB_OFFSET  0x00010000U`然后`SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;`因为`FLASH_BASE = 0x08000000`。所以：

```c
0x08000000 + 0x00010000 = 0x08010000
```

这一步非常关键。否则虽然已经跳到 APP 了，但是一旦 USART、SysTick、DMA 等中断触发，CPU 可能仍然去`0x08000000`。附近找中断向量，而不是 APP 的`0x08010000`附近找。

所以 Bootloader + APP 的完整关系可以记成：

```text
Bootloader:
0x08000000
│
├─ 自己的向量表
├─ Reset_Handler
├─ Bootloader代码
│
└─ 跳转到 APP
        ↓

APP:
0x08010000
│
├─ APP MSP
├─ APP Reset_Handler
├─ APP 中断向量表
├─ APP代码
└─ APP数据
```

而 Bootloader 跳转时通常做：

```c
uint32_t app_msp;
uint32_t app_reset;

app_msp = *(uint32_t *)APP_ADDR;
app_reset = *(uint32_t *)(APP_ADDR + 4);

__set_MSP(app_msp);

void (*app_entry)(void);
app_entry = (void (*)(void))app_reset;

app_entry();
```

然后 APP 的 `Reset_Handler` 开始执行。

## 2.9 传输APP固件

通过 USART 发过去的不是“几段普通 C 代码”，而是**已经编译、链接完成的 APP 固件镜像**。也就是说，中断向量表不是单片机在接收到 USART 数据以后“现创建”的。它早就在电脑上生成 APP 固件时，被链接器放进 `.bin/.hex` 里了。比如 APP 链接地址设置成`0x08010000`。那么编译链接后，这个 APP 镜像开头通常已经是

```c
0x08010000  -> MSP 初始值
0x08010004  -> Reset_Handler 地址
0x08010008  -> NMI_Handler
0x0801000C  -> HardFault_Handler
...
```

这些内容来自启动文件，例如：

```c
DCD __initial_sp
DCD Reset_Handler
DCD NMI_Handler
DCD HardFault_Handler
...
```

链接器把它们整理进最终固件。 Bootloader 做的只是：

```text
PC 上的 APP.bin
        ↓ USART
STM32 Bootloader 接收字节
        ↓
写入 Flash 0x08010000 开始的位置
```

假设 APP.bin 前 16 个字节是：

```c
00 00 02 20
A1 23 01 08
...
```

Bootloader 根本不知道“这4个字节是 MSP，这4个字节是 Reset_Handler。”它只是机械地：

```c
FLASH_ProgramByte(addr, byte);
addr++;
```

所以最终这些字节被原封不动写到了：

```c
0x08010000
0x08010001
0x08010002
...
```

结果看起来就像“单片机生成了向量表”，其实不是。准确流程是：

```text
APP源代码
main.c
startup.s
中断函数
        ↓
电脑上的编译器 + 链接器
        ↓
生成完整 APP.bin
其中已经包含：
向量表 + 机器码 + 常量 + .data初始值
        ↓
USART发送
        ↓
Bootloader逐字节接收
        ↓
写入APP Flash区域
```

 **中断向量表属于 APP 固件本身，不是 Bootloader 在下载 APP 时创建的。** 这也是为什么 Bootloader 跳转 APP 时可以直接

```c
app_msp = *(uint32_t *)APP_START_ADDR;
app_reset = *(uint32_t *)(APP_START_ADDR + 4);
```

因为知道APP 镜像的前 8 个字节，本来就是启动文件放进去的 MSP 和 Reset_Handler。再进一步说，如果你乱发一个普通文件，例如hello.txt到 `0x08010000`，Bootloader一样可以把它写进去。但此时`*(uint32_t *)0x08010000`读到的就只是 `"hell"` 对应的二进制数据，并不是真正的 MSP。

这种程序当然也跳不起来。所以 Bootloader 下载程序时其实有一个很重要的隐含前提你发送的必须是一个正确链接到 APP 地址的可执行固件镜像$\boxed{\text{发送的必须是一个正确链接到 APP 地址的可执行固件镜像}}$而不只是“一堆程序数据”。


APP 固件本身就已经包含了主栈地址和中断向量表启动信息，不是单片机后来临时创建的。比如 APP 链接到0x08010000那么这个 APP 固件开头通常已经排好了：

```c
0x08010000 -> MSP 初始值
0x08010004 -> Reset_Handler 地址
0x08010008 -> NMI_Handler
0x0801000C -> HardFault_Handler
...
```

这些信息来自 `startup_xxx.s` 里的中断向量表，再由链接器放进最终的 `.bin/.hex` 固件。所以通过 USART 发 APP 时，本质上是在发

```text
向量表
+
程序机器码
+
常量
+
.data 初始值
+
其他固件内容
```

Bootloader 只是把这些字节原样写进 APP 的 Flash 区域。之后才能

```c
app_msp = *(uint32_t *)APP_START_ADDR;
app_reset = *(uint32_t *)(APP_START_ADDR + 4);
```

因为已经知道APP 固件最前面的第 0 个字就是 MSP，第 1 个字就是 Reset_Handler 地址。**APP 不是只有 `main()`，它其实是一个完整的、可以独立启动的 STM32 程序镜像。** 所以 Bootloader 跳转 APP，本质上就是“找到这个完整 APP 镜像的启动入口，然后把 CPU 控制权交给它”。

## 2.10 跳转APP

跳转的流程

```text
检查APP是否合法
↓
关中断
↓
关闭/清理Bootloader留下的外设和SysTick
↓
设置VTOR到APP向量表
↓
设置MSP为APP自己的栈顶
↓
把0x08010004当函数地址
↓
跳进APP Reset_Handler
```

