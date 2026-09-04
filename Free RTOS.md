更适合你简历的是：
![[Pasted image 20260828163923.png]]

其中 **Zephyr复盘和Linux基础可以穿插进行**
# 1 FreeRTOS

`RTOS` （Real Time Operating System，中文就是实时操作系统）

`FreeRTOS`是一个迷你的实时操作系统内核。作为一个轻量级的操作系统，功能包括：任务管理、时间管理、信号量、消息队列、内存管理、记录功能、软件定时器、协程等，可基本满足较小系统的需要。

由于RTOS需占用一定的系统资源（尤其是RAM资源），只有uC/OS-I、口embOS、salvo、FreeRTOS等少数实时操作系统能在小RAM单片机上运行。相对uC/OS-II、embOS等商业操作系统，FreeRTOS操作系统是完全免费的操條3585462系统，具有源码公开、可移植、可裁减、调度策略灵活的特点，可以方便地移植到各种单片机上运行。

FreeRTOS的设计小巧且简易，整个核心代码只有3到4个C文件，为了让代码容易阅读、移植和维护，大部分的代码都是以C语言编写，只有一些函数（多数是架构特定排班副程序）采用汇编语言编写。

## 1.1  FreeRTOS移植

1. 添加RTOS源码到Keil工程
2. 添加head_4.c到Keil工程
3. 添加port.c到Keil工程
4. 添加头文件路径
5. 添加FreeRTOSConfig.h
6. 修改FreeRTOSConfig.h配置文件，直到工程编译无错误

## 数据类型与编程规范
### 数据类型

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

### 变量名

变量名有前缀：

| 变量名前缀 |                           含义                            |
| :---: | :-----------------------------------------------------: |
|   c   |                         `char`                          |
|   s   |                    `int16_t`，`short`                    |
|   l   |                    `int32_t`，`long`                     |
|   x   | `BaseType_t`，其他非标准的类型：结构体、`task handle`、`queue handle`等 |
|   u   |                       `unsigned`                        |
|   p   |                           指针                            |
|  uc   |                `uint8_t`，`unsigned char`                |
|  pc   |                        `char`指针                         |

### 函数名

函数名的前缀有2部分：返回值类型、在哪个文件定义

|        函数名前缀        |                    含义                     |
| :-----------------: | :---------------------------------------: |
| `vTaskPrioritySet`  |      返回值类型：`void`  <br>在`task.c`中定义       |
|   `xQueueReceive`   |  返回值类型：`BaseType_t`   <br>在`queue.c`中定义   |
| `pvTimerGetTimerID` | 返回值类型：`pointer to void`  <br>在`tmer.c`中定义 |
## 1.2 创建任务Task/Thread

```c
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                        Const char* const pcName,
                        Const configSTACK_DEPTH_TYPE usStackDepth,
                        Void* const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t* const pxCreatedTask)
```

`pxTaskCode`:指向任务函数的指针，注意，任务函数不能返回（即死循环）

`pcName`：任务名，字符串

`usStackDepth`：栈深，即任务的栈大小（单位是字，1字 = 4字节）

`pvParameters`：任务的参数指针（即FreeRTOS 允许你给任务函数传一个“通用指针”）

`uxPriority`：任务的优先级，最低优先级是0，数字越大，优先级越高

`pxCreatedTask`：任务的句柄，用于控制任务

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

那么：`&LED0`类型就是：`LED_TypeDef*` 也就是“指向 `LED_TypeDef` 的指针”。`(void*)&LED0`表示：把 `LED_TypeDef*` 转换成 `void*`，传给 FreeRTOS。因为 `xTaskCreate` 第四个参数规定就是 `void*`。`void*` 可以理解为：通用地址类型，什么类型的地址都可以先放进来。然后到了任务函数里面，你再转换回来：

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

## 1.3 队列

### 1.3.1 创建队列Queue

```c
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength,
                           UBaseType_t uxItemSize);  
```

`xQueueCreat` 函数有两个参数`uxQueueLength`和 `uxItemSize`
`uxQueueLength`：队列能够存储的最大消息数目，即队列长度
`uxItemSize`：队列中消息的大小，一字节为单位

返回值：如果创建成功则返回一个队列句柄（就是队列结构体的地址），用于访问创建的队列如果创建不成功则返回NULL，可能原因是创建队列需要的RAM无法分配成功。

| 内容          | 任务 Task          | 队列 Queue      |
| :-----------:| :----------------: | :-------------: |
| 本质          | 一段独立运行的代码        | 一个数据缓冲区       |
| 是否会被 CPU 执行 | 会                | 不会            |
| 作用          | 执行功能             | 传递数据          |
| 例子          | 读取传感器、BLE发送、控制电机 | 传颜色、传传感器值、传命令 |
| 由谁使用        | 调度器调度任务运行        | 任务之间读写队列      |

相当于创建了一个大数组

### 1.3.2  队列发送函数
```c
BaseType_t xQueueSend(QueueHandle_t xQueue,

                      const void * pvItemToQueue,

                      TickType_t xTicksToWait);
```

`xQueue`：要写入的队列
`pvItemToQueue`：要写入的消息（数据的地址）
`xTicksToWait`：阻塞超时时间（当队列为满，是否需要进行阻塞等待）

返回值：

1. `pdTRUE`：写入成功
2. `errQUEUE_FULL`：队列满，写入失败

### 1.3.3  队列接收函数
```c
BaseType_t xQueueReceive(QueueHandle_t xQueue,

                         const void * pvBuffer,

                         TickType_t xTicksToWait);
```

`xQueue`：要写入的队列

`pvBuffer`：要写入的消息（数据的地址）

`xTicksToWait`：阻塞超时时间（当队列为空，是否需要进行阻塞等待）

返回值:

1. `pdTRUE`：写入成功
2. `errQUEUE_FULL`：队列为空，写入失败

### 1.3.4 队列发送/接收函数中断版本

```c
BaseType_t xQueueSendFromISR (QueueHandle_t xQueue,
                             const void * pvItemToQueue,
                             BaseType_t * pvHigherPriorityTaskWoken);  

BaseType_t xQueueReceiveFromISR (QueueHandle_t xQueue,
                                 const void * pvBuffer,
                                 BaseType_t * pvHigherPriorityTaskWoken);  
```


中断中是不能进入阻塞态

中断中不能立马切换任务

中断是快进快出，执行的代码越少越好

## 1.4 信号量Semaphore

信号量 `Semaphore` 本质上是 **RTOS 里用来做同步和资源控制的机制**。信号量不是用来传具体数据的，而是用来告诉任务：“某件事发生了” 或 “某个资源现在可以用了”

我们希望**任务都是互斥的，同一个时间段一个任务只能被一个人调用** 在多任务里，每个任务一定是顺序执行的，他们各自独立，以不可预知的速度向前推进，但有时候希望多个任务能密切合作以实现一个共同的任务。绒布就是在多任务的一些关键点上可能需要互相等待和互通消息

`Task` = 人
`Queue` = 快递柜，用来放数据
`Semaphore` = 门铃，用来通知有人可以行动
`Mutex` （互斥）= 厕所门锁，同一时间只能一个人用
`Handle` = 钥匙 / 编号 / 地址
`Scheduler` = 管理员，决定谁先执行
