
提问：
1. 我现在下一步应该学什么    进行哪方面的提升
2. 怎么更好的用ai    发现用ai会学的深和远    怎么把握度
3. 怎么看待现在的就业环境       只能一直内卷吗
4. 怎么平衡学技术和生活


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
- 通常是这个架构下高效的**有符号整数类型**
- BaseType_t   ≈ int32_t
- UBaseType_t  ≈ uint32_t
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
## 3.4 Handle

**句柄不是那个对象本身，而是“找到那个对象的一个引用/标识”。** 在 FreeRTOS 里，大多数 Handle 本质上通常是某种指针类型。

### 3.4.1 TaskHandle_t 

```c
TaskHandle_t task1_handle;
```

然后：

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

这里最容易混的是`send_task1`,`task1_handle` 这两个完全不是一回事。

```text
send_task1
→ 任务函数
→ Flash里的代码入口

task1_handle
→ 句柄变量
→ 用来找到“这个具体任务实例”
```

---

**不能直接拿函数名控制 Task**

比如：

```c
void led_task(void *arg)
{
    while(1)
    {
        ...
    }
}
```

可能用这个函数创建两个 Task：

```c
xTaskCreate(led_task, "LED1", 128, &led1, 1, &handle1);
xTaskCreate(led_task, "LED2", 128, &led2, 1, &handle2);
```

注意，任务函数只有一个led_task。但创建出来的是Task实例1、Task实例2。它们分别有：

```text
不同TCB
不同Task Stack
不同参数
不同运行状态
```

所以led_task根本不能唯一代表其中某一个 Task。这时候`handle1`、`handle2` 才负责区分。所以以后`vTaskSuspend(handle1);` 意思是找到 handle1 对应的那个 Task 实例，把它挂起。

---

### 3.4.2 Handle 和 TCB 的关系

暂时理解为：

```text
TaskHandle_t
↓
指向/引用某个 Task 的 TCB
```

比如：

```text
task1_handle
        │
        ↓
┌──────────────────┐
│ Task1 的 TCB      │
│                  │
│ pxTopOfStack     │
│ uxPriority       │
│ pcTaskName       │
│ ...              │
└──────────────────┘
```

所以调`vTaskPrioritySet(task1_handle, 3);` FreeRTOS 就能顺着句柄找到这个 Task 的管理信息，然后修改优先级。

**Handle 是供 API 使用的对象引用，不是对象本身。**

### 3.4.3 `TaskHandle_t *pxCreatedTask`

`xTaskCreate()` 最后一个参数是`TaskHandle_t *pxCreatedTask` 注意这里：

```text
TaskHandle_t
→ Handle类型

TaskHandle_t *
→ 指向Handle变量的指针
```

需要“指向 Handle 的指针”因为 FreeRTOS 想修改调用者的`task1_handle` 所以要传`&task1_handle` 。定义`TaskHandle_t task1_handle;`这是一个变量。 `xTaskCreate()` 需要**把“新建出来的 Task Handle”写回变量**。也就是说：

```text
提供：
task1_handle 这个变量的地址

FreeRTOS：
创建Task
↓
获得Task的Handle
↓
写入 task1_handle
```

类似：

```c
void set_value(int *p)
{
    *p = 100;
}
```

调用：

```c
int a;
set_value(&a);
```

是同一个思路。

### 3.4.4 FreeRTOS 喜欢 Handle

因为这样可以做到：

```text
API
只需要知道一个Handle
↓
不用把内部结构全部暴露给用户
```

比如 FreeRTOS 不希望直接`task1_tcb->uxPriority = 5;` 而希望`vTaskPrioritySet(task1_handle, 5);`。好处很多：

```text
封装
隐藏内部实现
接口统一
用户不需要知道内部结构
以后内核结构变化也更容易维护
```

这个和

```text
flash.h
→ 暴露接口

flash.c
→ 隐藏实现
```

其实是同一个工程思想。


### 3.4.5 Handle 和普通指针的关系

很多 FreeRTOS Handle 底层确实就是指针类型。比如概念上可以类似：

```c
typedef struct tskTaskControlBlock * TaskHandle_t;
```

所以`TaskHandle_t handle;`，看起来就类似`struct tskTaskControlBlock *handle;` 但工程里不要老想着Handle 就一定等于某个具体结构体裸指针。更好的理解是**Handle 是 API 暴露给用户的“对象引用类型”。**

库作者不希望依赖内部具体结构。

---

handle与普通指针的区别

```text
函数指针
→ 指向函数代码
→ 用来“执行行为”

Handle
→ 指向/引用一个系统对象
→ 用来“找到并操作对象”
```

例如`void (*callback)(uint8_t);` 是函数指针。而`TaskHandle_t task_handle;` 是Task Handle。

前者最终`callback(data);` 会跳去执行函数。 后者`vTaskSuspend(task_handle);`是把这个引用传给 FreeRTOS，让内核找到对应 Task。所以一个是“**去哪执行代码**”，另一个是“**要操作哪个对象**”

### 3.4.6 Handle 通常初始化为 NULL

比如：

```c
TaskHandle_t task_handle = NULL;
```

创建前：

```text
task_handle
→ 还没指向有效Task
```

创建成功后：

```text
task_handle
→ 有效Handle
```

所以工程里经常：

```c
if (task_handle != NULL)
{
    vTaskSuspend(task_handle);
}
```

这和 callback 的：

```c
if (callback != NULL)
```

很像。区别只是：

```text
callback
→ 函数指针

task_handle
→ 对象句柄
```

### 3.4.7 完整例子

```c
TaskHandle_t led_handle = NULL;

void led_task(void *arg)
{
    while(1)
    {
        LED_Toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void)
{
    xTaskCreate(
        led_task,
        "LED",
        128,
        NULL,
        1,
        &led_handle
    );

    vTaskStartScheduler();
}
```

这里可以按角色分：

```
led_task
→ Task函数
→ Flash里的代码

led_handle
→ Handle变量

&led_handle
→ Handle变量自己的地址

xTaskCreate()
→ 创建Task实例
→ 创建TCB/Stack
→ 把Handle写回led_handle
```

以后`vTaskSuspend(led_handle);` FreeRTOS：

```text
拿到led_handle
↓
找到对应Task
↓
修改它的状态
↓
移出Ready相关调度结构
↓
进入Suspended
```

---

 Handle 固定成一句话 **Handle 是“用来找到某个 FreeRTOS 对象的引用”。**

然后分对象：

```
TaskHandle_t
→ 找Task

QueueHandle_t
→ 找Queue

SemaphoreHandle_t
→ 找Semaphore/Mutex

TimerHandle_t
→ 找Software Timer
```


```
函数名
→ 函数入口地址

函数指针
→ 保存函数地址

TCB
→ Task管理对象

TaskHandle_t
→ 找到某个TCB/Task实例

TaskHandle_t *
→ 指向Handle变量
→ 常用于让函数把Handle写回来
```

# 4 TCB
## 4.1 概念

**TCB** 的全称是 **Task Control Block（任务控制块）**。它的本质**就是 FreeRTOS 给每一个任务专门发放的“身份证/档案袋”。** 它是一个极其复杂的 C 语言**结构体（Struct）**。为了在多任务来回切换时实现“瞒天过海”的效果，每个任务在内存里都会躺着一个专属于自己的 TCB。

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

- 记录创建任务时填的数字（比如填的 `1`）。FreeRTOS 的核心调度器查找谁的优先级大，CPU 硬件的 PC 指针下一微秒就会去执行谁的任务函数。

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

- 使用 `xTaskCreate()` 动态创建 Task 时，FreeRTOS 通常通过配置的 heap 实现为 TCB 和 Task Stack 分配 RAM。
    - 第一块地盘：用来存放任务干活开辟局部变量用的**栈空间（Stack）**（填的 128 字）。
    - 第二块地盘：**就是一个正好能装下 `tskTCB` 结构体大小的内存块（通常几十个字节）**。
- 所以，TCB 是在程序运行起来后，在 **RAM 的堆区里动态申请、静态维护**的死忠档案。

总结：

- **Task Function (任务函数)**：是放在 Flash 里的**死指令代码**。
- **Stack (任务栈)**：是放在 RAM 里用来**存局部变量和恢复寄存器**的临时干粮仓库。
- **TCB (任务控制块)**：是操作系统内核握在手里的**遥控器和绝密档案**。它通过记录每个任务的栈顶指针（`pxTopOfStack`）和优先级，实现了在多任务之间“移形换影”的闭环调度。

## 4.2 TCB 与 Task Stack 的区别

`xTaskCreate()` 创建一个任务时，**不是创建两个 Stack，而是通常为这个任务准备一个 TCB + 一块独立的 Task Stack**。

```text
Task 实例
│
├── TCB
│   ├── pxTopOfStack
│   ├── uxPriority
│   ├── xStateListItem
│   ├── pcTaskName
│   └── 其他任务管理信息
│
└── Task Stack
    ├── 局部变量
    ├── 函数调用现场
    ├── 返回地址
    └── 任务切换时保存的 CPU 上下文
```

所以：

```text
TCB ≠ Stack
TCB = 任务管理结构体
Task Stack = 任务自己的栈空间
```

使用 `xTaskCreate()` 动态创建任务时，可以理解：

```text
xTaskCreate()
↓
为任务准备 TCB
+
为任务准备 Task Stack
↓
建立任务的初始状态
↓
进入 Ready 相关调度结构
```

例如 STM32F4 上：

```c
xTaskCreate(task1, "TASK1", 128, NULL, 1, &task1_handle);
```

如果 `sizeof(StackType_t) = 4 Byte`，那么 128 表示大约 `512 Byte` 的 Task Stack；除此之外还要有一块 RAM 用来存 Task1 的 TCB。

### 4.2.1 TCB 和任务现场的关系

任务被切走时，不是把所有寄存器都直接塞进 TCB。更准确的是：

```text
CPU 上下文
↓
主要保存在这个 Task 自己的 Stack 中

TCB->pxTopOfStack
↓
记录“这个任务的栈顶现在在哪里”
```

可以记**TCB 负责“记录到哪里找”，Task Stack 负责“真正保存任务现场”**。以后任务重新运行：

```text
Scheduler 选中 Task1
↓
找到 Task1 的 TCB
↓
读取 pxTopOfStack
↓
找到 Task1 的 Task Stack
↓
恢复寄存器 / PC / LR 等上下文
↓
Task1 从之前被打断的位置继续执行
```

这也是为什么任务被抢占以后，不需要从任务函数开头重新执行。


# 5 Task/Thread
## 5.1 TaskCreat

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

把 `xTaskCreate()` 理解成“告诉 FreeRTOS：**要创建一个以后可以被调度运行的任务，请你给它准备 TCB、栈、任务状态等管理信息**。” 它不是现在立刻运行这个函数，而是**先把任务创建出来**。真正开始调度，是后面的`vTaskStartScheduler();`

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
        vTaskDelay(pdMS_TO_TICKS(500));
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

`Task1` 调用 `vTaskDelay(pdMS_TO_TICKS(500));` 进入 **Blocked（阻塞态）** 的这个过程，可以用一句话来概括其本质 **“Task1 主动卸下肉身，把自己的 TCB 锁进一间小黑屋，给自己定了一个 500 毫秒的硬核闹钟，并把 CPU 这把车钥匙主动让给了其他人。”**

当写下 `vTaskDelay(pdMS_TO_TICKS(500));` 的这一微秒内，系统内部发生了：

1. 软件层面：移形换影的“链表搬运”

   在 FreeRTOS 内核的 RAM（堆区）里，维护着好几条像排队队列一样的 **“链表（List）”**：

- **Ready List（就绪链表）：** 里面挂着所有随时准备干活的任务 TCB。
- **Delayed Task List（延时/阻塞链表）：** 里面挂着所有正在睡觉、等时间的任务 TCB。

当 `Task1` 正在执行并一脚踩中 `vTaskDelay(pdMS_TO_TICKS(500));` 时：

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

**关键点：** 此时 `Task1` 的状态从 **Blocked（阻塞）** 瞬间变成了 **Ready（就绪）**。如果此时它的优先级比正在跑的 `Task2` 还要高，内核会当场把 `Task2` 踹下来，把刚刚出狱的 `Task1` 重新送进 **Running（运行态）**。CPU 硬件把 `Task1` 栈里的寄存器一恢复，`Task1` 就会睁开眼，从 `vTaskDelay(pdMS_TO_TICKS(500));` 的下一行代码开始，继续开开心心地往下点灯了。

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

## 5.3 Starvation

**Starvation（任务饥饿）** 是一个非常经典、极其危险的**多任务并发逻辑灾难**。它的本质是**由于低优先级的任务永远分配不到 CPU 的车钥匙，它在就绪态（Ready）里被活活憋死，一辈子没有机会进入运行态（Running）去执行一行代码。**

1. starvation的发生

   FreeRTOS 的内核调度器是一个极其冷酷、**绝对严格遵守“优先级强占”** 的机器人。假设你的工程里有三个任务：

- `Task_High` (优先级 = 3，**高级大佬**，负责紧急串口接收)
- `Task_Mid` (优先级 = 2，**中级员工**，负责不停地处理数据)
- `Task_Low` (优先级 = 1，**低级苦力**，点灯任务)

触发饥饿的致命错误写法：中级员工 `Task_Mid` 的死循环里，写出了类似于裸机的死等，或者**完全没有调用任何能让自己进入 Blocked（阻塞态）的函数**（比如忘了写 `vTaskDelay`）：

```c
void Task_Mid(void *pvParameters)
{
    while(1)
    {
        // 哼哧哼哧处理数据，但没有任何 vTaskDelay()，也没有等信号量
        process_data(); 
    }
}
```

此时低级苦力 `Task_Low` 的悲惨遭遇：

1. 每当系统时钟节拍（SysTick）爆发，内核调度器就会去扫描 `Ready List`。
2. 调度器发现：`Ready List` 里躺着优先级为 2 的 `Task_Mid` 和优先级为 1 的 `Task_Low`。
3. 根据 ARM 内核铁律，调度器大喊：**“谁的优先级大，CPU 就给谁！”** 于是，车钥匙永远被递给了 `Task_Mid`。
4. 因为 `Task_Mid` 内部永远不调用 `vTaskDelay`，它**永远不会主动进入 Blocked（阻塞态）**。它一辈子都挂在 `Ready List` 里或者直接霸占着 CPU。
5. **最终结果：** 只要 `Task_Mid` 还活着，排在它后面的 `Task_Low`（点灯任务）**在就绪链表里就会被永远忽略。** 这个可怜的点灯任务就处于 **Starvation（饥饿）** 状态。在电脑屏幕前看起来，你的板子就像死机了一样，LED 灯永远不会闪烁！

---

2. 任务饥饿与裸机死等的区别

   任务饥饿不是裸机里的死循环卡死，而是在实时操作系统里呈现出更诡异的特征：

- **全芯片没有死机：** 此时高级大佬 `Task_High` 如果突然来了一个串口中断，因为它的优先级（3）比中级员工（2）高，大佬依然可以**瞬间强占 CPU**，串口接收依然完美工作！
- **只有局部社会性死亡：** 芯片其他地方都在动，唯独低优先级任务（比如屏幕刷新、点灯提示、按键响应）被彻底冻结了。这在商业产品中是一个极其隐蔽且致命的 Bug（比如设备还在后台联网传数据，但前台屏幕按键全部点不动，用户以为坏了）。

---

3. 工业界解决“任务饥饿”的黄金法则

   为了不让低优先级的任务被活活饿死，在架构设计时必须遵守以下三条行业保命底线：

   法则一：所有任务的死循环里，必须包含“能让自己躺下”的机制（最重要！）

   任何任务（除了故意设计的空闲任务），只要占用了 CPU，干完当前这一轮的活之后，**必须通过 `vTaskDelay()`、等待队列（`xQueueReceive`）、或等待信号量，强行把自己踢进 【Blocked（阻塞态）】**。  只要任务躺下，`Ready List` 里高优先级的恶霸消失了，调度器才会把目光往下看，把 CPU 施舍给最底层的 `Task_Low`，让它喘一口气去点个灯。

   法则二：同等优先级轮转（Time Slicing）

   如果你有两个发送任务 `send_task1` 和 `send_task2`，**强烈建议把它们的优先级设为完全一样（比如都等于 1）**。  在 `FreeRTOSConfig.h` 中，默认开启了 `#define configUSE_TIME_SLICING 1`。这样，当两个任务优先级一样且都不肯睡觉时，内核时钟会化身无情裁判，每隔 1 毫秒（一个时间片）强行把当前的人踹下来，换另一个人上去。两个任务你吃一口我吃一口，**谁都不会发生饥饿**。

   法则三：看门狗与高阶的“优先级继承”

   如果低优先级任务不仅被饿死了，还手里死死攥着某个锁（互斥量），导致高优先级任务也在等它，这就会引发更恐怖的灾难——**优先级翻转（Priority Inversion）**。FreeRTOS 内核通过互斥量自带的“优先级继承”机制，在低优先级任务被饿死前强行拉它一把，帮它快速干完活释放锁。

## 5.4 任务常用的API

### 5.4.1 TaskDelay

 `vTaskDelay()` 不是“CPU 停在那里等”，而是“**当前 Task 主动进入 Blocked 状态，把 CPU 让给其他 Task**”。比如：

```c
void Task1(void *arg)
{
    while (1)
    {
        LED_Toggle();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

执行过程大概是：

```text
Task1 Running
↓
LED翻转
↓
调用 vTaskDelay(1000ms)
↓
Task1 进入 Blocked
↓
Scheduler 选择其他 Ready Task
↓
1秒后 Task1 重新进入 Ready
↓
等待调度
↓
再次 Running
```

所以它和裸机里的“死等”完全不是一个思路。

#### 5.4.1.1 与普通 delay 的区别

裸机里写`delay_ms(1000);`，或者`for (volatile int i = 0; i < 1000000; i++);`，这种通常是：

```text
CPU一直执行延时代码
↓
CPU被占着
↓
其他事情干不了
```

而`vTaskDelay(...)`是：

```text
当前Task暂时不需要CPU
↓
进入Blocked
↓
CPU去运行其他Task
```

所以 FreeRTOS 强调的是**不要没事占着 CPU**。

---

#### 5.4.1.2 `vTaskDelay()` 参数

函数原型`void vTaskDelay(const TickType_t xTicksToDelay);` **参数是Tick 数，不是毫秒**。比如`vTaskDelay(1000);` 并不一定是 1000 ms。取决于`configTICK_RATE_HZ`

假设`#define configTICK_RATE_HZ 1000`，那么`1 tick = 1 ms`，这时候`vTaskDelay(1000);` 才大约是`1000 ms`

---
 
参数更推荐使用 `pdMS_TO_TICKS()`

```c
vTaskDelay(pdMS_TO_TICKS(1000));
```

意思就是我要延时 1000 ms，请帮我转换成对应的 Tick 数。这样即使以后`configTICK_RATE_HZ`改了，代码也不容易出错。

比如`configTICK_RATE_HZ = 100`，那么`1 tick = 10 ms`，`pdMS_TO_TICKS(1000)` 会转换成大约`100 ticks`


#### 5.4.1.3 Task  delay 时的状态

FreeRTOS Task 常见状态：

```text
Running
Ready
Blocked
Suspended
```

`vTaskDelay()` 会让：

```text
Running
↓
Blocked
```

时间到了以后：

```text
Blocked
↓
Ready
```

注意**时间到了不一定立刻 Running**。因为可能有更高优先级任务正在运行。所以准确说法是：

```text
delay结束
→ Task进入Ready
→ 等Scheduler调度
```

不是`delay结束→ 马上运行`

#### 5.4.1.4 与 Scheduler 的关系

假设：

```text
TaskA priority = 2
TaskB priority = 1
```

TaskA 正在运行。如果 TaskA`vTaskDelay(pdMS_TO_TICKS(1000));`，那么`TaskA → Blocked`于是它不能继续运行。Scheduler 就会找`Ready状态中优先级最高的Task`

于是`TaskB开始运行`，一秒后`TaskA Blocked → Ready` 因为 TaskA 优先级更高，所以在合适的调度点，它可能重新抢占 TaskB。

---

 `vTaskDelay()` 比忙等待要好

对比一下。

忙等待：

```c
while (time_not_up)
{
}
```

CPU：`100%一直在这个Task里`，而`vTaskDelay(...)` CPU：

```text
当前Task睡眠
↓
其他Task运行
↓
如果没有Task
↓
Idle Task运行
```

以后如果启用低功耗，Idle 阶段还可以进一步省电。

#### 5.4.1.5 `vTaskDelay()` 适应场景

例如：

```c
void LedTask(void *arg)
{
    for (;;)
    {
        LED_Toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

非常适合：

```text
周期性LED
周期传感器读取
周期状态刷新
低频日志输出
```

但有一个重要问题`vTaskDelay()` 不适合要求非常严格周期的任务。这就会引出`vTaskDelayUntil()`

#### 5.4.1.6 `vTaskDelay()` 的周期会漂移

```c
while (1)
{
    do_work();
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

假设：

```text
do_work()用了100ms
delay用了1000ms
```

那么实际周期是`100ms + 1000ms = 1100ms` ，下一次再来又`1100ms` 所以时间会慢慢偏。

#### 5.4.1.7 `vTaskDelayUntil()` 

如果要求每隔 1000ms 精确执行一次。就更适合`vTaskDelayUntil()`,典型：

```c
TickType_t lastWakeTime;

lastWakeTime = xTaskGetTickCount();

while (1)
{
    do_work();

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(1000)
    );
}
```

它不是`从现在开始再等1000ms`，而是`以上一次计划唤醒时间为基准`，所以：

```text
第0次：0ms
第1次：1000ms
第2次：2000ms
第3次：3000ms
```

更适合固定周期任务。


### 5.4.2 TaskSuspended

```c
vTaskSuspend(TaskHandle_t xTask);
```

作用是**把某个任务人为地挂起，让它彻底退出调度。** 比如：

```c
TaskHandle_t task1_handle;

xTaskCreate(
    send_task1,
    "TASK1",
    128,
    NULL,
    2,
    &task1_handle
);
```

这里`&task1_handle`，就是让 FreeRTOS 把 Task1 的句柄保存下来。以后就可以`vTaskSuspend(task1_handle);` 把 Task1 挂起。

状态变化是：

```text
Running / Ready
        ↓
     Suspended
```

一旦进入 `Suspended` 这个任务不再参与 Scheduler 的竞争，哪怕它优先级是 100，也没用。

比如：

```c
Task1 priority = 2
Task2 priority = 1
```

正常情况下 Task1 优先级更高。但如果`vTaskSuspend(task1_handle);` 那么：

```text
Task1 = Suspended
Task2 = Ready
```

Scheduler 只能运行 Task2。

#### 5.4.2.1 使用句柄的原因

不使用函数名，而是使用函数句柄是**因为函数名代表的是放在 Flash 里的“死指令代码（一间样板房）”；而句柄代表的是动态躺在 RAM 里的“活任务实体（根据样板房盖好的真实大楼和它的遥控器）”。**

1. 致命原因一：同一个函数名，可能对应着“多个不同的任务实例”

   在真正的工程中，**同一个任务函数名，是可以被多次调用来创建出好几个完全独立运行的任务的**。举个最直观的例子：  写一个通用的 LED 闪烁函数，名字叫 `void LED_Task_Function(void *pvParameters)`。  在 `main` 函数里，用这**同一个函数名**，连续创建了三个任务：

- 任务 A（控制红灯闪烁）
- 任务 B（控制绿灯闪烁）
- 任务 C（控制蓝灯闪烁）

  此时在 RAM 的堆区（Heap）里，操作系统内核会强行圈出 **3 个完全独立的 TCB 档案袋** 和 **3 块独立的任务栈空间**。

  **恐怖推演：**  
  如果现在想把“红灯任务（任务A）”打入冷宫挂起，假如 FreeRTOS 允许你直接写 `vTaskSuspend(LED_Task_Function);`。内核调度器拿着这个函数名（Flash 里的一个首地址）直接就懵了：“三个函数共用同一个代码名字，到底是要把红灯、绿灯还是蓝灯挂起？该去把哪一个 TCB 丢进冷宫链表？”

  所以，函数名在这里**彻底失去了唯一指向性**。而**句柄（Handle）**，本质上就是指向那个在 RAM 里唯一的、专属 TCB 档案袋的**指针（地址）**。给调度器递过去任务 A 的句柄，调度器就能精准无误地把红灯的 TCB 锁进冷宫。

---

2. 致命原因二：函数名无法记录任务在运行中途的“活状态”

- **函数名（Flash 地址）：** 它是静态的、只读的。里面只有写好的 C 语言机器码（比如怎么做转换、怎么发串口）。不管这个任务是正在跑、还是在睡觉、还是被挂起，**函数名在 Flash 里的数据一辈子都不会发生任何改变**。
- **句柄（指向 RAM 里的 TCB）：** 它是动态的、实时的。任务控制块里记录着 `pxTopOfStack`（当前执行到哪一行的栈顶指针）、`uxPriority`（优先级）以及它当前被挂在哪个链表里。

  当想要挂起（Suspend）一个任务时，内核调度器必须要去改写这个任务的活状态——要把它的 TCB 从就绪链表解下来，挂到挂起链表里。

- 如果给内核一个函数名（Flash 地址），由于 Flash 在运行时是**只读、无法直接写入**的，内核根本没有办法在 Flash 里面去修改和记录这个任务的实时排队状态。
- 只有通过句柄找到 RAM 堆区里的 TCB，内核才能用指针“啪”地一下把状态改掉，完成挂起操作。

---

3. 代码实战对照：它们在代码里是怎么产生关联的？

   在写创建和挂起代码时，句柄和函数名是这样各司其职的：

```c
// 1. 先定义一个句柄变量（本质上是一个指向 TCB 的指针马甲）
TaskHandle_t MyTaskHandle = NULL; 

int main(void)
{
    xTaskCreate(
        LED_Task_Function,  // 参数 1：填函数名。告诉内核“去哪里读点灯的代码指令”
        "LED_Task",         
        128,                
        NULL,               
        1,                  
        &MyTaskHandle       // 参数 6：【核心】把定义好的句柄地址传进去！
                            // 创建成功后，FreeRTOS 会把这个任务在 RAM 里新开辟的
                            // TCB 专属档案袋的绝对内存地址，死死写入到 MyTaskHandle 里！
    );
    
    vTaskStartScheduler();
}

// 在另一个发送任务里，想把点灯任务打入冷宫：
void send_task1(void *pvParameters)
{
    while(1)
    {
        // 挂起时，必须递交句柄！
        // 内核顺着 MyTaskHandle 里存的 RAM 地址，直接精准揪出点灯任务的 TCB 丢进冷宫
        vTaskSuspend(MyTaskHandle); 
        
        vTaskDelay(2000);
    }
}
```

可以把它们的关系彻底固化在脑海里：

- **函数名**：是 **“设计图纸”** 。多个一模一样的房子（任务实例）可以按照同一张图纸来盖。
- **TCB 内存块**：是盖好的 **“真实房子”**。
- **句柄**：是这栋房子的 **“门牌号/钥匙”**。

想向房子里搬家具（挂起、改变运行状态），不能对着设计图纸（函数名）使劲，必须拿着具体的门牌号（句柄），去 RAM 里找到那栋属于它的真实大楼！

### 5.4.3 TaskResume

```c
vTaskResume(TaskHandle_t xTask);
```

作用是**把被 Suspend 的任务重新恢复回来。** 比如：

```c
vTaskResume(task1_handle);
```

状态：

```text
Suspended
    ↓
  Ready
```

注意`Resume` 以后不是一定立刻 Running。它只是先变成Ready，然后 Scheduler 再判断优先级。如果恢复的 Task1：`priority = 2` ，当前 Task2：`priority = 1` 那 Task1 很可能马上抢占 Task2：

```text
Task1:
Suspended → Ready → Running

Task2:
Running → Ready
```

---

注意**区分 Blocked 和 Suspended**，它们表面上都像“这个任务现在不运行”，但本质完全不同。

```text
Blocked
= 等条件
= 条件满足后自动回来

Suspended
= 人为暂停
= 必须显式 Resume 才回来
```

比如`vTaskDelay(pdMS_TO_TICKS(500));` ，Task1：

```text
Running
↓
Blocked
↓
500ms到
↓
Ready
```

它是自动恢复。

而`vTaskSuspend(task1_handle);`，Task1：

```text
Running
↓
Suspended
↓
一直停着
```

只有`vTaskResume(task1_handle);` 之后才：

```
Suspended → Ready
```

可以直接记 **`Blocked` 是“等东西”，`Suspended` 是“被人为暂停”**。

### 5.4.4 TaskYIELD

```c
taskYIELD();
```

这个更容易误解。它的意思不是“把 CPU 交给低优先级任务。”它真正的意思是**当前任务主动要求 Scheduler 重新调度一次。** 比如：

```c
void send_task1(void *pvParameters)
{
    while(1)
    {
        usart_send_string(&usart1, "Task1\r\n");

        taskYIELD();
    }
}
```

执行`taskYIELD();` 之后：

```text
当前 Task1 主动放弃当前这次 CPU 使用机会
↓
Scheduler重新选择
```

但如果：

```text
Task1 priority = 2
Task2 priority = 1
```

而且两个都是 Ready：

```text
Task1 = Ready priority 2
Task2 = Ready priority 1
```

Scheduler 一看最高优先级还是 Task1，那结果很可能还是Task1继续运行，所以`taskYIELD();` **不能解决低优先级 Task2 饥饿的问题。**这个点很重要。

---

 `taskYIELD()` 典型情况是在两个任务优先级相同时有意义

比如：

```text
Task1 priority = 2
Task2 priority = 2
```

Task1 调`taskYIELD();` 调度器重新选同优先级 Ready Task，Task2 就可能获得 CPU。可以把它理解成：

```text
“我这次先让一下，
你重新看看还有没有同优先级的人该跑。”
```

---

所以这三个 API 的区别可以直接这样记：

```
vTaskDelay()
→ 我等一段时间
→ Blocked
→ 时间到了自动 Ready

vTaskSuspend()
→ 把任务人为停掉
→ Suspended
→ 不会自动回来

vTaskResume()
→ 把 Suspended 任务恢复
→ Ready

taskYIELD()
→ 当前任务主动要求重新调度
→ 自己仍然是 Ready
```

特别注意 `taskYIELD()`：

```text
不会进入 Blocked
不会进入 Suspended
```

只是Running → Ready → Scheduler，重新选如果它依然是最高优先级又可能马上 Running

## 5.5 主动任务切换

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

## 6.1 Tick

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

## 6.2 Preemption

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

### 6.2.1 抢占、同优先级轮转与 taskYIELD

调度时要分成 **“不同优先级”和“同优先级”** 两种情况。

#### 6.2.1.1 不同优先级的 Preemption

假设：

```text
Task_Low   priority = 1   Running
Task_High  priority = 3   Blocked
```

某个事件让高优先级任务：

```text
Task_High: Blocked → Ready
```

如果 `configUSE_PREEMPTION = 1`，那么高优先级任务可以立即抢占低优先级任务：

```text
Task_Low:  Running → Ready
Task_High: Ready   → Running
```

这里最重要的是**被抢占的低优先级任务不会进入 Blocked，而是回到 Ready**。因为它并没有在等时间或事件，只是暂时失去了 CPU。

等高优先级任务以后进入 Blocked、Suspended，或者不再占用 CPU 时，LowTask 仍然是 Ready；如果此时它成为最高优先级的 Ready Task，就会：

```text
Ready → Running
```

并**从之前被打断的位置继续执行**。

#### 6.2.1.2 同优先级使用 Time Slicing

假设：

```c
TaskA priority = 2
TaskB priority = 2
```

两个任务都 Ready，并且 `configUSE_TIME_SLICING = 1`。那么即使 TaskA 不主动 Block、也不调用 `taskYIELD()`，Tick 到来时，同优先级任务之间也可以进行时间片轮转：

```text
TaskA Running
TaskB Ready
↓ Tick
TaskA Ready
TaskB Running
```

所以：

```text
configUSE_PREEMPTION
→ 主要决定高优先级 Ready 后能不能抢占低优先级 Running

configUSE_TIME_SLICING
→ 主要决定同优先级 Ready Task 是否按 Tick 轮转
```

如果关闭时间片：

```c
configUSE_TIME_SLICING = 0
```

同优先级任务不会因为 Tick 自动轮转。此时通常要等当前任务主动 Block，或者调用 `taskYIELD()`，其他同优先级任务才有机会运行。

#### 6.2.1.3 `taskYIELD()`

`taskYIELD()` 的本质是**当前 Task 主动请求 Scheduler 重新调度**。它不会进入 Blocked，也不会进入 Suspended。可以理解成：

```
Running → Ready → Scheduler重新选择
```

如果存在同优先级 Ready Task，对方就可能获得 CPU；如果当前任务仍然是最高优先级，它也可能很快再次 Running。

### 6.2.2 被抢占以后为什么还能继续执行

例如低优先级任务运行到一半，高优先级任务变成 Ready：

```
LowTask Running
↓
保存 LowTask 上下文
↓
LowTask → Ready
↓
HighTask → Running
```

保存的上下文包括 PC、LR、通用寄存器、xPSR、栈相关信息等。上下文主要保存在 LowTask 自己的 Task Stack 中，TCB 通过 pxTopOfStack 等成员记录任务栈的位置。

以后 LowTask 再次被调度：

```
Scheduler 选中 LowTask
↓
通过 TCB 找到 pxTopOfStack
↓
从 LowTask Stack 恢复上下文
↓
恢复 PC
↓
继续从之前被抢占的位置执行
```

一句话：

> 任务切换不是“重新运行函数”，而是“保存现场 → 切走 → 恢复现场”。

## 6.3 `Ready List` 和 `Blocked List`。

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

```text
从 Blocked List 移走
↓
放回对应优先级的 Ready List
```

然后 Scheduler 再从 Ready List 里选最高优先级任务。可以画成这样：

```text
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

## 6.4 PendSV

Scheduler 逻辑上**决定现在该换任务了**。但“真的把 CPU 从 Task1 切到 Task2”需要做很多事情：

```text
保存 Task1 的 CPU 上下文
↓
找到 Task2 的栈
↓
恢复 Task2 的 CPU 上下文
↓
CPU继续从 Task2 上次停的位置执行
```

这个真正的“上下文切换”在 Cortex-M 上通常主要靠`PendSV`来完成。可以先记：

```text
Scheduler
= 决定“换成谁”

PendSV
= 真正执行“任务切换”
```

比如：

```text
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

```text
寄存器
栈指针
返回地址
程序执行位置
```

这也是为什么**每个 Task 必须有自己的栈**。因为任务切换时：

```text
Task1的现场
存在Task1自己的栈

Task2的现场
存在Task2自己的栈
```

所以 `xTaskCreate()` 里面那个`128`不是随便给的。它决定了这个任务有多少栈空间可以保存：

```text
局部变量
函数调用
寄存器现场
中断上下文相关内容
```

把 `SysTick / Tick / Scheduler / PendSV` 串一起。可以记成：

```
SysTick

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

- Blocked = 暂时退出CPU竞争
- Ready = 有资格运行但还没拿到CPU
- Running = 当前正在CPU执行
- Scheduler = 从Ready任务里挑最高优先级
- Tick = 提供系统时间基准
- PendSV = 真正执行上下文切换


# 7 Queue

## 7.1 概念

- `xQueueSend()` = 往队列里**复制一份数据**。  
- `xQueueReceive()` = 从队列里**取出一份数据，并复制到你的变量里**。

它们传递的是**数据副本**，不是“把原变量整个搬走”。Queue 是**任务之间传递数据的缓冲区，而且它还能让任务在“没有数据”或“没有空间”时进入 Blocked**。先把它想成：

```text
Task1 生产数据
    ↓
 [ Queue ]
    ↓
Task2 消费数据
```

最简单的例子：

```text
Task1 每500ms产生一个数字
↓
Queue
↓
Task2拿到这个数字并打印
```

Queue不仅存数据，还维护“**有几个数据没被取走**”。比如：

```text
Queue 长度 = 5

初始：
[ ][ ][ ][ ][ ]

Task1发送 10：
[10][ ][ ][ ][ ]

再发送20：
[10][20][ ][ ][ ]

Task2接收：
取出10

变成：
[20][ ][ ][ ][ ]
```

FreeRTOS Queue 默认是 **FIFO**（First In First Out） 先进先出，所以先进去的先出来。

Queue 最关键的三个 API ：

```c
xQueueCreate()
xQueueSend()
xQueueReceive()
```

## 7.2 QueueCreate

```c
QueueHandle_t xQueueCreate( 
    UBaseType_t uxQueueLength,   /* 队列深度：最多能存放多少个数据项 */
    UBaseType_t uxItemSize       /* 每个数据项的大小（单位：字节） */
);

```

- `xQueueCreat` 函数有两个参数`uxQueueLength`和 `uxItemSize`
- `uxQueueLength`：队列能够存储的最大消息数目，即队列长度
- `uxItemSize`：队列中消息的大小，一字节为单位

返回值：**如果创建成功则返回一个队列句柄**（就是队列结构体的地址），用于访问创建的队列如果创建不成功则返回NULL，可能原因是创建队列需要的RAM无法分配成功。

| 内容          | 任务 Task          | 队列 Queue      |
| :-----------:| :----------------: | :-------------: |
| 本质          | 一段独立运行的代码        | 一个数据缓冲区       |
| 是否会被 CPU 执行 | 会                | 不会            |
| 作用          | 执行功能             | 传递数据          |
| 例子          | 读取传感器、BLE发送、控制电机 | 传颜色、传传感器值、传命令 |
| 由谁使用        | 调度器调度任务运行        | 任务之间读写队列      |

```c
QueueHandle_t queue;

queue = xQueueCreate(5, sizeof(uint32_t));
```

相当于创建了一个大数组。意思是：

```text
创建一个队列
最多5个元素
每个元素4字节
```

注意这里`sizeof(uint32_t)`，说明 Queue 每格放的是一个 `uint32_t`。

## 7.3 QueueSend

```c
BaseType_t xQueueSend(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    TickType_t xTicksToWait
);
```

输入参数：
- `xQueue`：要写入的队列
- `pvItemToQueue`：要写入的消息（数据的地址）
- `xTicksToWait`：阻塞超时时间（**如果Queue满了，最多等多久**）

返回参数：

1. 返回 `pdPASS` (数值通常为 1)
- **含义**：**发送成功（Success）**。
- **发生了什么**：队列（仓库）里面还有空位子，FreeRTOS 已经成功把数据**复制**到了队列的某一个格子里。
- **下一步**：发送任务可以高高兴兴地继续往下执行别的代码了。

2. 返回 `errQUEUE_FULL` 或 `pdFALSE` (数值通常为 0)

- **含义**：**发送失败 / 队列满了（Queue Full / Timeout）**。
- **发生了什么**：队列已经**塞满了**。即使设置了超时等待时间（`xTicksToWait`），等到时间耗尽了，接收任务依然没有来把旧数据取走（仓库没腾出空位）。因此，新数据**被无情地拒之门外，没有存进去**。
- **下一步**：必须在代码里做应对处理。如果不做处理，这笔新数据就会**直接丢失（丢包）**。

---

 注意：`xQueueSend()` 的第三个数据 **传的是数据地址**，但 Queue 会把数据内容复制进去。比如：

```c
uint32_t data = 100;

xQueueSend(queue, &data, 0);
```

注意传的是 `&data`，不是 `data`。因为 Queue 需要知道“去哪个内存地址，把这 4 Byte 数据复制进 Queue。”内部概念上类似：

```text
data
地址：0x20000100
内容：100

xQueueSend(queue, &data, ...)
                  ↓
             读取4 Byte
                  ↓
             复制进Queue
```

所以 Queue 保存的是`100 的副本`，不是保存`&data`。前提是 Queue 创建时`xQueueCreate(..., sizeof(uint32_t))`

---

例子：

```c
uint32_t data = 100;

xQueueSend(queue, &data, 0);
```

发送前：

```text
Queue

[ 空 ]
[ 空 ]
[ 空 ]
```

发送后：

```text
Queue

[ 100 ]
[ 空 ]
[ 空 ]
```

然后再 `data = 200;` Queue 里面原来的 100 不会跟着变。因为Queue 已经把数据复制了一份。这是非常重要的。

#### 7.3.1.1 使用通用指针

`xQueueSend()` 的设计目标是**它要能发送任意类型的数据，而不是只能发送某一种固定类型。** 所以它不能把第二个参数写成`int data` ，因为这样就只能发送 `int`。也不能写成`SensorData_t data`，因为这样就只能发送这个结构体。FreeRTOS 要做到：

```text
int
float
char
struct
指针
自定义类型
```

全都能发。所以它用了：

```c
const void *pvItemToQueue
```

也就是$\boxed{\text{通用指针}}$，这个 `void *` 的意思就是“不管这个数据到底是什么类型，把地址给我就行。”然后 FreeRTOS 再根据创建 Queue 时给的`xQueueCreate(length, item_size);` 里的`item_size` **决定复制多少字节**。

比如创建：

```c
QueueHandle_t q = xQueueCreate(5, sizeof(int));
```

FreeRTOS 已经知道`每个元素 = 4 Byte`，之后：

```c
int data = 100;
xQueueSend(q, &data, 0);
```

它根本不需要知道这是 int。它只需要知道：

```text
地址 = &data
长度 = 4 Byte
```

然后`memcpy(queue_buffer, &data, 4);`就行了。

---

如果换成结构体：

```c
typedef struct
{
    int temperature;
    int humidity;
} Sensor_t;
```

创建`QueueHandle_t q = xQueueCreate(5, sizeof(Sensor_t));`，然后：

```c
Sensor_t data;

xQueueSend(q, &data, 0);
```

FreeRTOS 还是同一个 `xQueueSend()`。它只会：

```text
从 &data 开始
↓
复制 sizeof(Sensor_t) Byte
```

它甚至不需要知道：

```text
temperature 是什么
humidity 是什么
```

这就是 `void *` 非常强大的地方。

---

可以反过来想。如果 FreeRTOS 把函数写成：

```c
BaseType_t xQueueSend(
    QueueHandle_t queue,
    int data,
    TickType_t wait
);
```

那这个函数只能发`int`，那如果要发`float`，可能还得再写`xQueueSendFloat();`，要发`struct`又得写`xQueueSendStruct();`，那 API 就会变成：

```c
xQueueSendInt()
xQueueSendChar()
xQueueSendFloat()
xQueueSendStruct()
xQueueSendPointer()
...
```

非常丑，而且用户自定义结构体根本列不完。所以 C 里面非常常见一种设计 $\boxed{\text{void * + 数据长度}}$ 来实现“通用数据处理”。

还有一个更深的原因**如果参数是普通变量，C 函数必须提前知道这个变量有多大。**

比如：

```c
void func(int data);
```

编译器知道 `data = 4 Byte`，但如果是：

```c
void func(??? data);
```

想让 `???` 同时支持：

```text
int         4 Byte
double      8 Byte
struct      12 Byte
其他struct  100 Byte
```

C 语言没有“万能值类型”。但是地址的大小基本固定。在 STM32F4 这种 32 位 MCU中指针 = 4 Byte。无论它指向：

```text
int
char
float
100 Byte struct
```

地址本身都还是`4 Byte`，所以传地址非常方便：

```text
调用者
↓
给我一个地址
↓
Queue 根据 item_size
↓
从那个地址复制对应字节
```

这也是为什么函数原型里`const void *pvItemToQueue`会这么设计。


C 语言里**很多“通用接口”都靠指针来实现**。例如：

```c
void *buffer
void *context
void (*callback)(void)
```

因为$\boxed{\text{地址是一种非常通用的“接口”}}$

$\boxed{ \texttt{void *} + \texttt{item\_size} = \text{支持任意数据类型} }$

## 7.4 QueueReceive

```c
BaseType_t xQueueReceive(
    QueueHandle_t xQueue,
    void *pvBuffer,
    TickType_t xTicksToWait
);
```

输入参数：
- `xQueue`：要写入的队列
- `pvBuffer`：要写入的消息（数据的地址）
- `xTicksToWait`：阻塞超时时间（**如果Queue为空，最多等多久**）

它的返回值**只有两种可能**：**`pdPASS`**（代表成功）和 **`pdFALSE`**（代表失败）

1. 返回 `pdPASS` (数值通常为 1)

- **含义**：**成功（Success）**。
- **发生了什么**：队列里面有数据，并且 FreeRTOS 已经成功把数据复制到了指定的变量（收货地址）里。此时，队列里的这笔数据已经被删除了。
- **下一步**：可以放心地去使用、打印或处理这个变量。

2. 返回 `pdFALSE` (数值通常为 0)

- **含义**：**失败 / 超时（Timeout / Empty）**。
- **发生了什么**：队列是**空的**，任务在规定的超时时间（`xTicksToWait`）内**苦苦等了很久，但依然没有等到任何人发数据**。时间一到，函数被迫返回。
- **下一步**：此时传入的接收变量里**什么都没有（或者全是垃圾残渣数据）**，绝对不能使用它。需要进行超时错处理（比如报错、报警或直接跳过）。

---

FreeRTOS 会把 Queue 里的数据复制出来放到 `received`，所以过程是：

```text
Queue里面：
[10]

xQueueReceive()

↓复制

received = 10
```

然后 Queue 里的这个元素被移除。

例如：

```c
uint32_t recv_data;

xQueueReceive(queue, &recv_data, 0);
```

假设 Queue：

```text
[ 100 ]
[ 200 ]
[ 300 ]
```

执行后`recv_data = 100`，Queue 变成：

```text
[ 200 ]
[ 300 ]
```

所以 Queue **通常是FIFO，先进先出**。

## 7.5 `xTicksToWait`

```c
BaseType_t xQueueSend(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    TickType_t xTicksToWait
);
```

`xTicksToWait` 这个参数决定**Queue 满/空的时候，要不要 Block**。根据 `xTicksToWait` 填写的数值不同，任务的等待策略完全不同：

|               填写的数值                |     代表的意思     |                                行为表现                                 |
| :--------------------------------: | :-----------: | :-----------------------------------------------------------------: |
|              **`0`**               | **完全不提速/不等待** |            过来瞅一眼，有数据就拿走；没数据就**拍拍屁股走人（立刻返回）**，绝不耽误一丁点时间。             |
| **`固定的 Tick 值`**  <br>(例如 10, 100) |  **有限度的等待**   |          没数据我就去睡觉，但**只要有人发数据，我随时醒来**。如果等满了这个时间还没人发，我就不等了。           |
|        **`portMAX_DELAY`**         | **死等（无限等待）**  | 没数据我就**一直睡，直到有数据为止**。只要没人发数据，我就永远不醒来（除非设置了 `INCLUDE_vTaskSuspend`）。 |

所以，`xTicksToWait` **不是一个死板的延时函数**（不像 `vTaskDelay`），它是一个 **“只要有货，随时接单；如果没货，最多等多久”** 的保障机制。

### 7.5.1 Send

1. 0

```c
xQueueSend(queue, &data, 0);
```

   如果 **Queue 满马上返回失败，不等**。

2. 固定值

```c
xQueueSend(queue, &data, pdMS_TO_TICKS(100));，
```

如果Queue 满时：

```text
当前Task Running
↓
等待Queue有空间
↓
Blocked
↓
最多等100ms
```

如果 100ms 之内有别的 Task `Receive` 走一个数据：

```text
Queue出现空位
↓
发送Task可能被唤醒
↓
重新尝试发送
```

- **情况 A：队列里本来就有数据**
    - 任务根本**不需要等待**。
    - 它会**立刻**拿走数据，往下执行代码。整个过程耗时接近 0。
- **情况 B：队列是空的（开始等待）**
    - 任务发现没数据，为了不占用 CPU，它会**立刻进入阻塞状态（Block State）**，也就是把 CPU 让给其他任务。
    - **重点来了**：它并不会死死睡足 100 个 Tick。
- **情况 C：在第 10 个 Tick 时，有人往队列里发了数据**
    - 只要一有数据写入，操作系统会**立刻、甚至在微秒级内**把这个正在等待的任务**唤醒**。
    - 任务拿到数据，马上继续往下执行。此时，它实际只等待了 10 个 Tick。
- **情况 D：等了 100 个 Tick，还是没有数据**
    - 时间到了，任务“闹钟”响了。
    - 任务会被迫唤醒，但因为没拿到数据，函数会返回 `pdFALSE`。任务接着往下执行（通常需要您写代码判断返回值，做超时处理）。

3. portMAX_DELAY

```c
xQueueSend(queue, &data, portMAX_DELAY);
```

通常表示**一直等到能发送成功**。具体是否真正无限等待还和 FreeRTOS 配置有关。

### 7.5.2 Receive

1. 0

```c
xQueueReceive(queue, &recv, 0);
```

如果 Queue 为空马上失败返回

2. 固定值

```c
xQueueReceive(
    queue,
    &recv,
    pdMS_TO_TICKS(1000)
);
```

Queue 为空：

```text
当前Task
↓
Blocked
↓
等待数据
↓
最多1秒
```

如果期间有 Task Send：

```text
Queue有数据
↓
Receive Task被唤醒
↓
Blocked → Ready
```

Blocked = 等事件，Queue 就是一种事件等待来源。

## 7.6 Task-to-Task 例子

发送 Task：

```c
void sender_task(void *arg)
{
    uint32_t count = 0;

    while (1)
    {
        count++;

        xQueueSend(
            queue,
            &count,
            portMAX_DELAY
        );

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

接收 Task：

```c
void receiver_task(void *arg)
{
    uint32_t value;

    while (1)
    {
        if (xQueueReceive(
                queue,
                &value,
                portMAX_DELAY
            ) == pdPASS)
        {
            printf("%u\n", value);
        }
    }
}
```

逻辑：

```text
Sender Task
每1秒产生一个数字
↓
Send进Queue
↓
Receiver Task 原本 Blocked
↓
Queue有数据
↓
Receiver醒来
↓
Receive
↓
打印
↓
Queue为空
↓
Receiver再次Blocked
```

这就是 RTOS 的典型写法。

不是：

```c
while (1)
{
    if (queue_has_data())
    {
    }
}
```

疯狂轮询。而是没数据就睡、有数据再醒


## 7.7 ISR

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

# 8 Semaphore

## 8.1 概念

信号量 `Semaphore` 本质上是 **RTOS 里用来做同步和资源控制的机制**。信号量不是用来传具体数据的，而是用来告诉任务：“某件事发生了” 或 “某个资源现在可以用了”

我们希望**任务都是互斥的，同一个时间段一个任务只能被一个人调用** 在多任务里，每个任务一定是顺序执行的，他们各自独立，以不可预知的速度向前推进，但有时候希望多个任务能密切合作以实现一个共同的任务。

最核心的一句话**Queue 主要传数据，Semaphore 主要传“发生了某件事”或“某个资源现在可以用了”。** 可以先把它和 Queue 对比：

```text
Queue
= 我给你一份数据

Semaphore
= 我告诉你：可以行动了
```

比如中断里检测到按键按下：

```text
按键中断
↓
Give Semaphore
↓
Task被唤醒
↓
Task处理按键逻辑
```

这里根本不一定需要传具体数据，**只需要传递“事件发生了”这个事实**。

Semaphore 常见分两类：

```
Binary Semaphore
Counting Semaphore
```

`Task` = 人
`Queue` = 快递柜，用来放数据
`Semaphore` = 门铃，用来通知有人可以行动
`Mutex` （互斥）= 厕所门锁，同一时间只能一个人用
`Handle` = 钥匙 / 编号 / 地址
`Scheduler` = 管理员，决定谁先执行

## 8.2 Binary Semaphore

### 8.2.1 概念

Binary Semaphore 只有两种状态，可以理解为：

```
0
= 没有信号

1
= 有信号
```

或者也可以理解成**空、有一个token**。创建：

```c
SemaphoreHandle_t sem;

sem = xSemaphoreCreateBinary();
```

需要 `#include "semphr.h"`，有两个最关键的操作：

```c
xSemaphoreGive(sem);
xSemaphoreTake(sem, timeout);
```

含义是：

```text
Give
= 放一个信号进去

Take
= 取走这个信号
```

如果 Take 的时候没有信号，并且允许等待：

```c
xSemaphoreTake(sem, portMAX_DELAY);
```

那么当前 Task 就会：

```
Running
↓
发现Semaphore不可用
↓
Blocked
```

等别人 `xSemaphoreGive(sem);`，之后：

```text
Blocked
↓
Ready
```

这和 Queue 空的时候 `xQueueReceive(...)`，非常像。

可以直接类比：

```text
Queue为空
→ Receive任务Blocked

Semaphore没有token
→ Take任务Blocked
```

区别只是**Queue 里面有数据内容，而 Semaphore 本身通常不关心具体数据**。

### 8.2.2 SemaphoreCreateBinary

```c
SemaphoreHandle_t sem;

sem = xSemaphoreCreateBinary();
```

注意这里的 `sem` 也是 Handle：

```
SemaphoreHandle_t → 用来找到这个 Semaphore 对象
```

### 8.2.3 SemaphoreGive

```c
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore);
```

参数解析

- **`xSemaphore`**：想要释放的**信号量句柄**。这个句柄必须是之前通过 `xSemaphoreCreateBinary()`、`xSemaphoreCreateCounting()` 或 `xSemaphoreCreateMutex()` 成功创建出来的。

📤 返回值（`BaseType_t`）

- **`pdPASS`** (通常为 1)：**释放成功**。
- **`pdFALSE`** (通常为 0)：**释放失败**。这通常发生在信号量已经处于“满状态”，无法再增加时（例如二值信号量已经有效，却连续调用了两次 `Give`）。

例如：

```c
xSemaphoreGive(sem);
```

相当于`Semaphore: 0 → 1`，然后`xSemaphoreTake(sem, portMAX_DELAY);`，如果成功：

```text
Semaphore:
1 → 0
```

---

`xSemaphoreTake()` 最像 `xQueueReceive()` 原型概念上：

```c
BaseType_t xSemaphoreTake(
    SemaphoreHandle_t xSemaphore,
    TickType_t xTicksToWait
);
```

第一个参数：

```text
xSemaphore
→ 取哪个Semaphore
```

第二个：

```text
xTicksToWait
→ 如果当前拿不到信号，最多等多久
```

比如`xSemaphoreTake(sem, 0);` ，如果当前 Semaphore 没有信号就立即失败返回，不会 Block。而`xSemaphoreTake(sem, pdMS_TO_TICKS(1000));`，如果没有信号：

```text
当前Task Running
↓
等待Semaphore
↓
Blocked
↓
最多等1秒
```

如果 1 秒之内有人`xSemaphoreGive(sem);` 那么等待这个 Semaphore 的任务：

```
Blocked → Ready
```

之后根据优先级决定是否立刻 Running。

### 8.2.4 SemaphoreTake

**`xSemaphoreTake`** 是一个用于**获取（或者是占有、等待）信号量**的宏定义。

```c
BaseType_t xSemaphoreTake( 
    SemaphoreHandle_t xSemaphore,   /* 信号量句柄：你要拿哪把锁/等哪个暗号 */
    TickType_t xTicksToWait         /* 等待超时时间：如果没空位，最多等多久 */
);
```

📥 参数解析

- **`xSemaphore`**：想要获取的**信号量句柄**（必须是提前创建好的）。
- **`xTicksToWait`**：这个参数的逻辑与队列的 `xTicksToWait` **完全一模一样**：
    - **`0`**：过来瞅一眼，有令牌就拿走；没令牌**立刻返回 `pdFALSE`**，绝不死等。
    - **`固定的 Tick 值`**（如 10, 100）：没令牌我就去睡觉，只要期间有人 `Give` 了信号量，**立刻醒来**；如果等满了时间还没人给，被迫醒来并返回 `pdFALSE`。
    - **`portMAX_DELAY`**：**死等**。只要没人 `Give`，就永远在这里睡下去，直到地老天荒。

📤 返回值（`BaseType_t`）

- **`pdPASS`** (通常为 1)：**获取成功**。信号量的计数值成功减 1。您可以安全地进入临界区或处理同步业务。
- **`pdFALSE`** (通常为 0)：**获取失败 / 超时**。在规定时间内没有等到信号量。



### 8.2.5 典型例子

比如有一个按键中断，按键按下以后让 LED Task 工作。

LED Task：

```c
void led_task(void *arg)
{
    while (1)
    {
        xSemaphoreTake(
            button_sem,
            portMAX_DELAY
        );

        LED_Toggle();
    }
}
```

一开始`Semaphore = 0`，LED Task 执行到`xSemaphoreTake(...)`，拿不到：

```text
LED Task
Running → Blocked
```

这时候按键事件发生，Give：

```text
Semaphore:
0 → 1
```

于是：

```
LED Task
Blocked → Ready
```

如果它优先级够高`Ready → Running`，然后 Take 成功：

```text
Semaphore:
1 → 0
```

最后`LED_Toggle();`。所以整个模型：

```text
事件发生
↓
Give
↓
Semaphore available
↓
等待中的Task被唤醒
↓
Take成功
↓
执行处理
```

## 8.3 Counting Semaphore
### 概念

Counting Semaphore 本质可以先理解成**一个有上限的计数器 + 可以让 Task 阻塞等待。** 比如：

```text
最大值 = 5
当前值 = 0
```

每`xSemaphoreGive()`一次，`count + 1`。每`xSemaphoreTake()`一次，`count - 1`。但：

```text
count不能超过最大值
count也不能小于0
```

---

### SemaphoreCreateCounting

创建函数是：

```c
SemaphoreHandle_t xSemaphoreCreateCounting(
    UBaseType_t uxMaxCount,
    UBaseType_t uxInitialCount
);
```

两个参数非常重要。

假设：

```c
SemaphoreHandle_t count_sem;

count_sem = xSemaphoreCreateCounting(5,0);
```

意思是：

```text
最大计数值 = 5
初始计数值 = 0
```

也就是创建之后count_sem，当前count = 0，最大count = 5

---

第一个参数`uxMaxCount`，表示 **这个 Counting Semaphore 最大能累计几个“许可/事件”。**

例如：

```
xSemaphoreCreateCounting(5, 0);
```

最多：

```
0 1 2 3 4 5
```

不能变成：

```
6
```

---

第二个参数：

```
uxInitialCount
```

表示：

> **创建出来的时候，当前计数是多少。**

比如：

```
xSemaphoreCreateCounting(5, 3);
```

那么一创建：

```
当前count = 3
最大count = 5
```

也就是说一开始就已经有：

```
3个许可
```

可以被 Take。

---

最典型调用就是：

```
SemaphoreHandle_t count_sem;

count_sem = xSemaphoreCreateCounting(5, 0);

if (count_sem == NULL)
{
    // 创建失败
}
```

之后：

```
xSemaphoreGive(count_sem);
```

计数：

```
0 → 1
```

再 Give：

```
1 → 2
```

再 Give：

```
2 → 3
```

---

然后：

```
xSemaphoreTake(count_sem, 0);
```

成功：

```
3 → 2
```

再 Take：

```
2 → 1
```

所以完整变化：

```
初始：

count = 0

Give
↓
count = 1

Give
↓
count = 2

Give
↓
count = 3

Take
↓
count = 2

Take
↓
count = 1
```

---

如果当前：

```
count = 0
```

执行：

```
xSemaphoreTake(count_sem, 0);
```

会：

```
立刻失败
```

因为没有许可可以拿。

如果：

```
xSemaphoreTake(
    count_sem,
    portMAX_DELAY
);
```

那么当前 Task：

```
发现 count = 0
↓
拿不到
↓
进入 Blocked
↓
等待别人 Give
```

当另一个 Task：

```
xSemaphoreGive(count_sem);
```

发生：

```
等待中的Task
Blocked → Ready
```

之后这个 Task 被调度运行时，就可以继续 Take。

---

你可以看一个完整例子。

生产者 Task：

```
void producer_task(void *arg)
{
    while (1)
    {
        // 假设发生了一个事件
        xSemaphoreGive(count_sem);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

消费者 Task：

```
void consumer_task(void *arg)
{
    while (1)
    {
        if (xSemaphoreTake(
                count_sem,
                portMAX_DELAY
            ) == pdTRUE)
        {
            process_event();
        }
    }
}
```

逻辑：

```
Producer
↓
Give
↓
count + 1

Consumer
↓
Take
↓
count - 1
↓
处理一次事件
```

---

比如 Producer 很快：

```
Give
Give
Give
```

而 Consumer 还没来得及处理。

Counting Semaphore 可以记住：

```
count = 3
```

Consumer 后面：

```
Take
→ 3 → 2

Take
→ 2 → 1

Take
→ 1 → 0
```

所以它和 Binary Semaphore 最大区别就在这里。

Binary：

```
只有0/1
```

例如连续：

```
Give
Give
Give
```

很可能最后还是：

```
1
```

中间多个事件可能被“合并”。

而 Counting：

```
Give
Give
Give
```

可以：

```
0 → 1 → 2 → 3
```

能记住事件发生了几次。

Counting Semaphore 和 Binary 的区别主要是数量。Binary，最大就是1。Counting，可以有0、1、2、3...N

比如停车位：

```c
Counting Semaphore = 3
```

表示同时允许 3 个资源被使用。

Take 一个 `3 → 2`，再 Take `2 → 1`。Give `1 → 2`， 所以 Counting Semaphore 更适合：

```text
有限数量资源管理
事件累计计数
```

例如中断来了 5 次，而 Task 还没来得及处理，可以用 counting semaphore 记录“来了5次”。

Binary Semaphore 就只能记：

```
有 / 没有
```

### SemaphoreCreate



## Mutex


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
