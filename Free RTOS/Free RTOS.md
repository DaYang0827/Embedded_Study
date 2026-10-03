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

# 5 创建任务Task/Thread

```c
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                        Const char* const pcName,
                        Const configSTACK_DEPTH_TYPE usStackDepth,
                        Void* const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t* const pxCreatedTask)
```

把 `xTaskCreate()` 理解成“告诉 FreeRTOS：**要创建一个以后可以被调度运行的任务，请你给它准备 TCB、栈、任务状态等管理信息**。” 它不是“现在立刻运行这个函数”，而是**先把任务创建出来**。真正开始调度，是后面的`vTaskStartScheduler();`

- `pxTaskCode`:指向任务函数的指针，注意，任务函数不能返回（即死循环）
- `pcName`：任务名，字符串
- `usStackDepth`：栈深，即任务的栈大小（单位是字，1字 = 4字节）
- `pvParameters`：任务的参数指针（即FreeRTOS 允许你给任务函数传一个“通用指针”）
- `uxPriority`：任务的优先级，最低优先级是0，数字越大，优先级越高
- `pxCreatedTask`：任务的句柄，用于控制任务

在调用的时候使用

```c
xTaskCreate(led_blink, "led_blink", 256, (void*)&LED0, 1, NULL);

xTaskCreate(led_blink, "led_blink", 256, (void*)&LED1, 1, NULL);

xTaskCreate(led_blink, "led_blink", 256, (void*)&LED2, 1, NULL);
```

进行调用，注意到第四个参数是`(void*)&LED0`，`&LED0`表示：取 LED0 这个结构体变量的地址。因为 LED0 是一个结构体变量：

```c
LED_TypeDef LED0 =
{
	GPIOB, GPIO_Pin_0, RCC_AHB1Periph_GPIOB
};
```

那么：`&LED0`类型就是：`LED_TypeDef*` 也就是“指向 `LED_TypeDef` 的指针”。`(void*)&LED0`表示：把 `LED_TypeDef*` 转换成 `void*`，传给 FreeRTOS。因为 `xTaskCreate` 第四个参数规定就是 `void*`。`void*` 可以理解为**通用地址类型**，什么类型的地址都可以先放进来。然后到了任务函数里面，再转换回来：

```c
LED_TypeDef *led = (LED_TypeDef *)args;
```

|             函数             | 含义      | 返回值     |
| :------------------------: | :-----: | :-----: |
|      `xTaskCreate()`       | 创建任务    | 有，成功或失败 |
|       `vTaskDelay()`       | 当前任务延时  | 无       |
|  `vTaskStartScheduler()`   | 启动调度器   | 无       |
| `xTaskGetSchedulerState()` | 获取调度器状态 | 有       |
|      `vTaskDelete()`       | 删除任务    | 无       |

`xTask` 和 `vTask` 不是不同任务，而是 FreeRTOS 的函数命名习惯；v 通常表示无返回值，x 通常表示有返回值，Task 表示任务管理相关函数

# Scheduler


# 6 队列

## 6.1 创建队列Queue

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

## 6.2  队列发送函数
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

## 6.3  队列接收函数

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

## 6.4 队列发送/接收函数中断版本

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

# 7 信号量Semaphore

信号量 `Semaphore` 本质上是 **RTOS 里用来做同步和资源控制的机制**。信号量不是用来传具体数据的，而是用来告诉任务：“某件事发生了” 或 “某个资源现在可以用了”

我们希望**任务都是互斥的，同一个时间段一个任务只能被一个人调用** 在多任务里，每个任务一定是顺序执行的，他们各自独立，以不可预知的速度向前推进，但有时候希望多个任务能密切合作以实现一个共同的任务。绒布就是在多任务的一些关键点上可能需要互相等待和互通消息

`Task` = 人
`Queue` = 快递柜，用来放数据
`Semaphore` = 门铃，用来通知有人可以行动
`Mutex` （互斥）= 厕所门锁，同一时间只能一个人用
`Handle` = 钥匙 / 编号 / 地址
`Scheduler` = 管理员，决定谁先执行

# 8 Hook 函数

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
