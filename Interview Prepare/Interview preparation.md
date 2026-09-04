# 一、先给你一个结论：你需要掌握到什么程度？

对你这个项目，建议达到：

> **项目流程能完整讲 → 每个核心模块能解释实现 → 面试官追问 1~2 层能接住 → 更高级方案知道思想但明确说“项目里没实现”。**

例如：

面试官：

> Flash 怎么规划？

你不能只说：

> “Boot 一块，APP 一块。”

至少要能继续解释：

> 为什么这么分、为什么 APP 要偏移、为什么不能擦到 Bootloader、STM32F407 按 Sector 擦除。

但如果继续问：

> 双 Bank 下如何实现零停机 OTA？

你可以回答思路，然后说：

> “这个我的项目里没有实际实现。”

**这就足够了。**

---

# 二、按你的 GitHub 项目，我认为最可能问的是这 8 大块

你的仓库核心其实就是：

```
STM32F407
   │
   ├── Bootloader
   │
   ├── APP
   │
   ├── Flash 分区
   │
   ├── UART 升级协议
   │
   ├── 数据接收
   │
   ├── CRC / 完整性校验
   │
   ├── APP合法性检查
   │
   └── APP跳转
        ↑
SerialUpgrader 上位机
```

所以面试官大概率从这里问。

---

# 三、第一大高频：你这个 Bootloader 项目是干什么的？

### 面试官

> 介绍一下你的 Bootloader 项目。

你最好不要回答成：

> “就是通过串口烧程序。”

应该这样讲：

> 我这个项目是在 STM32F407 上实现一个 IAP Bootloader。MCU 上电以后先运行 Bootloader，Bootloader 根据升级条件决定是进入升级模式还是跳转 APP。
> 
> 升级时由 PC 端 SerialUpgrader 通过串口发送固件，MCU 按自定义协议接收数据，对 APP 所在 Flash 区域进行擦除和写入，并进行完整性校验。
> 
> 固件有效以后，Bootloader 检查 APP 向量表，设置栈指针和中断向量表位置，再跳转到 APP 的 Reset_Handler。
> 
> 如果 APP 不合法，就不进行跳转，而是保留在 Bootloader 中等待重新升级。

这基本就是你的项目主线。仓库描述本身也明确覆盖了 Flash 分区、串口协议、异步数据接收、完整性校验和安全跳转这些功能。

---

# 四、面试官：为什么需要 Bootloader？

这个非常高频。

### 你回答

> 如果没有 Bootloader，正常开发的时候我们一般通过 ST-Link 直接烧写 Flash。
> 
> 但产品部署以后，不可能要求用户每次拿 ST-Link 更新程序。
> 
> 所以 Bootloader 常驻 Flash，可以通过 UART、CAN、USB 等通信接口接收新的 APP，实现 IAP 在线升级。
> 
> 另外因为 Bootloader 和 APP 分开，即使 APP 损坏，只要 Bootloader 本身正常，仍然有机会重新下载 APP。

这里重点：

> **Bootloader 的意义不只是启动 APP，还有升级和恢复。**

---

# 五、面试官：你的 Flash 怎么规划？

这个确实是你昨天被问到那类问题。

你的回答应该有三个层次。

## 第一层：为什么要分区

> Bootloader 和 APP 是两个独立程序，所以必须放在不同 Flash 地址，否则会发生覆盖。

大概：

```
0x08000000
┌────────────────┐
│ Bootloader     │
├────────────────┤
│ APP            │
│                │
├────────────────┤
│ 参数/状态区域   │
└────────────────┘
```

---

## 第二层：为什么 Bootloader 从 0x08000000 开始？

因为 STM32 正常从 Flash 启动时：

```
0x08000000 → 初始 MSP
0x08000004 → Reset_Handler
```

所以芯片复位以后首先进入的是 Bootloader。

你现在正好学过 Startup，所以这里必须能关联起来。

---

## 第三层：APP 为什么不是紧接着随便放？

这是你该掌握的地方。

STM32F407 Flash：

> 不是随便按一个字节一个字节擦的，而是按 Sector 擦除。

所以分区通常会考虑：

> **Sector 边界。**

面试可以说：

> 我规划时首先保证 Bootloader 和 APP 独立，然后根据 STM32F407 的 Flash Sector 分布确定边界。因为 Flash 擦除按 Sector 进行，如果 APP 和 Bootloader 共用同一个 Sector，升级 APP 时就可能把 Bootloader 一起擦掉。

这比死背某个地址重要得多。

---

# 六、面试官：为什么 APP 需要改地址？

这题你一定会遇到。

假设：

```
Bootloader = 0x08000000
APP        = 0x08010000
```

### 回答

> 因为 APP 已经不是整个 Flash 的第一个程序了。
> 
> Bootloader 占用了 Flash 起始位置，所以 APP 工程必须修改链接地址，让链接器按照新的 Flash 起始地址组织代码。
> 
> 如果 APP 仍然按照 0x08000000 链接，即使把 bin 强行写到后面的地址，它内部的函数地址和向量表地址仍然可能不正确。

这里把一句话背住：

> **不是烧写时随便把 APP 搬过去，而是链接阶段 APP 就必须知道自己运行在哪。**

---

# 七、面试官：APP 指针指不到怎么办？

你提到的这个问题，其实面试官真正想问的是：

> **你凭什么知道这里真的有一个合法 APP？**

这就是核心。

假设：

```
APP_ADDR = 0x08010000;
```

APP 向量表：

```
APP_ADDR
    ↓
MSP

APP_ADDR + 4
    ↓
Reset_Handler
```

所以 Bootloader 会先读：

```
uint32_t app_stack =
    *(uint32_t *)APP_ADDR;

uint32_t app_reset =
    *(uint32_t *)(APP_ADDR + 4);
```

---

## 第一件事：判断 MSP

正常 APP 的 MSP：

> 应该落在 SRAM。

例如：

```
0x200xxxxx
```

如果读出来：

```
0xFFFFFFFF
```

说明：

> 这个区域可能是空 Flash。

那肯定不能跳。

---

## 第二件事：判断 Reset_Handler

Reset_Handler：

> 应该指向 APP Flash 的合法代码区域。

不能是：

```
0xFFFFFFFF
```

也不能乱飞到 RAM 或错误地址。

---

## 所以你可以回答

> APP 跳转前我不会直接拿一个函数指针就跳，而是先判断 APP 的向量表是否合理。
> 
> 首先检查首地址保存的 MSP 是否落在有效 SRAM 范围，然后检查第二个 word，也就是 Reset_Handler 是否落在 APP Flash 区域。
> 
> 如果地址不合法，就认为 APP 不存在或者已经损坏，Bootloader 不进行跳转，而是停留在升级模式等待重新下载。

这个就是你昨天举的：

> “APP 指针指不到怎么办？”

非常标准的回答。

---

# 八、面试官：为什么 APP 第一个地址是栈指针？

这特别适合追问你最近 Startup 学习。

### 回答

> Cortex-M 的中断向量表第一个 word 就是初始 MSP，第二个 word 是 Reset_Handler。
> 
> MCU 正常复位时，硬件会自动把第一个 word 装载到 MSP，再把第二个 word 装载到 PC。
> 
> 但是 Bootloader 跳 APP 并不是真正执行了一次硬件复位，所以这个过程需要 Bootloader 自己模拟。

于是：

```
硬件复位：

Flash[0] → MSP
Flash[1] → PC
```

而 Boot 跳 APP：

```
读取 APP[0]
 ↓
__set_MSP()

读取 APP[1]
 ↓
函数指针
 ↓
Jump
```

---

# 九、面试官：为什么要跳 Reset_Handler，不直接跳 main？

这也是你这个项目应该掌握的。

### 回答

> 因为 APP 的 main 并不是程序真正的第一条执行入口。
> 
> 正常情况下 APP 会先执行 Reset_Handler，在里面完成 C 运行环境初始化，比如 `.data` 搬运、`.bss` 清零、系统时钟初始化等，然后才进入 main。
> 
> 如果直接跳 main，可能导致全局变量、静态变量甚至运行环境没有正确初始化。

所以：

```
Boot
 ↓
APP Reset_Handler
 ↓
SystemInit
 ↓
.data
 ↓
.bss
 ↓
main
```

不是：

```
Boot → main
```

---

# 十、面试官：为什么还要设置 VTOR？

这是这个项目里我认为**必须掌握**的一题。

因为 APP 原来向量表应该在：

```
0x08000000
```

现在 APP 被移动到：

```
0x08010000
```

那么：

```
SCB->VTOR = APP_ADDR;
```

就是告诉 Cortex-M：

> 后续中断请从 APP 这里找向量表。

### 面试回答

> 修改 APP 链接地址只解决了程序被放到哪个 Flash 地址的问题。
> 
> 因为 APP 的中断向量表也跟着发生了偏移，所以还必须把 VTOR 指向 APP 的向量表。
> 
> 否则 APP 虽然可能正常进入 main，但是 UART、SysTick、Timer 等中断一触发，CPU 仍可能去 Bootloader 的向量表找中断入口，造成异常甚至 HardFault。

这一题是**非常可能出现的第二层追问**。

---

# 十一、面试官：你自己的串口协议怎么设计？

你的仓库明确写了“自定义串口协议”，所以这个非常危险：

> 简历写了，就一定要能回答。

你至少应该说清楚一个通信帧：

```
帧头
命令
长度
序号
Payload
校验
```

例如：

```
| HEAD | CMD | LEN | DATA | CRC |
```

面试回答：

> 上位机和 Bootloader 之间不是直接裸传 bin，而是定义通信帧。
> 
> 帧里会有帧头用于同步，命令字段区分擦除、写入、结束等操作，长度字段描述 Payload 大小，最后有校验字段。
> 
> Bootloader 接收到完整帧以后先进行协议解析和校验，通过之后才进行 Flash 操作，并通过 ACK/NACK 告诉上位机成功还是失败。

然后面试官可能问：

> 为什么需要帧头？

回答：

> 为了在连续字节流中找到一帧的起点，尤其发生丢字节或者接收错位以后，可以重新同步。

---

# 十二、面试官：为什么需要包序号？

如果你的协议里有类似东西，要会解释。

> 主要用来判断重复包、丢包和顺序错误。
> 
> 比如 MCU 已经正确接收第 10 包但 ACK 丢失，上位机会重新发第 10 包，如果没有序号，Bootloader 可能把同一段数据重复写两次。

这个回答已经很工程化了。

---

# 十三、面试官：为什么不是 UART 收到一个字节就写 Flash？

这题非常容易问。

### 回答

> UART 是字节流，而 Flash 写操作相比 RAM 慢，而且还有固定的编程粒度。
> 
> 所以我会先把串口数据接收到 RAM buffer 中，形成一个完整的数据包，完成协议校验以后，再把有效 Payload 写入 Flash。
> 
> 这样既方便做 CRC，也能减少频繁 Flash 写操作。

流程：

```
UART
 ↓
RAM Buffer
 ↓
完整帧
 ↓
CRC
 ↓
Flash
```

---

# 十四、你仓库写了“异步数据接收”，面试官很可能问这个

仓库介绍里明确有 **异步数据接收**。

所以面试官可能问：

> 什么叫异步？你怎么接收？

你要避免回答：

> “UART 本身就是异步串口，所以叫异步。”

这里很可能指的是程序设计上的：

> **接收和处理解耦。**

典型思想：

```
UART IRQ / DMA
       ↓
   RX Buffer
       ↓
设置接收标志
       ↓
主循环解析数据
```

例如：

```
void USART_IRQHandler(void)
{
    rx_buf[index++] = USART_ReceiveData(...);

    // 不在这里擦 Flash
}
```

主循环：

```
if (frame_ready)
{
    ParseFrame();
    FlashWrite();
}
```

你可以回答：

> 中断里尽量只完成数据搬运和状态更新，不进行比较耗时的 Flash 擦写和复杂协议解析，把这些处理留到主循环或者状态机中，这样可以缩短 ISR 时间，降低丢数据风险。

这个答案特别适合你的项目。

---

# 十五、面试官：为什么不能在串口中断里直接擦 Flash？

回答：

> Flash 擦除耗时相对比较长。
> 
> 如果在 UART ISR 中直接擦 Flash，中断执行时间会非常长，会阻塞其他中断，同时新的串口数据可能继续到来，增加丢包风险。
> 
> 所以 ISR 更适合快速收数据，然后通知主流程处理。

这就把：

```
UART
中断
Flash
实时性
```

串起来了。

---

# 十六、面试官：你的 Flash 写入流程是什么？

回答：

```
收到升级命令
 ↓
判断固件信息
 ↓
解锁 Flash
 ↓
擦除 APP Sector
 ↓
接收数据
 ↓
校验数据包
 ↓
写 APP Flash
 ↓
地址递增
 ↓
全部接收完
 ↓
完整性校验
 ↓
Lock Flash
```

关键：

> **只擦 APP，不擦 Bootloader。**

---

# 十七、面试官：Flash 为什么必须先擦？

你至少掌握：

```
擦除后：
11111111

编程：
1 → 0
```

而通常不能直接：

```
0 → 1
```

因此如果已有旧程序：

> 必须擦除对应 Sector，再写新程序。

---

# 十八、面试官：你怎么知道写 Flash 没越界？

这个是非常典型工程追问。

假设：

```
flash_addr = APP_START;
```

每收到一包：

```
flash_addr += packet_len;
```

但是应该判断：

```
if (flash_addr + len > APP_END)
{
    error;
}
```

### 回答

> 在 Flash 写函数前做地址合法性检查，目标地址只能位于 APP 分区范围。固件总长度也要先判断不能超过 APP 最大空间。
> 
> 这样即使上位机传入错误长度，也不能把数据写进 Bootloader 区或者其他保留区域。

---

# 十九、你的项目明确写了“固件完整性校验”，所以 CRC 必须会

仓库说明明确包含“固件完整性校验”。

面试官：

> CRC 用来干嘛？

不要说：

> “保证数据正确。”

更准确：

> CRC 用于检测固件在串口传输或 Flash 写入过程中有没有发生数据错误。
> 
> MCU 对接收到的数据按照相同算法重新计算 CRC，与上位机发送的结果比较，如果不一致就认为数据损坏。

---

# 二十、面试官：CRC 和 checksum 有什么区别？

简单回答即可：

> checksum 通常是简单累加，计算简单但检错能力有限；
> 
> CRC 是基于多项式除法，能够更有效检测突发错误，因此通信和固件校验里更常用。

不用给我背 CRC 数学推导。

你的项目阶段完全没必要。

---

# 二十一、面试官：整包 CRC 通过是不是代表 APP 一定能运行？

答案：

> 不是。
> 
> CRC 只能说明接收到的数据和发送端预期的数据一致，不能证明这个固件本身没有逻辑 bug。
> 
> 所以启动 APP 时还要做最基本的向量表合法性检查，例如 MSP 和 Reset_Handler 地址范围。

这个回答很好。

---

# 二十二、面试官：升级过程中突然断电怎么办？

这一题你要根据**你现在这个项目真正做到的层级**回答。

别说自己做了 A/B，如果没做。

可以回答：

> 如果是在擦除或者写入 APP 的过程中断电，APP 很可能已经不完整。
> 
> 下一次上电 Bootloader 会重新检查 APP 的有效性和完整性，如果校验失败就不跳 APP，而是保留在升级模式，等待上位机重新发送固件。
> 
> 我目前项目主要做到这个级别。如果产品要求升级过程断电后仍保留旧程序，则可以进一步做 A/B 双分区，这部分我目前没有实际实现。

这个回答我非常建议你背。

因为它：

> **不装，但知道下一层工程方案。**

---

# 二十三、面试官：你所谓“安全跳转”安全在哪里？

你的 GitHub 里面用了“Bootloader/APP 安全跳转”这个描述，所以面试官可能抓这个词。

可以回答：

> 这里的“安全跳转”主要不是指安全加密，而是指跳转前保证运行环境是正确的。
> 
> 包括检查 APP 向量表是否合法，关闭或清理 Bootloader 使用的中断和 SysTick，设置 APP 的 VTOR 和 MSP，然后跳 APP Reset_Handler。
> 
> 这样避免带着 Bootloader 的运行状态直接进入 APP。

这点特别重要：

**不要把“安全跳转”说成 Secure Boot。**

不是一个概念。

---

# 二十四、面试官：为什么跳 APP 前要处理 SysTick？

你可以回答：

> Bootloader 如果使用了 SysTick，直接跳 APP 后 SysTick 可能仍然保持 Bootloader 的配置，而且可能已经存在 pending interrupt。
> 
> 因此跳转前通常停止 SysTick 并清理相关中断状态，让 APP 自己重新初始化。

够用了。

---

# 二十五、面试官：为什么跳 APP 前要关中断？

回答：

> 因为 Bootloader 和 APP 的中断向量表不同。
> 
> 在 VTOR、MSP 和运行环境切换过程中如果突然发生中断，CPU 可能执行错误的中断处理程序，所以跳转过程需要避免被打断。

---

# 二十六、面试官：你上位机 SerialUpgrader 做什么？

你的仓库确实把 `SerialUpgrader` 独立出来了。

所以你至少要能讲：

> PC 端负责读取 bin 固件，根据自定义升级协议进行分包，通过串口发送给 STM32，并接收 Bootloader 的响应。
> 
> 升级过程大致包括建立连接、发送升级命令、发送固件信息、分包传输、处理 ACK/NACK、完成校验以及结束升级。

如果他问：

> 为什么需要上位机？

回答：

> Bootloader 只负责 MCU 端协议和 Flash 操作，而 bin 文件读取、分包、重传、进度显示等更适合放到 PC 端。

---

# 二十七、面试官：bin 文件是什么？

这个你也应该会。

> bin 是编译链接以后生成的纯二进制机器码镜像。
> 
> 相比 ELF，它不包含符号表和调试信息，非常适合直接作为固件数据写入 Flash。

再追问：

> bin 里面有没有 Flash 起始地址？

回答：

> 单纯的 bin 文件没有像 HEX 那样携带地址记录，所以写到哪里需要升级程序和 Bootloader 根据约定确定。

这个很容易被问。

---

# 二十八、面试官：你的项目为什么叫 IAP？

回答：

> IAP 是 In-Application Programming，也就是 MCU 已经运行了一套程序，再由这套程序对片内 Flash 的另外区域重新编程。
> 
> 我的 Bootloader 常驻 Flash，运行后通过串口接收 APP 数据并写入 APP 区，因此属于典型的 IAP 场景。


# 1. 你先把整个 Bootloader 流程背成这一条

面试官：

> 你这个 Bootloader 整体是怎么做的？

你可以回答：

> 我的 Bootloader 和 APP 是分别放在 STM32 Flash 的不同区域。上电以后 MCU 首先从 Bootloader 的地址启动，Bootloader 初始化串口等必要外设，然后判断是否需要升级。
> 
> 如果需要升级，就通过 UART 接收新的 APP 固件，对 APP 所在的 Flash 区域进行擦除，然后分包写入，写完之后进行 CRC 或其他完整性校验。
> 
> 如果 APP 有效，就读取 APP 起始地址的前两个字：
> 
> 第一个字是 APP 的 MSP 初始值，第二个字是 Reset_Handler 地址。
> 
> Bootloader 设置 MSP，并跳转到 APP 的 Reset_Handler。APP 同时需要修改自己的链接地址和中断向量表地址，保证程序和中断都在 APP 区域正常运行。

你脑子里应该有：

```
复位
 ↓
Bootloader
 ↓
初始化必要外设
 ↓
是否升级？
 ├─ 是 → UART接收固件
 │       ↓
 │     擦除APP区
 │       ↓
 │     写Flash
 │       ↓
 │     CRC校验
 │       ↓
 │     APP有效
 │
 └─ 否
      ↓
 判断APP是否合法
      ↓
 设置VTOR/MSP
      ↓
 跳转Reset_Handler
      ↓
 APP main()
```

这就是整个项目的主线。

---

# 2. 面试官：你的 Flash 是怎么规划的？

这是你非常应该准备的问题。

比如以 STM32F407 为例，假设是 1 MB Flash：

```
Flash 起始地址：
0x08000000
```

你可以规划：

```
0x08000000
┌───────────────────┐
│   Bootloader      │
│   例如 64 KB      │
├───────────────────┤
│   APP             │
│                   │
│                   │
├───────────────────┤
│ 参数区 / Flag区   │
└───────────────────┘
```

例如：

```
Bootloader：
0x08000000 ~ 0x0800FFFF

APP：
0x08010000 ~ ...

参数区：
Flash 最后若干 Sector
```

面试回答：

> 我会先根据芯片 Flash 容量和 Sector 分布来规划，而不是随便按照字节切。
> 
> Bootloader 一般放在 Flash 起始地址，因为芯片复位以后默认从 0x08000000 获取初始 MSP 和 Reset_Handler。
> 
> 然后给 Bootloader 预留固定大小，比如 64 KB，APP 从 0x08010000 开始。
> 
> 如果项目需要升级状态、版本号或者 CRC，可以再预留一个独立的参数区。
> 
> 划分时还需要考虑 STM32F4 的 Flash 是按 Sector 擦除，因此 APP 起始地址最好和 Sector 边界对齐，避免擦 APP 时误擦 Bootloader。

这里有个非常重要的点：

## STM32F407 的 Flash 擦除不是“按字节”

而是：

> **按 Sector 擦除。**

所以你不能说：

```
我要从 0x08008005 擦到 0x0800A053
```

Flash 擦除的物理单位是 Sector。

这很容易被继续追问。

---

# 3. 面试官：为什么 APP 不能还是放在 0x08000000？

你回答：

> 因为 0x08000000 已经被 Bootloader 占用了。
> 
> APP 如果还链接到 0x08000000，那么 APP 的代码、向量表和 Bootloader 会发生地址冲突。
> 
> 所以 APP 工程必须修改链接脚本或者 Keil 的 IROM 起始地址，让 APP 从指定地址，例如 0x08010000 开始链接。

例如原来：

```
FLASH:
0x08000000
```

改成：

```
FLASH:
0x08010000
```

本质不是：

> “程序下载到了别的位置。”

而是：

> **编译阶段就告诉链接器，我的代码运行地址是 0x08010000。**

这点一定要讲出来。

---

# 4. 面试官：APP 起始地址里面到底放的是什么？

这个你最近正好学 Startup，可以直接连接起来。

假设：

```
APP_ADDR = 0x08010000
```

那么：

```
0x08010000
    ↓
APP 初始 MSP

0x08010004
    ↓
Reset_Handler 地址

0x08010008
    ↓
NMI_Handler

0x0801000C
    ↓
HardFault_Handler
...
```

也就是：

> APP 起始地址实际上就是 APP 的中断向量表。

所以：

```
*(uint32_t *)APP_ADDR
```

读出来的是：

> APP 初始栈顶地址。

而：

```
*(uint32_t *)(APP_ADDR + 4)
```

是：

> APP Reset_Handler 的地址。

这和你之前问的 Startup 中：

```
DCD __initial_sp
DCD Reset_Handler
DCD NMI_Handler
...
```

是完全对应的。

这也是你一定要打通的知识点。

---

# 5. 面试官：你是怎么跳转到 APP 的？

标准套路：

```
typedef void (*pFunction)(void);

uint32_t JumpAddress;
pFunction JumpToApplication;

if (((*(__IO uint32_t*)APP_ADDR) & 0x2FFE0000) == 0x20000000)
{
    JumpAddress = *(__IO uint32_t*)(APP_ADDR + 4);

    JumpToApplication = (pFunction)JumpAddress;

    __set_MSP(*(__IO uint32_t*)APP_ADDR);

    JumpToApplication();
}
```

你不要只会背代码。

你必须会解释。

第一步：

```
*APP_ADDR
```

拿：

> APP MSP。

第二步：

```
*(APP_ADDR + 4)
```

拿：

> APP Reset_Handler。

第三步：

```
__set_MSP(...)
```

把：

> 当前 CPU 的主栈指针切换成 APP 自己的栈。

第四步：

```
JumpToApplication();
```

本质就是：

> 把 Reset_Handler 地址转换成函数指针，然后执行。

所以你面试可以说：

> Bootloader 跳 APP 本质上就是模拟一次 MCU 复位后的启动过程：重新设置 APP 的 MSP，然后跳到 APP Reset_Handler。

这句话很好用。

---

# 6. 面试官：为什么必须设置 MSP？

这是高频追问。

你回答：

> 因为 Bootloader 和 APP 是两个独立链接的程序，各自有自己的栈空间。
> 
> MCU 上电时，本来会自动从中断向量表第一个 word 加载 MSP。
> 
> 但是 Bootloader 跳 APP 并不是真正的硬件复位，所以 CPU 不会自动重新加载 APP 的 MSP。
> 
> 因此需要 Bootloader 手动读取 APP 向量表第一个 word，并使用 `__set_MSP()` 设置 MSP。

这就是非常标准的回答。

---

# 7. 面试官：你说“判断 APP 指针有效”，具体怎么判断？

这个就是你说的：

> “如果 APP 指针指不到怎么办？”

首先要把“APP 指针”拆成两个。

APP 开头：

```
第一个 word → MSP
第二个 word → Reset_Handler
```

所以至少检查两件事。

## ① 检查 MSP 是否落在 SRAM

例如 STM32F407 SRAM 起始：

```
0x20000000
```

所以常见代码：

```
if (((*(uint32_t *)APP_ADDR) & 0x2FFE0000) == 0x20000000)
```

本质是：

> 判断 APP 的初始栈地址是不是 SRAM 地址。

更清晰的工程写法：

```
uint32_t app_msp = *(uint32_t *)APP_ADDR;

if (app_msp >= SRAM_START &&
    app_msp <= SRAM_END)
{
    // MSP 合法
}
```

---

## ② 检查 Reset_Handler 是否落在 APP Flash

```
uint32_t reset_handler =
    *(uint32_t *)(APP_ADDR + 4);
```

然后判断：

```
APP_START <= reset_handler < APP_END
```

需要注意 Cortex-M 的函数地址通常：

```
bit0 = 1
```

表示 Thumb 状态。

所以实际地址可能长这样：

```
0x080105A9
```

真正代码地址是：

```
0x080105A8
```

因此可以：

```
if ((reset_handler & 1) == 0)
{
    // 非法
}
```

这是一个比较容易拉开面试差距的点。

---

# 8. 如果 APP 指针指不到怎么办？

你可以回答：

> 如果 APP MSP 或 Reset_Handler 校验失败，我不会直接跳转，否则很可能进入 HardFault。
> 
> 我会认为 APP 当前无效，让 Bootloader 留在升级模式，例如持续监听 UART，等待上位机重新发送固件。
> 
> 工程上还可以结合 CRC、Magic Number、固件长度和版本号判断 APP 是否有效，而不是只判断 MSP 地址。

例如：

```
Boot启动
 ↓
检查Magic
 ↓
检查固件长度
 ↓
检查CRC
 ↓
检查MSP
 ↓
检查Reset_Handler
 ↓
全部通过
 ↓
Jump APP
```

如果失败：

```
不跳APP
 ↓
进入升级等待
```

这就是很完整的回答。

---

# 9. 面试官：为什么只检查 MSP 不够？

回答：

> MSP 在 SRAM 范围只能说明 APP 起始位置看起来像一个合法向量表，但不代表整个 APP 固件是完整的。
> 
> Flash 中即使有一段随机数据，第一个 word 恰好也可能落在 SRAM。
> 
> 所以真实工程一般还会增加 Magic Number、固件长度、版本号以及 CRC 校验。

例如固件信息：

```
typedef struct
{
    uint32_t magic;
    uint32_t version;
    uint32_t length;
    uint32_t crc32;
} firmware_info_t;
```

Bootloader 判断：

```
Magic 对不对
+
length 是否合理
+
CRC 是否匹配
+
MSP 是否合理
+
Reset_Handler 是否合理
```

才跳。

---

# 10. 面试官：APP 下载一半断电怎么办？

这个问题非常重要。

你不能回答：

> 再重新下载。

要分两个层级。

### 你项目当前简单版本

你可以说：

> 我的项目里 APP 区被擦除以后，如果下载一半断电，APP 会变成无效状态。
> 
> 再次启动后 Bootloader 校验 APP 失败，因此不会跳转 APP，而是继续留在升级模式，等待重新下载。

这就已经合理。

然后补一句：

> 如果做更完整的产品，我会使用 A/B 分区或者双 Bank 升级避免这个问题。

这句话非常加分。

---

# 11. 什么叫 A/B 分区？

例如：

```
Bootloader

APP_A

APP_B

Config
```

当前运行：

```
APP_A
```

升级的时候：

```
写 APP_B
```

写完校验：

```
CRC OK
```

再修改启动标志：

```
active = B
```

下一次：

```
Boot → APP_B
```

如果 APP_B 启动失败：

```
回滚 APP_A
```

这就是 OTA / IAP 产品常用思想。

---

# 12. 面试官：Bootloader 擦除 Flash 擦的是谁？

这个你之前问过。

答案非常明确：

> **擦的是 APP 所在的 Flash 区，不是整个 Flash。**

比如：

```
Boot:
Sector 0~3

APP:
Sector 4~...

参数：
最后一个 Sector
```

升级时：

```
FLASH_EraseSector(APP_SECTOR, ...);
```

只擦 APP 对应 Sector。

绝对不能：

```
把 Bootloader 自己擦掉
```

否则 Bootloader 下一次复位就没了。

---

# 13. 面试官：为什么 Flash 写之前通常需要先擦？

因为 Flash 的物理特性：

> Flash 编程通常只能把 bit 从 `1 -> 0`。

擦除以后：

```
0xFFFFFFFF
```

如果想重新：

```
0 → 1
```

不能直接写回来。

必须：

```
Erase
```

把整个 Sector 恢复为：

```
11111111...
```

然后重新编程。

所以：

```
Erase:
0 → 1

Program:
1 → 0
```

这是经典八股。

---

# 14. 面试官：升级时 Bootloader 怎么接收数据？

结合你的 UART：

你可以说：

> 上位机通过 UART 分包发送固件，每个包包含包头、长度、序号、Payload 和 CRC。
> 
> Bootloader 解析数据包，校验通过以后写入对应 Flash 地址，并返回 ACK；如果 CRC 错误则返回 NACK，让上位机重传。

例如：

```
| Header |
| CMD    |
| Seq    |
| Length |
| Data   |
| CRC    |
```

Boot：

```
UART RX
 ↓
解析帧
 ↓
CRC校验
 ↓
写Flash
 ↓
ACK
```

这是非常典型的 Bootloader 通信设计。

---

# 15. 面试官：为什么不能 UART 收一个字节就立刻写一个字节到 Flash？

可以从两个方面回答：

> 第一，Flash 编程有最小编程粒度，而且写 Flash 的速度比 RAM 操作慢。
> 
> 第二，如果 UART 一个字节一个字节直接写，效率很低，也不好做完整包 CRC 和重传。
> 
> 所以一般先在 RAM 中用 buffer 接收一帧，例如 256 Byte 或 1 KB，校验完整以后再批量写 Flash。

例如：

```
uint8_t rx_buf[256];
```

过程：

```
UART
 ↓
RAM Buffer
 ↓
CRC
 ↓
Flash
```

---

# 16. 面试官：Flash 写到哪一个地址？

维护：

```
uint32_t flash_write_addr = APP_ADDR;
```

收到一包：

```
Flash_Write(flash_write_addr, data, len);

flash_write_addr += len;
```

但是必须判断：

```
flash_write_addr + len <= APP_END
```

否则会越界。

否则可能：

> 写到参数区甚至其他程序区域。

所以工程上必须做：

```
地址范围检查
```

---

# 17. 面试官：APP 跳转前为什么经常要关中断？

假设 Bootloader 开了：

```
SysTick
UART IRQ
TIM IRQ
```

此时直接：

```
Jump APP
```

刚跳 APP 的过程中突然：

```
UART IRQ
```

CPU 可能还是按照 Bootloader 的环境处理。

甚至在 VTOR 切换前后发生中断。

所以跳转前通常：

```
__disable_irq();
```

然后：

```
停止 SysTick
清除 pending IRQ
关闭使用过的外设
设置 VTOR
设置 MSP
Jump
```

APP 初始化完成以后再：

```
__enable_irq();
```

---

# 18. 面试官：为什么要修改 VTOR？

这是 Bootloader 必考题。

正常 STM32：

```
向量表：
0x08000000
```

但是 APP：

```
0x08010000
```

APP 的：

```
USART1_IRQHandler
TIM_IRQHandler
SysTick_Handler
```

都在：

```
APP_VECTOR_TABLE
```

如果 VTOR 还是：

```
0x08000000
```

中断发生以后 CPU 会去：

> Bootloader 向量表找 ISR。

这就错了。

所以要：

```
SCB->VTOR = APP_ADDR;
```

或者 APP 自己启动的时候：

```
SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
```

所以你可以总结：

> 修改链接地址解决的是“APP 代码放在哪里”，修改 VTOR 解决的是“APP 中断向量表去哪里找”。

这句话很适合面试。

---

# 19. 面试官：如果忘记修改 VTOR，会发生什么？

非常经典。

回答：

> 如果 APP 只运行普通主循环，可能暂时看不出问题。
> 
> 但是一旦 UART、TIM、SysTick 等中断触发，CPU 仍然会根据原来的 VTOR 到 Bootloader 的向量表取中断入口。
> 
> 轻则执行错误的中断函数，重则跳到无效地址导致 HardFault。

这一题非常值得背。

---

# 20. 面试官：Bootloader 和 APP 是两个 main 吗？

是。

你回答：

> 是，它们本质上是两个独立工程，都有自己的 startup、main、链接脚本和中断向量表。
> 
> MCU 上电最先运行 Bootloader 的 Reset_Handler，然后进入 Bootloader main。
> 
> Bootloader 选择启动 APP 后，再人为设置 APP 的 MSP 并跳转 APP Reset_Handler，APP 的启动代码完成 `.data` 搬运、`.bss` 清零等初始化，最后进入 APP main。

这个回答非常适合你。

因为它把你最近学的 Startup 串起来了。

---

# 21. 面试官：APP Reset_Handler 进去以后做什么？

你可以直接回答：

> Reset_Handler 会完成 C 运行环境初始化，例如把 `.data` 段的初始值从 Flash 拷贝到 RAM，把 `.bss` 清零，还可能调用 SystemInit、C/C++ runtime 初始化，最终调用 main。

所以：

```
Bootloader
 ↓
Jump Reset_Handler
 ↓
SystemInit
 ↓
.data copy
 ↓
.bss clear
 ↓
main()
```

注意：

> 不是 Bootloader 直接跳 APP main。

一般应该：

> 跳 APP Reset_Handler。

---

# 22. 面试官：能不能直接跳 APP main？

理论上你可能找到 main 地址硬跳。

但是不要这么设计。

回答：

> 不建议，因为直接进入 main 会绕过 APP 的启动代码，导致 MSP、`.data`、`.bss`、SystemInit 等初始化可能没有完成。
> 
> 正确做法应该模拟正常复位过程，设置 APP MSP，然后跳到 APP 的 Reset_Handler。

这个答案很标准。

---

# 23. 面试官：怎么判断升级完成？

可以设计：

```
Flash 写入完成
 ↓
检查接收长度
 ↓
计算 CRC
 ↓
CRC == 固件CRC
 ↓
写 upgrade_success flag
```

然后：

```
reset
```

Boot 再：

```
检查APP
 ↓
Jump
```

而不是：

> 收到最后一个 UART 包就默认升级成功。

---

# 24. 面试官：CRC 是对什么算？

推荐回答：

> CRC 应该对整个有效 APP 镜像计算，而不是只对通信帧计算。
> 
> 每个 UART 数据包可以有自己的 Packet CRC，用于发现传输错误；所有包写完以后，再计算整个 APP 镜像 CRC，与固件头中的 CRC 比较，用于确认 Flash 中最终固件完整。

也就是两层：

```
Packet CRC
    ↓
保证通信包正确

Firmware CRC
    ↓
保证整个APP正确
```

这个回答会比“用了 CRC”专业很多。

---

# 25. 面试官：CRC 对了是不是程序就一定能运行？

不是。

CRC 只能证明：

> 数据和预期一致。

不能证明：

> 程序逻辑没 bug。

所以可以回答：

> CRC 只能验证固件传输和存储完整性，不能保证程序逻辑正确。
> 
> 产品级方案还会结合版本信息、签名认证、启动 watchdog、Boot Success Flag 或回滚机制。

---

# 26. 面试官：你的 Bootloader 会不会被 APP 覆盖？

你回答：

> 正常不会，因为 Flash 分区固定，Bootloader 写 Flash 的函数会做地址边界检查，只允许写 APP 区域。
> 
> APP 文件长度也必须校验，超过 APP 最大空间会直接拒绝升级。

比如：

```
if (addr < APP_START ||
    addr + len > APP_END)
{
    return ERROR;
}
```

---

# 27. 面试官：如果上位机给你一个超大的 bin 怎么办？

答案：

> 收固件之前读取固件长度，先判断：
> 
> `firmware_size <= APP_MAX_SIZE`
> 
> 如果超过，就直接拒绝升级。
> 
> 写 Flash 的时候还会再次检查目标地址，避免因为协议错误或者异常数据导致越界写。

这叫：

> 防御性设计。

---

# 28. 面试官：bin 和 hex 有什么区别？

Bootloader 项目很可能问。

你回答：

> `.bin` 是纯二进制镜像，没有地址等额外信息，因此写入时 Bootloader 自己知道起始地址，比如从 APP_ADDR 顺序写。
> 
> `.hex` 通常是 Intel HEX 格式，是文本格式，每条记录里面包含地址、长度、类型和校验信息。
> 
> 嵌入式升级协议里常见传输 bin，因为格式简单、数据量小。

---

# 29. 面试官：为什么 Bootloader 一般做得尽可能小？

你回答：

> Bootloader 本身主要负责启动、升级、校验和恢复，功能越复杂越容易引入 bug，而且占用更多 Flash。
> 
> Bootloader 一旦损坏，系统可能连升级入口都没有，所以一般尽量保持简单稳定，并且生产环境中可能通过 Flash protection 防止被误写。

---

# 30. 面试官：Bootloader 自己能升级吗？

可以，但复杂很多。

你可以回答：

> 普通单 Bootloader 架构一般不轻易更新自身，因为 CPU 正在执行 Bootloader 时，再擦写自身所在 Flash 区风险很大。
> 
> 如果需要 Bootloader 自升级，可以使用一级 Bootloader + 二级 Bootloader 的方式，或者利用双 Bank / RAM 中运行 Flash 写入代码等机制。

你不用硬说自己实现过。

直接：

> “我的项目没有做到 Bootloader 自升级，但我了解实现思路。”

最合适。

---

# 31. 面试官：为什么 Flash 操作期间有时候要关中断？

回答：

> 如果 MCU 在 Flash 擦写过程中不能正常从同一个 Flash Bank 取指，那么此时触发中断，中断函数也在 Flash 中，就可能受到影响。
> 
> 因此根据芯片架构和具体 Flash 控制器要求，有些场景需要暂时关闭中断，或者把关键代码放到 RAM 中执行。

这是进阶回答。

---

# 32. 面试官：程序跑飞了怎么办？

你可以把 Bootloader 和 watchdog 联系起来。

> APP 中可以开启独立看门狗。
> 
> 如果程序跑飞无法正常喂狗，MCU 自动复位。
> 
> Bootloader 可以记录复位原因或启动失败次数。如果连续多次启动 APP 失败，可以进入升级模式或者回滚到上一个有效固件。

这就是产品级思想。

---

# 33. 面试官：怎么让 Bootloader 决定进入升级模式？

常见有几种：

```
1. GPIO 按键
2. APP 写 upgrade flag
3. UART 收到升级命令
4. APP 校验失败
5. Watchdog多次启动失败
```

比如：

APP：

```
upgrade_flag = 0x12345678;
NVIC_SystemReset();
```

Boot：

```
if (upgrade_flag == UPDATE_MAGIC)
{
    EnterUpdateMode();
}
```

如果 flag 存 Flash：

> 擦写次数有限。

如果存在 backup register：

> 掉电行为又不同。

都是可以继续延伸的点。

---

# 34. 面试官：为什么 APP 不能直接自己给自己升级？

核心问题：

> 它不能简单地一边运行，一边把自己正在执行的代码区擦掉。

所以通常：

```
APP
 ↓
收到升级请求
 ↓
设置flag
 ↓
reset
 ↓
Bootloader
 ↓
升级APP
```

Bootloader 的一个重要价值就是：

> 独立于 APP 存在，因此 APP 损坏以后仍然可以恢复。

---

# 35. 面试官：为什么需要 Bootloader，不直接用 ST-Link 下载？

你回答：

> 开发阶段 ST-Link 很方便，但产品部署以后用户通常没有调试器。
> 
> Bootloader 可以通过 UART、CAN、USB、Ethernet 等通信方式完成 IAP 升级，实现现场更新、远程升级和故障恢复。

然后区分：

```
ISP
In-System Programming

IAP
In-Application Programming
```

你的项目更接近：

> IAP Bootloader。

---

# 36. 面试官：如果 APP 首地址都是 0xFF，意味着什么？

Flash 擦除状态：

```
0xFFFFFFFF
```

那么：

```
*(uint32_t *)APP_ADDR == 0xFFFFFFFF
```

说明大概率：

> APP 区为空或者已经被擦除。

此时：

```
MSP = 0xFFFFFFFF
```

明显不在 SRAM。

所以：

> 不允许跳转 APP。

进入升级模式。

---

# 37. 面试官：如果 Reset_Handler 是 0xFFFFFFFF 呢？

同理。

代表：

> APP 向量表没有有效的 Reset_Handler。

如果硬跳：

```
PC = 0xFFFFFFFF
```

必然异常。

所以必须：

```
Reset_Handler range check
```

---

# 38. 面试官：APP 跳转代码完整一点应该怎么做？

你至少理解这个思想：

```
void JumpToApp(void)
{
    uint32_t appStack;
    uint32_t appResetHandler;

    appStack = *(volatile uint32_t *)APP_ADDR;
    appResetHandler =
        *(volatile uint32_t *)(APP_ADDR + 4);

    if (!IsValidStack(appStack))
        return;

    if (!IsValidResetHandler(appResetHandler))
        return;

    __disable_irq();

    HAL_DeInit();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    SCB->VTOR = APP_ADDR;

    __set_MSP(appStack);

    ((void (*)(void))appResetHandler)();
}
```

面试时不要说：

> 我背过这个代码。

你应该逐句解释：

```
检查向量表
↓
关中断
↓
关闭Boot外设
↓
关闭SysTick
↓
切换VTOR
↓
设置MSP
↓
跳Reset_Handler
```

---

# 39. 这里还有一个特别容易被追问的坑

面试官：

> 你都 `__disable_irq()` 了，跳到 APP 后 APP 怎么开中断？

这个问题挺不错。

答案：

> `__disable_irq()` 会设置 PRIMASK，正常情况下如果 Bootloader 关闭全局中断，就应该确保 APP 初始化完成后重新调用 `__enable_irq()`。
> 
> 或者 Bootloader 在完成 VTOR、MSP 等切换后，根据设计恢复中断状态。
> 
> 重点是不能让 APP 永远继承 Bootloader 的全局中断关闭状态。

---

# 40. 面试官：APP 的中断向量表是谁生成的？

这直接联系你 Startup。

回答：

> APP 工程自己的 Startup 文件会定义中断向量表，其中包含初始 MSP、Reset_Handler、NMI_Handler、HardFault_Handler 和各个外设 IRQHandler 地址。
> 
> 链接的时候向量表会被放在 APP Flash 起始位置，例如 0x08010000。

所以：

```
Startup
↓
向量表
↓
Linker
↓
APP_START
```

你之前学 Startup，这里完全就串起来了。

---

# 41. 面试官：Boot 和 APP 为什么都有 startup？

因为：

> 它们本质上是两个独立程序。

Boot：

```
Boot startup
Boot vector table
Boot Reset_Handler
Boot main
```

APP：

```
APP startup
APP vector table
APP Reset_Handler
APP main
```

只不过硬件复位首先进入 Boot。

随后 Boot：

> 人工模拟复位启动 APP。

---

# 42. 面试官：如果 APP 跳进去之后 HardFault，你怎么排查？

这是很像真实面试的问题。

你可以按顺序回答：

### 第一查 MSP

```
APP_ADDR 第一个 word
```

是否在 SRAM。

### 第二查 Reset_Handler

```
APP_ADDR + 4
```

是否落在 APP Flash。

### 第三查链接地址

APP 是否真的链接到了：

```
0x08010000
```

### 第四查 VTOR

```
SCB->VTOR
```

是否指向 APP。

### 第五查 Boot 外设和中断

```
SysTick
UART
DMA
TIM
```

是否残留。

### 第六查时钟

Bootloader 和 APP 如果都初始化 RCC：

> 是否存在状态不一致。

### 第七看 HardFault 寄存器

```
CFSR
HFSR
BFAR
MMFAR
```

定位异常原因。

这个回答会很有工程感。

---

# 43. 一个典型面试连续追问，你可以这样接

面试官：

> 你做过 Bootloader？

你：

> 做过 STM32F407 的 UART IAP Bootloader，把 Bootloader 和 APP 放在不同 Flash 区，通过 UART 接收 APP 固件，完成 Flash 擦写、固件校验和 APP 跳转。

面试官：

> Flash 怎么规划？

你：

> 按 Sector 规划，Bootloader 从 0x08000000 开始，APP 从预留的 Sector 边界开始，另外留参数区。写入时做地址边界保护，避免擦到 Bootloader。

面试官：

> APP 怎么启动？

你：

> 读取 APP 向量表前两个 word，第一个是 MSP，第二个是 Reset_Handler。检查 MSP 在 SRAM、Reset_Handler 在 APP Flash 后，设置 VTOR 和 MSP，再跳 Reset_Handler。

面试官：

> 如果那个地址不对呢？

你：

> 不跳转，否则可能直接 HardFault。我会认为 APP 无效，继续留在 Bootloader 的升级模式，等待重新下载。真实产品还会再检查 Magic、长度和 CRC。

面试官：

> 下载一半断电呢？

你：

> 当前单 APP 方案中 APP 会变得无效，再启动时 Boot 校验失败，保持升级模式重新下载。如果要求更高可靠性，会做 A/B 双分区，先升级备用区，校验成功后再切换启动标志。

这已经是一条非常完整的回答链了。