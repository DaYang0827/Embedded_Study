更适合你简历的是：
![[Pasted image 20260828163923.png]]

其中 **Zephyr复盘和Linux基础可以穿插进行**

# 1 概念

`RTOS` （Real Time Operating System，中文就是实时操作系统）

`FreeRTOS`是一个迷你的实时操作系统内核。作为一个轻量级的操作系统，功能包括：任务管理、时间管理、信号量、消息队列、内存管理、记录功能、软件定时器、协程等，可基本满足较小系统的需要。

由于RTOS需占用一定的系统资源（尤其是RAM资源），只有uC/OS-I、口embOS、salvo、FreeRTOS等少数实时操作系统能在小RAM单片机上运行。相对uC/OS-II、embOS等商业操作系统，FreeRTOS操作系统是完全免费的操條3585462系统，具有源码公开、可移植、可裁减、调度策略灵活的特点，可以方便地移植到各种单片机上运行。

FreeRTOS的设计小巧且简易，整个核心代码只有3到4个C文件，为了让代码容易阅读、移植和维护，大部分的代码都是以C语言编写，只有一些函数（多数是架构特定排班副程序）采用汇编语言编写。

# 2  FreeRTOS移植

1. 添加RTOS源码到Keil工程
2. 添加head_4.c到Keil工程
3. 添加port.c到Keil工程
4. 添加头文件路径
5. 添加FreeRTOSConfig.h
6. 修改FreeRTOSConfig.h配置文件，直到工程编译无错误

# 3 数据类型与编程规范

## 3.1 数据类型

每个移植的版本都含有自己的 `portmacro.h` 头文件，里面定义了2个数据类型：

**`TickType_t`：
- FreeRTOS配置了一个周期性的时钟中断：`Tick Interrupt`
- 每发生一次中断，中断次数累加，这被称为`tick count`
- `tick count`这个变量的类型就是`TickType_t`
- `TickType_t`可以是16位的，也可以是32位的
- `FreeRTOSConfig.h`中定义`configUSE_16_BIT_TICKS`时，`TickType_t`就是`uint16_t`
- 否则`TickType_t`就是`uint32_t`
- 对于32位架构，建议把`TickType_t`配置为`uint32_t`

**`BaseType_t`：
- 这是该架构**最高效的数据类型**
- 32位架构中，它就是`uint32_t`
- 16位架构中，它就是`uint16_t`
- 8位架构中，它就是`uint8_t`
- `BaseType_t`通常用作简单的返回值的类型，还有逻辑值，比如 `pdTRUE/pdFALSE`

## 3.2 变量名

| 变量名前缀 |                           含义                            |
| :---: | :-----------------------------------------------------: |
|  `c`  |                         `char`                          |
|  `s`  |                    `int16_t`，`short`                    |
|  `l`  |                    `int32_t`，`long`                     |
|  `x`  | `BaseType_t`，其他非标准的类型：结构体、`task handle`、`queue handle`等 |
|  `u`  |                       `unsigned`                        |
|  `p`  |                           指针                            |
| `uc`  |                `uint8_t`，`unsigned char`                |
| `pc`  |                        `char`指针                         |

## 3.3 函数名

函数名的前缀有2部分：返回值类型、在哪个文件定义

|        函数名前缀        |                    含义                     |
| :-----------------: | :---------------------------------------: |
| `vTaskPrioritySet`  |      返回值类型：`void`  <br>在`task.c`中定义       |
|   `xQueueReceive`   |  返回值类型：`BaseType_t`   <br>在`queue.c`中定义   |
| `pvTimerGetTimerID` | 返回值类型：`pointer to void`  <br>在`tmer.c`中定义 |

# 4 TCB

**TCB** 的全称是 **Task Control Block（任务控制块）**。它的本质：**TCB 就是 FreeRTOS 给每一个任务专门发放的“身份证/档案袋”。** 它是一个极其复杂的 C 语言**结构体（Struct）**。为了在多任务来回切换时实现“瞒天过海”的效果，每个任务在内存里都会躺着一个专属于自己的 TCB。

1. TCB 内部

   如果把 TCB 这个结构体拆开，它内部最核心的几个硬核成员变量如下：

```c
typedef struct tskTaskControlBlock
{
    volatile StackType_t    *pxTopOfStack;   // 1. 【生死攸关】当前任务的“栈顶指针(SP)”
    ListItem_t              xStateListItem;  // 2. 状态列表项（用来把任务挂在 就绪/阻塞 链表里）
    UBaseType_t             uxPriority;      // 3. 任务的“优先级号”
    StackType_t             *pxStack;        // 4. 任务分配的“栈内存起始基地址”
    char                    pcTaskName[16];  // 5. 任务的英文小名（就是你写的"SENT_TASK"）
    // ... 其他诸如任务通知、死锁保护等高级变量
} tskTCB;
```

核心成员一：`pxTopOfStack`（当前栈顶指针）—— 灵魂所在

- 这是 TCB 结构体的**第一个成员**。它的作用是记录当前任务**的 `R13 (SP)` 堆栈指针现在指在 RAM 的哪个具体位置**。
- 有了它，当系统从任务一切换到任务二时，内核才能知道上一次任务一活干到哪了、局部变量保存在哪。

核心成员二：`uxPriority`（优先级）

- 记录创建任务时填的数字（比如填的 `1`）。FreeRTOS 的核心调度器每时每刻都在扫描所有 TCB 里的这个数字，谁的数字大，CPU 硬件的 PC 指针下一微秒就会无条件去执行谁的任务函数。

---

2. 运行时画面：TCB 是如何配合 CPU 寄存器实现“任务切换”的？

现在，把 **“硬件寄存器（R0~R15）”** 和 **“软件 TCB”** 结合起来。  
假设当前有两个任务：`task1`（正在运行）和 `task2`（正在排队）。突然，时钟节拍（SysTick）爆发，操作系统决定强行切换任务。

步骤 1：把当前 CPU 的“肉身”剥离成数据，存入 `task1` 的家

1. CPU 停下 `task1` 手头的活。
2. 内核会执行一连串汇编指令（`PUSH`），**把当前 CPU 硬件里正在高频闪烁的寄存器（R0~R12、LR、PC、xPSR）的值，全部强行压入 `task1` 自己的 RAM 栈（Stack）区间里**。
3. 压完之后，当前的硬件堆栈指针 `SP` 停在了一个新位置。
4. 内核把这个最新的 `SP` 地址数字，**死死地写入到 `task1` 的 TCB 结构体的第一个变量 `pxTopOfStack` 盒子里**。
5. 自此，`task1` 被完美“封印”在它的 TCB 档案袋里了。

步骤 2：从 `task2` 的档案袋里“借尸还魂”

1. 调度器通过链表，找到了接下来要跑的 `task2` 的 TCB。
2. 内核读取 `task2->pxTopOfStack`，拿到了它上一次上岸时保存的栈顶地址。
3. 内核强行把这个地址赋值给 CPU 硬件的 **`R13 (SP)` 寄存器**。
4. 紧接着，执行出栈汇编指令（`POP`），把 `task2` 当年保存在栈里的数据，**重新倒灌回 CPU 的硬件寄存器（R0~R12、LR、PC）中**。
5. **见证奇迹：** 当 PC（`R15`）寄存器被重新灌入了上一次 `task2` 临走时的地址后，下一微秒，CPU 就彻底换了脑子，开始顺着 `task2` 的代码行继续往下狂飚了！

这就是整个嵌入式实时操作系统最核心的魔法——**上下文切换（Context Switch）**。

---

3. TCB 是在哪里开辟的？（结合之前的内存管理）

   在 `main.c` 里调用 `xTaskCreate` 的那一瞬间：

- FreeRTOS 内部的内存管理总管（如 `heap_4.c`）会立刻跑到 **RAM 的堆区（Heap）** 中，强行圈下两块地盘：
    - 第一块地盘：用来存放任务干活开辟局部变量用的**栈空间（Stack）**（填的 128 字）。
    - 第二块地盘：**就是一个正好能装下 `tskTCB` 结构体大小的内存块（通常几十个字节）**。
- 所以，TCB 是在程序运行起来后，在 **RAM 的堆区里动态申请、静态维护**的死忠档案。

总结：

- **Task Function (任务函数)**：是放在 Flash 里的**死指令代码**。
- **Stack (任务栈)**：是放在 RAM 里用来**存局部变量和恢复寄存器**的临时干粮仓库。
- **TCB (任务控制块)**：是操作系统内核握在手里的**遥控器和绝密档案**。它通过记录每个任务的栈顶指针（`pxTopOfStack`）和优先级，实现了在多任务之间“移形换影”的闭环调度。

# 5 任务Task/Thread
## 5.1 创建任务

|             函数             | 含义      | 返回值     |
| :------------------------: | :-----: | :-----: |
|      `xTaskCreate()`       | 创建任务    | 有，成功或失败 |
|       `vTaskDelay()`       | 当前任务延时  | 无       |
|  `vTaskStartScheduler()`   | 启动调度器   | 无       |
| `xTaskGetSchedulerState()` | 获取调度器状态 | 有       |
|      `vTaskDelete()`       | 删除任务    | 无       |

`xTask` 和 `vTask` 不是不同任务，而是 FreeRTOS 的函数命名习惯；v 通常表示无返回值，x 通常表示有返回值，Task 表示任务管理相关函数

`xTaskCreate`定义：

```c
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                        Const char* const pcName,
                        Const configSTACK_DEPTH_TYPE usStackDepth,
                        Void* const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t* const pxCreatedTask)
```

把 `xTaskCreate()` 理解成“告诉 FreeRTOS：**要创建一个以后可以被调度运行的任务，请你给它准备 TCB、栈、任务状态等管理信息**。” 它不是“现在立刻运行这个函数”，而是**先把任务创建出来**。真正开始调度，是后面的`vTaskStartScheduler();`

- `pxTaskCode`:是**任务函数入口**
- `pcName`：任务名，字符串
- `usStackDepth`：栈深，**即任务的栈大小（单位是字，1字 = 4字节）**
- `pvParameters`：任务的参数指针（即FreeRTOS 允许给任务函数传一个“通用指针”）
- `uxPriority`：任务的优先级，最低优先级是0，数字越大，优先级越高
- `pxCreatedTask`：任务的句柄，用于控制任务

在调用的时候使用：

```c
void send_task1(void *pvParameters)
{
    while(1)
    {
        vTaskDelay(500);
        usart_send_string(&usart1,"Task1");
    }
}

xTaskCreate(send_task1,"TASK1", 128, NULL, 1, NULL);
xTaskCreate(send_task2,"TASK2", 128, NULL, 1, NULL);
```

理解：

1. `send_task1`

```c
void send_task1(void *pvParameters)
```

这是**任务函数入口**。本质上和普通函数地址类似：

```text
xTaskCreate()
   ↓
记住 send_task1 的入口地址
```

等调度器以后选中这个任务时，就从这个函数开始执行。

---

2. `"TASK1"`

这是**任务名字**。主要用于：

- Debug
- RTOS-aware 调试工具
- 查看任务列表
- Stack Overflow Hook 里识别任务

以后如果某个任务栈溢出pcTaskName，可以看出来到底是 `TASK1` 还是 `TASK2`。

---

3. `128`

这个非常重要。它表示任务栈深度128 个 StackType_t不是一定代表 128 bytes。 STM32F4 是 32 位 Cortex-M4，通常`sizeof(StackType_t) = 4 bytes` 所以`128 × 4 = 512 bytes`。也就是说128大约给这个 Task 分了512 bytes stack这个栈是任务私有的。所以：

```
Task1 有自己的栈
Task2 也有自己的栈
```

它们互不共用。这点非常关键，因为任务切换的时候 FreeRTOS 要保存：

```
局部变量
函数调用现场
寄存器上下文
返回地址
```

这些都和 Task 的栈有关。

---

4. `NULL`

第四个参数`void *pvParameters`是传给 Task 的参数。使用NULL，说明这个 Task 不需要外部参数。所以函数

```c
void send_task1(void *pvParameters)
```

虽然有这个参数，但现在实际上没使用。可以写：

```c
void send_task1(void *pvParameters)
{
    (void)pvParameters;

    while(1)
    {
        ...
    }
}
```

避免 unused parameter warning。以后可以传结构体`LED_TypeDef LED0;`然后：

```c
xTaskCreate(
    led_task,
    "LED",
    128,
    &LED0,
    1,
    NULL
);
```

任务里：

```c
void led_task(void *pvParameters)
{
    LED_TypeDef *led = (LED_TypeDef *)pvParameters;
}
```

所以第四个参数本质是给 Task 带一个地址进去。

---

5. 第五个参数

   `1`这是优先级。**数字越大→ 优先级越高**，最低通常`tskIDLE_PRIORITY`就是 0。现在：

```text
Task1 priority = 1
Task2 priority = 1
```

所以两个任务是同优先级。如果时间片轮转开启，它们都 Ready 时可以轮流执行。但当前代码里它们大部分时间都在`vTaskDelay(...)`

所以实际上经常是：

```text
Task1 Blocked
Task2 Running

或者

Task2 Blocked
Task1 Running
```

---

6. 最后一个参数

   `NULL` 是`TaskHandle_t *pxCreatedTask`也就是**要不要把创建出来的任务“句柄”保存下来**。现在不需要以后控制这个任务，所以传`NULL`。如果想以后：

```c
vTaskDelete()
vTaskSuspend()
vTaskResume()
vTaskPrioritySet()
```

   就可以保存 handle  `TaskHandle_t task1_handle;`然后：

```c
xTaskCreate(
    send_task1,
    "TASK1",
    128,
    NULL,
    1,
    &task1_handle
);
```

之后`vTaskSuspend(task1_handle);` 就能暂停 Task1。所以 Handle 可以理解成FreeRTOS 里**用来找到这个任务的“身份证/引用”**。

代码串起来就是：

```text
main()
↓
xTaskCreate(Task1)
↓
为Task1创建TCB + Stack
↓
xTaskCreate(Task2)
↓
为Task2创建TCB + Stack
↓
此时任务已经存在，但还没真正开始调度
↓
vTaskStartScheduler()
↓
Scheduler启动
↓
根据优先级 / Ready / Blocked 状态选择任务
↓
Task1 / Task2开始运行
```

## 5.2 任务状态

主要有四种任务状态：

```text
Running   正在占用 CPU
Ready     已经准备好，等 CPU
Blocked   在等时间/事件     阻塞状态
Suspended 被人为挂起
```

由于单片机通常只有**一个 CPU 内核**，同一时间其实**只能有一个任务真正霸占 CPU 执行代码**，其他任务都在后台根据状态进行排队和流转。FreeRTOS 官方对这四种状态的定义就像是一个精妙的“职场社会制度”：

### 5.2.1 Running

运行态 (Running) —— 正在干活的“现任总裁”

- **概念：** 这个任务当前正在真正**霸占 CPU 硬件**。它的 `PC` 指针正在一行行执行机器码。
- **数量限制：** 对于单核 STM32（如常见的 F407），**同一微秒内，有且仅能有一个任务处于运行态！**
- **切换触发：** 一旦它的干活时间到了（时间片轮转），或者来了一个优先级更高的大佬，它就会被无情剥离，交出 CPU 控制权。

### 5.2.2 Ready

就绪态 (Ready) —— 万事俱备，只欠 CPU 的“候补队员”

- **概念：** 这个任务已经**完成了所有的准备工作**。它的全局变量、局部变量、干粮（堆栈空间）全都各就各位，只要操作系统内核一招手，它能**在 1 微秒内瞬间恢复肉身，跳进运行态干活**。
- **排队机制（TCB 的舞台）：** 内核把所有就绪态任务的 **TCB** 挂在一个“就绪链表（Ready List）”里。每当时钟节拍爆发，内核调度器会去这个链表里翻看，**挑选优先级（`uxPriority`）最高的那个任务，直接升级为运行态。**
- **特点：** 它没在干活，不是因为它被卡住了，纯粹是因为前面有更高优先级的任务在插队。

### 5.2.3 Blocked

阻塞态 (Blocked) —— 正在等信号或在“睡大觉”的退休员工

- **概念：** 任务在等待某个特定的事件，或者在主动延时。在事件发生之前，**即使 CPU 完全空闲，也绝对不会把执行权分给它。**
- **经典场景：**
    1. **主动睡觉：** 比如之前在任务死循环里写的 **`vTaskDelay(1000);`**。调用这一行的瞬间，任务就会主动卸卸下肉身，把自己的 TCB 踢进“阻塞链表”，给自己定一个 1 秒后的闹钟。
    2. **死等信号：** 比如发送任务在等待串口发送完毕的信号（队列、信号量）。如果队列是空的，任务就会在 `xQueueReceive` 这里死等阻塞。
- **意义（极度省电）：** 阻塞态是 FreeRTOS 的精髓。任务在阻塞时，**完全不消耗任何 CPU 算力**，CPU 可以腾出 100% 的精力去跑别的任务或者进入低功耗休眠，绝对不像裸机编程里的 `delay_ms()` 那样让 CPU 在原地傻傻转圈。

`Task1` 调用 `vTaskDelay(500)` 进入 **Blocked（阻塞态）** 的这个过程，可以用一句话来概括其本质 **“Task1 主动卸下肉身，把自己的 TCB 锁进一间小黑屋，给自己定了一个 500 毫秒的硬核闹钟，并把 CPU 这把车钥匙主动让给了其他人。”**

当写下 `vTaskDelay(500)` 的这一微秒内，系统内部发生了：

1. 软件层面：移形换影的“链表搬运”

   在 FreeRTOS 内核的 RAM（堆区）里，维护着好几条像排队队列一样的 **“链表（List）”**：

- **Ready List（就绪链表）：** 里面挂着所有随时准备干活的任务 TCB。
- **Delayed Task List（延时/阻塞链表）：** 里面挂着所有正在睡觉、等时间的任务 TCB。

当 `Task1` 正在执行并一脚踩中 `vTaskDelay(500)` 时：

- **打入另册：** 内核会把 `Task1` 的 **TCB（任务控制块）** 从 `Ready List` 里面强行解下来。**一旦解下来，下一次 CPU 挑选谁来干活时，就绝对看不到 Task1 了。**
- **定下闹钟：** 内核会读取当前系统的“绝对时间总节拍”（假设当前系统从开机到现在数了 1000 下，叫 `xTickCount = 1000`）。内核算了一道加法：`1000 + 500 = 1500`。
- **塞入黑屋：** 内核把 `1500` 这个**未来的苏醒时间**作为标签，死死刻在 `Task1` 的 TCB 上，然后把它的 TCB 挂进了 `Delayed Task List`（阻塞链表）里。

自此，`Task1` 在软件层面上就被正式 **Blocked**（阻塞）了。它就像一个陷入冬眠的船员，在时间没到 1500 之前，不管外面发生了什么，它都绝对不会醒来。

---

2. 硬件层面：瞒天过海的“上下文切换”

在把 TCB 搬到阻塞链表的同时，CPU 硬件寄存器里正在发生一场激烈的“脱壳”运动（这就是我们在 TCB 那一节讲过的内核魔术）：

- **封印肉身：** CPU 硬件立刻执行汇编指令，把当前 `Task1` 还没跑完的通用寄存器（R0~R12、LR、PC、xPSR）的数值，**全部强行压入 `Task1` 自己的 RAM 栈（Stack）空间里**。
- **锁死指针：** 把最新的栈顶地址，写进 `Task1->pxTopOfStack`。
- **让出钥匙：** 操作系统内核的“调度器”上场。调度器看了一眼 `Ready List`（就绪链表），发现里面还躺着正在排队的 `Task2`。
- **注入灵魂：** 调度器读取 `Task2` 档案袋里的栈顶指针，灌入 CPU 的 `R13 (SP)`，然后执行出栈指令（`POP`），把 `Task2` 上次没干完的寄存器值倒灌回 CPU 硬件。
- **换脑完成：** 这一微秒结束，`Task1` 彻底睡去，CPU 的 `PC` 指针已经开始在 `Task2` 的代码里狂飙了。整个过程对 `Task2` 来说，就像 `Task1` 凭空蒸发了一样。

---

3. 阻塞之后：闹钟响了怎么回来的？

既然被 Blocked 锁进了小黑屋，那 500 毫秒后它是怎么自己爬回来的呢？  这就要依靠单片机的内核嫡系定时器——**`SysTick`（系统滴答定时器）**。

- **定时器在数数：** 硬件 `SysTick` 定时器极其规律，比如每 1 毫秒就会雷打不动地产生一次硬件中断（SysTick_Handler）。
- **每毫秒对暗号：** 每次这个时钟中断爆发时，内核都会把全局时间基准 `xTickCount` 加 1。
- **死死盯着 1500：** 当某一次中断进来，内核发现 `xTickCount` 终于数到了 **`1500`**！
- **英雄出狱：** 内核立刻跑到 `Delayed Task List`（阻塞链表）里，把盖有 `1500` 戳记的 `Task1` 的 TCB **一把捞出来，重新挂回到 `Ready List`（就绪链表）中**。

**关键点：** 此时 `Task1` 的状态从 **Blocked（阻塞）** 瞬间变成了 **Ready（就绪）**。如果此时它的优先级比正在跑的 `Task2` 还要高，内核会当场把 `Task2` 踹下来，把刚刚出狱的 `Task1` 重新送进 **Running（运行态）**。CPU 硬件把 `Task1` 栈里的寄存器一恢复，`Task1` 就会睁开眼，从 `vTaskDelay(500)` 的下一行代码开始，继续开开心心地往下点灯了。

---
### 5.2.4 Suspended

挂起态 (Suspended) —— 被打入冷宫、彻底“社会性死亡”的员工

- **概念：** 任务被彻底冷冻了。除非有别的任务拿着圣旨来主动唤醒它，否则它会一辈子躺在冷宫里，**永远不会被内核调度器多看一眼**。
- **如何进出：**
    - **进冷宫：** 别的任务或它自己调用了 **`vTaskSuspend(任务句柄);`**。
    - **出冷宫：** 必须由别的任务（或者中断）主动调用 **`vTaskResume(任务句柄);`**，它才能重新回到就绪态去重新排队。
- **特点：** 阻塞态的闹钟响了或者等到了信号，会自动醒来。而**挂起态没有闹钟、不听信号，必须靠别人用代码强行唤醒**。

🔄 状态流转图（经典闭环逻辑）

这四种状态在系统运行中是高度动态互转的，可以用一张图来把它们的生老病死连起来：

```text
               vTaskSuspend() [打入冷宫]
      ┌────────────────────────────────────────┐
      │                                        ▼
【 就绪态 (Ready) 】 ◄── 闹钟响了/等到了信号 ── 【 阻塞态 (Blocked) 】
      │                                        ▲
   调度器                                   调用了 vTaskDelay()
   选中它                                   或者卡在死等队列
      │                                        │
      ▼                                        │
【 运行态 (Running) 】 ─────────────────────────┘
      │
      ▼ vTaskSuspend()
【 挂起态 (Suspended) 】 ─── 别人调用 vTaskResume() ───► 回到【就绪态】
```

# 6 Scheduler

FreeRTOS 的 Scheduler 本质就是**从所有“现在可以运行”的任务里，选一个优先级最高的，让它占 CPU。** 所以 Scheduler 最关心的不是“这个任务是谁”，而是：

```text
这个任务现在是什么状态？
它优先级多少？
```

把四个状态彻底区分开。

```text
Running
正在CPU上执行

Ready
有资格运行，但现在CPU给了别人

Blocked
暂时没资格竞争CPU，正在等时间或事件

Suspended
被人为挂起，除非显式恢复，否则不参与调度
```

最关键的是 `Ready` 和 `Blocked`。

- `Ready` 是**现在就能跑，但是CPU可能暂时给了更高优先级任务**
- `Blocked` 是**现在先不跑，要等一个条件**

比如`vTaskDelay(pdMS_TO_TICKS(500));` 意思不是“CPU原地等500ms”。而是：

```text
当前Task:
Running
↓
调用 vTaskDelay()
↓
进入 Blocked
↓
调度器去运行其他 Ready Task
```

这就是 RTOS 和普通裸机 `delay()` 最大的区别之一。普通阻塞延时大概是CPU自己在那空等，而 FreeRTOS 的 `vTaskDelay()` 是当前任务让出CPU，CPU去干别的

假设：

```c
Task1 priority = 2
Task2 priority = 1
```

一开始，假设 Task1 被调度：

```c
Task1 = Running
Task2 = Ready
```

Task1 执行`vTaskDelay(pdMS_TO_TICKS(500));`，于是：

```text
Task1:
Running → Blocked
```

调度器再看剩下的任务：

```text
Task1 = Blocked
Task2 = Ready
```

所以 Task2 运行。

即使`Task1优先级更高`也没用，因为 **Blocked 的任务根本不参与竞争**。可以把 Scheduler 想象成：

```text
所有任务
↓
先筛选 Ready
↓
在 Ready 里找最高优先级
↓
让它 Running
```

## Tick

FreeRTOS 里有一个**周期性时钟中断**，一般叫 Tick interrupt。假设`configTICK_RATE_HZ = 1000`，那么：

```text
1秒1000次Tick
1 Tick = 1 ms
```

所以`vTaskDelay(pdMS_TO_TICKS(500));`本质上就是这个任务需要等500个Tick。FreeRTOS 会记住：

```text
Task1现在进入Blocked
它应该在哪个Tick醒来
```

然后每次 Tick 中断发生，系统会更新 tick count。比如：

```text
当前Tick = 1000
Task1 delay 500
```

那么大概可以理解成`Task1 wake tick = 1500`，当 tick count 到 1500：

```text
Task1:
Blocked → Ready
```

注意，**这时候它只是先变成 Ready，不一定立刻运行**。然后 Scheduler 再判断：

```text
现在有哪些 Ready Task？
谁优先级最高？
```

如果 Task1 优先级比当前 Running Task 高，那么就可能发生抢占。

## Preemption

Preemption，**抢占式调度**。比如当前：

```text
Task2 priority = 1
Task2 Running
```

然后 Tick 到了：

```text
Task1 Blocked → Ready
Task1 priority = 2
```

Scheduler 一看Task1优先级更高，于是 Task1 会抢占 Task2。状态变成：

```text
Task2:
Running → Ready

Task1:
Ready → Running
```

这就是**抢占**。所以可以记**更高优先级任务一旦变成 Ready，就可能立刻抢占当前低优先级任务**。

## `Ready List` 和 `Blocked List`。

FreeRTOS 内部不会只放几个变量说“Task1是Ready”。它会**用链表管理任务**。可以简化理解成：

```text
Ready List
├── priority 0 的 Ready Task
├── priority 1 的 Ready Task
├── priority 2 的 Ready Task
└── ...

Blocked List
├── 等到 Tick = 1500 的 Task
├── 等到 Tick = 1800 的 Task
└── ...
```

所以一个任务调用`vTaskDelay(...)`，内部大致做的就是：

```text
从 Ready List 移走
↓
放进 Blocked/Delayed List
↓
记录唤醒时间
↓
触发重新调度
```

等时间到了：

```
从 Blocked List 移走
↓
放回对应优先级的 Ready List
```

然后 Scheduler 再从 Ready List 里选最高优先级任务。

你现在可以画成这样：

```
          xTaskCreate()
               ↓
             Ready
               ↓
          Scheduler选中
               ↓
            Running
          /    |     \
         /     |      \
vTaskDelay   被抢占   vTaskSuspend
   ↓          ↓          ↓
Blocked      Ready     Suspended
   ↓
时间/事件满足
   ↓
 Ready
```

这个图很重要。

然后讲 `PendSV`。

这个你不需要现在背汇编，只要先理解它干什么。

Scheduler 逻辑上决定：

```
现在该换任务了
```

但“真的把 CPU 从 Task1 切到 Task2”需要做很多事情：

```
保存 Task1 的 CPU 上下文
↓
找到 Task2 的栈
↓
恢复 Task2 的 CPU 上下文
↓
CPU继续从 Task2 上次停的位置执行
```

这个真正的“上下文切换”在 Cortex-M 上通常主要靠：

```
PendSV
```

来完成。

你可以先记：

```
Scheduler
= 决定“换成谁”

PendSV
= 真正执行“任务切换”
```

比如：

```
Task1 Running
↓
Task1调用 vTaskDelay()
↓
Scheduler发现Task2该运行
↓
触发PendSV
↓
保存Task1现场
↓
恢复Task2现场
↓
Task2继续执行
```

所谓“现场”大概就是：

```
寄存器
栈指针
返回地址
程序执行位置
```

这也是为什么每个 Task 必须有自己的栈。

因为任务切换时：

```
Task1的现场
存在Task1自己的栈

Task2的现场
存在Task2自己的栈
```

所以你之前 `xTaskCreate()` 里面那个：

```
128
```

不是随便给的。

它决定了这个任务有多少栈空间可以保存：

```
局部变量
函数调用
寄存器现场
中断上下文相关内容
```

最后再把 `SysTick / Tick / Scheduler / PendSV` 串一起。

你可以记成：

```
SysTick
↓
周期性产生Tick中断
↓
FreeRTOS更新Tick Count
↓
检查有没有Blocked任务到时间
↓
Blocked → Ready
↓
Scheduler判断是否需要换任务
↓
如果需要
↓
PendSV执行上下文切换
↓
新Task Running
```

这条链你以后面试也可以直接讲。

你现在最应该做的不是立刻学 Queue，而是做两个小实验，把这套调度模型彻底验证。

第一个实验：

```
Task1 priority 2
Task2 priority 1

Task1有vTaskDelay()
```

观察 Task2 仍然会运行。

第二个实验：

```
Task1 priority 2
Task2 priority 1

Task1不再调用vTaskDelay()
```

这时候如果 Task1 一直处于 Running/Ready，Task2 很可能一直没有机会运行。

这会让你理解：

```
高优先级 + 永不阻塞
≈ 低优先级任务可能被饿死
```

也就是 Starvation。

# 7 队列

## 7.1 创建队列Queue

```c
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength,
                           UBaseType_t uxItemSize);  
```

- `xQueueCreat` 函数有两个参数`uxQueueLength`和 `uxItemSize`
- `uxQueueLength`：队列能够存储的最大消息数目，即队列长度
- `uxItemSize`：队列中消息的大小，一字节为单位

返回值：如果创建成功则返回一个队列句柄（就是队列结构体的地址），用于访问创建的队列如果创建不成功则返回NULL，可能原因是创建队列需要的RAM无法分配成功。

| 内容          | 任务 Task          | 队列 Queue      |
| :-----------:| :----------------: | :-------------: |
| 本质          | 一段独立运行的代码        | 一个数据缓冲区       |
| 是否会被 CPU 执行 | 会                | 不会            |
| 作用          | 执行功能             | 传递数据          |
| 例子          | 读取传感器、BLE发送、控制电机 | 传颜色、传传感器值、传命令 |
| 由谁使用        | 调度器调度任务运行        | 任务之间读写队列      |

相当于创建了一个大数组

## 7.2  队列发送函数
```c
BaseType_t xQueueSend(QueueHandle_t xQueue,

                      const void * pvItemToQueue,

                      TickType_t xTicksToWait);
```

- `xQueue`：要写入的队列
- `pvItemToQueue`：要写入的消息（数据的地址）
- `xTicksToWait`：阻塞超时时间（当队列为满，是否需要进行阻塞等待）

返回值：

1. `pdTRUE`：写入成功
2. `errQUEUE_FULL`：队列满，写入失败

## 7.3  队列接收函数

```c
BaseType_t xQueueReceive(QueueHandle_t xQueue,

                         const void * pvBuffer,

                         TickType_t xTicksToWait);
```

- `xQueue`：要写入的队列
- `pvBuffer`：要写入的消息（数据的地址）
- `xTicksToWait`：阻塞超时时间（当队列为空，是否需要进行阻塞等待）

返回值:
1. `pdTRUE`：写入成功
2. `errQUEUE_FULL`：队列为空，写入失败

## 7.4 队列发送/接收函数中断版本

```c
BaseType_t xQueueSendFromISR (QueueHandle_t xQueue,
                             const void * pvItemToQueue,
                             BaseType_t * pvHigherPriorityTaskWoken);  

BaseType_t xQueueReceiveFromISR (QueueHandle_t xQueue,
                                 const void * pvBuffer,
                                 BaseType_t * pvHigherPriorityTaskWoken);  
```


- 中断中是不能进入阻塞态
- 中断中不能立马切换任务
- 中断是快进快出，执行的代码越少越好

# 8 信号量Semaphore

信号量 `Semaphore` 本质上是 **RTOS 里用来做同步和资源控制的机制**。信号量不是用来传具体数据的，而是用来告诉任务：“某件事发生了” 或 “某个资源现在可以用了”

我们希望**任务都是互斥的，同一个时间段一个任务只能被一个人调用** 在多任务里，每个任务一定是顺序执行的，他们各自独立，以不可预知的速度向前推进，但有时候希望多个任务能密切合作以实现一个共同的任务。绒布就是在多任务的一些关键点上可能需要互相等待和互通消息

`Task` = 人
`Queue` = 快递柜，用来放数据
`Semaphore` = 门铃，用来通知有人可以行动
`Mutex` （互斥）= 厕所门锁，同一时间只能一个人用
`Handle` = 钥匙 / 编号 / 地址
`Scheduler` = 管理员，决定谁先执行

# 9 Hook 函数

Hook 可以理解为 **FreeRTOS 预留给用户的“回调入口”** 。当 FreeRTOS 内部发生某些特定事件时，内核会主动调用用户自己实现的 Hook 函数。 基本流程：

```text
FreeRTOS内部事件
    ↓
FreeRTOS内核检测到
    ↓
调用用户实现的Hook函数
```

Hook 和中断有点像，但来源不同：

- 硬件中断：硬件事件 → NVIC → ISR
- Hook：FreeRTOS内部事件 → FreeRTOS内核 → Hook函数

常见 Hook：

1. `vApplicationStackOverflowHook()`
   - 某个**任务发生栈溢出时调用**
   - 参数可以告诉我们：
     - 哪个任务出问题
     - 任务名称是什么
2. `vApplicationMallocFailedHook()`
   - FreeRTOS动态内存分配失败时调用
   - 比如 `xTaskCreate()` 内部申请任务栈或 TCB 失败
3. `vAssertCalled()`
   - `configASSERT() `检查失败时进入
   - 用于捕获“不应该发生”的内核状态

典型处理：

```c
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();

    while(1)
    {
    }
}
```

这里：
- `(void)xTask`：表示当前故意不使用这个参数，避免编译器警告
- `taskDISABLE_INTERRUPTS()`：发生严重异常后关闭中断
- `while(1)`：让程序停在这里，方便调试

可以把 Hook 理解成：
- StackOverflowHook = 栈溢出报警器
- MallocFailedHook   = 内存申请失败报警器
- AssertHook         = 内核异常断点
