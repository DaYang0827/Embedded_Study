
先把 Linux 想成下面这 5 层：

```
┌──────────────────────────────┐
│        用户应用层 User       │
│  C程序 / pthread / socket    │
│  open read write ioctl       │
└──────────────┬───────────────┘
               │ System Call
               ↓
┌──────────────────────────────┐
│        Linux Kernel          │
│                              │
│  进程调度 / 内存管理 / VFS   │
│  网络 / IPC / 驱动 / 中断     │
└──────────────┬───────────────┘
               │
               ↓
┌──────────────────────────────┐
│       Device Driver          │
│ GPIO / UART / I2C / SPI      │
│ Character Driver / Platform  │
└──────────────┬───────────────┘
               │
               ↓
┌──────────────────────────────┐
│       Hardware / SoC         │
│ CPU / RAM / Flash / 外设      │
└──────────────────────────────┘
```

再往系统启动方向看，还有一条：

```
BootROM
↓
Bootloader（例如 U-Boot）
↓
Linux Kernel
↓
Device Tree
↓
Root File System
↓
用户程序
```

以后学 Linux，实际上就是把这两张图慢慢填满。

---

# Linux 和 STM32 区别

STM32 的思维是：

```text
main()
↓
while(1)
↓
直接操作寄存器
↓
硬件
```

例如：

```
USART1->CR1 |= ...;
GPIOA->MODER |= ...;
```

Linux 里通常变成：

```text
用户程序
↓
系统调用
↓
Kernel
↓
Driver
↓
硬件
```

用户程序一般不会直接：

```c
USART1->DR = data;
```

而是：

```c
fd = open("/dev/ttyS1", O_RDWR);
write(fd, buf, len);
```

真正去操作 UART 寄存器的是内核里的 UART Driver。所以这是最先要建立的核心：

> STM32 裸机：应用和硬件距离很近。  
> Linux：中间多了内核和驱动这一层。

# Linux 用户态

这是进入 Linux 的第一站。需要先会：

```text
文件
进程
线程
内存
IPC
Socket
```

而不是先去碰内核。

## 文件 IO

Linux 最核心的几个函数：

```c
open()
read()
write()
close()
```

例如：

```
int fd = open("test.txt", O_RDWR);

write(fd, "hello", 5);

close(fd);
```

这里：

```
fd
```

叫：

> File Descriptor，文件描述符。

以后你打开：

```
普通文件
串口
设备节点
socket
pipe
```

很多都会返回一个 fd。

所以：

> fd 是 Linux 用户态非常核心的概念。

---

## 2. 进程

你需要理解：

```
Program
和
Process
```

区别。

程序：

```
硬盘上的可执行文件
```

进程：

```
程序正在运行的实例
```

例如：

```
./app
```

就产生一个 Process。

你需要学：

```
fork()
exec()
wait()
exit()
```

理解：

```
父进程
子进程
PID
进程地址空间
```

---

## 3. 线程

这是和 FreeRTOS 非常容易串起来的。

FreeRTOS：

```
Task
```

Linux：

```
Thread
```

以后你会学：

```
pthread_create()
pthread_join()
pthread_mutex_lock()
pthread_cond_wait()
```

可以对应你 FreeRTOS 的：

```
Task
Mutex
Semaphore
Queue
```

所以你现在先学 FreeRTOS其实对 Linux 很有帮助。

---

# 三、Linux 内存模型

这是你后面一定要理解的。

STM32：

```
Flash
RAM
Stack
Heap
```

Linux 进程里会变成：

```
高地址
┌───────────────┐
│ Stack         │
├───────────────┤
│               │
│ mmap区域       │
│               │
├───────────────┤
│ Heap          │
├───────────────┤
│ .bss          │
├───────────────┤
│ .data         │
├───────────────┤
│ .rodata       │
├───────────────┤
│ .text         │
└───────────────┘
低地址
```

你前面已经学过：

```
.text
.data
.bss
Stack
Heap
```

所以这个知识完全可以迁移。

但 Linux 多一个重要东西：

> Virtual Memory，虚拟内存。

程序看到：

```
0x7xxxxxxx
```

不一定是真实 RAM 地址。

而是：

```
Virtual Address
↓
MMU
↓
Page Table
↓
Physical Address
```

这一块等你用户态 C 熟一点再深入。

---

# 四、Kernel 是干什么的

Linux Kernel 你先不要理解成“一坨很大的代码”。

先把它分成几个核心模块：

```
Linux Kernel
│
├── Process Scheduler
│
├── Memory Management
│
├── VFS
│
├── Device Driver
│
├── Network Stack
│
├── IPC
│
└── Interrupt / Timer
```

你目前最需要的主要是：

```
Scheduler
Memory
VFS
Driver
Interrupt
```

---

## 1. Scheduler

这个和 FreeRTOS Scheduler 很像。

FreeRTOS：

```
Task A
Task B
Task C
↓
Scheduler决定谁运行
```

Linux：

```
Process
Thread
↓
Linux Scheduler
```

核心思想一样：

> CPU 同一时刻执行一个上下文，调度器决定什么时候切换。

所以你先学 FreeRTOS Task/Context Switch 后，Linux Scheduler 会好理解很多。

---

# 五、VFS

这个是 Linux 非常重要的一层。

VFS：

```
Virtual File System
```

你用户程序只需要：

```
open()
read()
write()
```

下面可能是：

```
普通文件
ext4
proc
sysfs
字符设备
socket
```

用户不用管。

所以：

```
User
↓
open/read/write
↓
VFS
↓
具体实现
```

这就是抽象。

你以后理解 Linux Driver，一定会一直碰到 VFS。

---

# 六、Device Driver

这个是你未来偏 Linux 嵌入式最重要的一块。

你可以先记：

> Driver = Linux Kernel 和硬件之间的桥。

比如：

```
用户程序
↓
write(fd)
↓
VFS
↓
Driver
↓
UART寄存器
```

你以后会学到：

```
struct file_operations
```

例如：

```
static const struct file_operations fops = {
    .open = my_open,
    .read = my_read,
    .write = my_write,
};
```

然后：

```
用户 open()
↓
my_open()

用户 read()
↓
my_read()

用户 write()
↓
my_write()
```

这就是 Linux 驱动最核心的一条链。

---

# 七、字符设备

Linux 驱动入门一般先学：

```
Character Device
字符设备
```

因为最容易理解。

例如：

```
/dev/myled
/dev/ttyS0
/dev/i2c-1
```

你会接触：

```
dev_t
major
minor
cdev
file_operations
```

---

## 主设备号和次设备号

一句话：

```
Major
→ 找驱动

Minor
→ 区分同一驱动下不同设备
```

比如：

```
/dev/led0   240:0
/dev/led1   240:1
/dev/led2   240:2
```

都是：

```
major = 240
```

说明同一个 Driver。

但是：

```
minor = 0/1/2
```

表示不同 LED。

---

# 八、Device Tree

这个是嵌入式 Linux 必学。

你现在 STM32 里可能是：

```
GPIO_Init();
USART_Init();
```

很多硬件参数写死在 C 里。

Linux 倾向于：

```
Driver
→ 描述“怎么驱动这种硬件”

Device Tree
→ 描述“这块板上有哪些硬件”
```

例如：

```
uart1 {
    compatible = "vendor,my-uart";
    reg = <...>;
    interrupts = <...>;
};
```

Driver：

```
compatible = "vendor,my-uart"
```

匹配成功：

```
Device Tree
↓
Driver match
↓
probe()
```

---

# 九、Platform Driver

SoC 内部很多东西：

```
GPIO Controller
UART
SPI
I2C
Timer
```

并不是 USB 那种即插即拔设备。

Linux 通常用：

```
Platform Device
+
Platform Driver
```

来管理。

核心流程：

```
Device Tree
↓
产生device信息
↓
和driver匹配
↓
probe()
```

所以以后：

```
static int xxx_probe(...)
```

你就知道它是什么意思：

> Linux 发现这个驱动可以管理这个设备，现在开始初始化。

---

# 十、中断

你 STM32 已经有很好的基础。

STM32：

```
USART1_IRQHandler()
```

Linux：

```
request_irq()
```

然后写：

```
irqreturn_t my_irq_handler(...)
```

但 Linux 驱动里更强调：

> ISR 要短。

所以你后面还会遇到：

```
Top Half
Bottom Half
Workqueue
Threaded IRQ
```

这个和你之前学的：

```
ISR快进快出
RingBuffer
Deferred processing
```

其实完全是一条思路。

---

# 十一、同步和并发

Linux Kernel 里会有：

```
mutex
spinlock
semaphore
completion
atomic
wait queue
```

你现在 FreeRTOS 学：

```
Mutex
Semaphore
Critical Section
Task Notification
```

以后都能迁移。

所以你 FreeRTOS 要认真学的原因之一就是：

> 它在帮你提前建立 Linux Kernel 并发思维。

---

# 十二、Linux 启动流程

以后做嵌入式 Linux，必须知道：

```
Power On
↓
BootROM
↓
U-Boot
↓
Linux Kernel
↓
Device Tree
↓
Root File System
↓
init/systemd
↓
用户程序
```

和你现在 STM32：

```
Reset
↓
Bootloader
↓
APP
```

其实概念非常像。

只是 Linux 更复杂。

---

# 十三、U-Boot

你现在做过 STM32 Bootloader，所以 U-Boot 会很有意思。

你的 Bootloader：

```
检查APP
↓
加载/跳转APP
```

U-Boot：

```
初始化硬件
↓
寻找Linux Kernel
↓
加载Kernel
↓
加载Device Tree
↓
准备启动参数
↓
跳入Linux Kernel
```

所以你以后学 U-Boot，不是完全陌生东西。

---

# 十四、Root File System

Linux 和 MCU 另一个巨大区别是：

> Linux Kernel 自己还不够。

还需要：

```
Root File System
```

里面有：

```
/bin
/sbin
/etc
/lib
/dev
/proc
/sys
```

以及：

```
Shell
动态库
系统工具
应用程序
```

你以后会碰：

```
BusyBox
```

这也是嵌入式 Linux 常见组件。

---

# 十五、把整个嵌入式 Linux 体系压缩成一张图

你以后就按这张图学习：

```
                Embedded Linux
                      |
       ┌──────────────┴──────────────┐
       │                             │
   用户空间                       内核空间
       |                             │
       │                             │
 C / Makefile                    Kernel
 pthread                        Scheduler
 socket                         Memory
 file IO                        VFS
       │                             │
       └────── System Call ──────────┘
                                     │
                                  Driver
                                     │
                 ┌───────────────────┼─────────────┐
                 │                   │             │
              Char Driver        Platform      Network
                 │               Driver
                 │                   │
              /dev             Device Tree
                 │                   │
                 └───────── Hardware ──────────────┘
```

启动部分：

```
BootROM
↓
U-Boot
↓
Kernel
↓
Device Tree
↓
RootFS
↓
Application
```

---

# 十六、结合你的背景，学习路线应该这样走

你现在不要直接从 Device Tree 开始。

建议：

```
阶段1 Linux使用基础
────────────────
Linux命令
gcc
Makefile
文件IO
目录IO

阶段2 Linux系统编程
────────────────
Process
Thread
pthread
Mutex
Semaphore
IPC
Socket

阶段3 Kernel基础
────────────────
Kernel Module
printk
insmod/rmmod
内核/用户空间
copy_to_user
copy_from_user

阶段4 Character Driver
────────────────
dev_t
Major/Minor
cdev
file_operations
/dev

阶段5 Device Model
────────────────
device
driver
bus
class
sysfs

阶段6 Embedded Driver
────────────────
Device Tree
Platform Driver
probe/remove
ioremap
interrupt

阶段7 总线驱动
────────────────
I2C
SPI
UART
GPIO

阶段8 深入
────────────────
U-Boot
Kernel启动
Memory
Scheduler
Driver Model
```