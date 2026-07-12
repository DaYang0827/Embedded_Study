# 1 外设
## 1.1 LCD

TFT-LCD 即薄膜晶体管液晶显示器。其英文全称为:Thin Film Transistor-Liquid Crystal Display。TFT-LCD与无源TN-LCD、STN-LCD的简单矩阵不同,它在液晶显示屏的每一个象素上都设置有一个薄膜晶体管(TFT),可有效地克服非选通时的串扰,使显示液晶屏的静态特性与扫描线数无关。

TFT-LCD具有:亮度好、对比度高、层次感强、颜色鲜艳等特点。应用于电视、手机、电脑、平板等各种电子产品。

分辨率:240+320驱动IC: ILI9341

自带触摸屏（电阻触摸屏)16位80并口驱动

16位真彩显示(65536色)

### 1.1.1 读写数据

| 引脚名称    | 代码变量名     | 功能描述      |
| :-------: | :---------: | :---------: |
| NCS     | CS        | 片选信号      |
| DNC     | RS        | 数据/命令选择信号 |
| NRD     | RD        | 读信号       |
| NWR     | WR        | 写信号       |
| DB7-DB0 | DATA_PORT | 数据总线      |

1. CS（Chip Select）作用：片选用于 SPI 或并口通信。

当：CS = 0 → TFT被选中

CS = 1  → TFT不响应

如果总线上有多个设备，就需要 CS

2. DC / RS（Data Command）不同模块叫法不同：

DC （Data/Command）

RS （Register Select）

作用：DC = 0 → 发送命令   DC = 1 → 发送数据

例如：写寄存器地址 → DC=0   写像素数据 → DC=1

这是非常重要的控制线

## 2 HC-05 模块

### 2.1 简介

- HC-05和HC-06是广泛使用的**经典蓝牙模块**，常用于与单片机（如Arduino）进行无线通信。
- 它们基于蓝牙2.0标准，支持串口通信（UART）协议，使得与单片机的连接和数据交换相对简单。
- HC-05可作为主设备或从设备，支持蓝牙SPP（串口通信）和AT命令模式。
- HC-06通常作为从设备，仅支持蓝牙SPP模式。
- 它们具有基本的通信范围，通常在10米左右。

### 2.2 HC-05参数

|名称|说明|
|:---:|:---:|
|接口说明|TTL，兼容3.3V/5V单片机系统|
|波特率|4800.9600（默认）、19200、38400、57600、115200、230400、460800、921600、1382400|
|工作温度|-25°C~75°C|
|工作电压|DC3.3V-5.0V|
|工作电流|配对中：30~40mA，配对完毕未通信：1-8mA；通信中：5-20mA|

### 2.3 HC-05工作模式

具有两种工作模式：命令响应工作模式和自动连接工作模式，在自动连接工作模式下模块又可分为主（Master）、从（Slave）和回环（Loopback）三种工作角色。

当模块处于**自动连接工作模式**时，将自动根据事先设定的方式连接的数据传输；

当模块处于**命令响应工作模式**时能执行下述所有AT命令，用户可向模块发送各种AT指令，为模块设定控制参数或发布控制命令。通过控制模块外部引脚（PIO11）输入电平，可以实现模块工作状态的动态转换。

1. Master（主角色）—查询周围SPP蓝牙从设备，并主动发起连接，从而建立主、从蓝牙设备间的透明数据传输通道。
2. Slave（从角色）—被动连接；
3. Slave-Loop（回环角色）—被动连接，接收远程蓝牙主设备数据并将数据原样返回给远程蓝牙主设备；

引脚定义
1. PIO8 连接LED，指示模块工作状态，模块上电后闪烁，不同的状态闪烁间隔不同。当灯快闪的时候，就是自动连接工作模式；当灯慢闪的时候，就是命令响应工作模式。
2. PIO9连接LED，指示模块连接成功，蓝牙串口匹配连接成功后，LED 长亮
3. PIO11 模块状态切换脚，高电平-->AT 命令响应工作状态，低电平或悬空-->蓝牙常规工作状态。
4. 模块上已带有复位电路，重新上电即完成复位。

# 3 中间件层
## 3.1 LVGL
LVGL （Light and Versatile Graphics Library，轻量级通用型图形库）是目前全球在嵌入式和微控制器（MCU/MPU）领域中最流行、最活跃的免费开源 GUI（图形用户界面）库

1. 核心特性
- **极其丰富的内置组件（Widgets）**：内置了 30 多个标准组件，包括**按钮（Button）、滑块（Slider）、图表（Chart）、日历（Calendar）、下拉列表、键盘**等，全部支持动态创建和销毁。]
- **现代化的视觉特效**：原生支持高级图形效果，如**抗锯齿（边缘平滑）、高级动画、透明度叠加、平滑滚动、圆角阴影和渐变色**。 
- **Web 级的布局机制**：引入了类似网页前端的 **Flexbox（弹性盒子）和 Grid（网格）布局管理器**，当屏幕分辨率发生改变时，界面能够自动做响应式适配。]
- **强大的样式系统**：采用类似于网页 **CSS 样式表** 的设计思想。你可以给一组按钮统一定义一种“皮肤样式”，当进入按下（Pressed）或禁用（Disabled）状态时，样式会自动切换。]
- **极佳的商用协议**：采用 **MIT 开源协议**，意味着你完全可以用它来开发商业闭源产品，不需要支付任何版权费。

2. 它对单片机的硬件要求高吗？（低资源消耗）

LVGL 是纯 C 语言编写的（完美兼容 C++），它没有任何外部库依赖，可以运行在**纯裸机**或者任何 **RTOS（如 FreeRTOS、RT-Thread）** 上。

想要跑一套精美的 UI，它的**硬件物理门槛最低只需要**： 

- **Flash（闪存）**：约 `200KB - 300KB`（用于存放核心库代码与基础字体）。
- **RAM（运行内存）**：约 `100KB`。
- **渲染缓冲区**：极度节省内存！它甚至**不需要一整块屏幕大小的帧缓冲区**。你只需要开辟一个相当于 **1/10 屏幕大小**的行缓冲区，利用 DMA 异步刷新，就能跑到 60 帧的丝滑效果。

3. LVGL 编程的“三大支柱”思想

- **对象系统（Object System）**：一切皆对象。在代码中，基础的基础是一个叫 `lv_obj_t` 的结构体指针。按钮是对象，标签是对象，屏幕本身也是一个大对象。它们通过**父子层级关系**嵌套组合在一起。
- **样式系统（Style System）**：通过 `lv_style_t` 结构体来描述对象的颜色、边宽、圆角等外观。通过将样式“绑定”到对象上，实现业务逻辑与美工视觉的分离。
- **事件系统（Event System）**：采用类似高级语言的“信号与槽”或回调函数机制。通过 `lv_obj_add_event_cb` 给按钮绑定一个你写好的函数指针。当用户物理触摸点击屏幕、或者转动旋钮时，单片机会自动触发该回调函数去执行你的应用代码。

## 3.2  Free RTOS

RTOS （Real Time Operating System，中文就是实时操作系统）

FreeRTOS是一个迷你的实时操作系统内核。作为一个轻量级的操作系统，功能包括：任务管理、时间管理、信号量、消息队列、内存管理、记录功能、软件定时器、协程等，可基本满足较小系统的需要。

由于RTOS需占用一定的系统资源（尤其是RAM资源），只有uC/OS-I、口embOS、salvo、FreeRTOS等少数实时操作系统能在小RAM单片机上运行。相对uC/OS-II、embOS等商业操作系统，FreeRTOS操作系统是完全免费的操條3585462系统，具有源码公开、可移植、可裁减、调度策略灵活的特点，可以方便地移植到各种单片机上运行，主流的稳定版本为10.4.4版。

FreeRTOS的设计小巧且简易，整个核心代码只有3到4个C文件，为了让代码容易阅读、移植和维护，大部分的代码都是以C语言编写，只有一些函数（多数是架构特定排班副程序）采用汇编语言编写。

### 3.2.1  FreeRTOS移植

1、添加RTOS源码到Keil工程

2、添加head_4.c到Keil工程

3、添加port.c到Keil工程

4、添加头文件路径

5、添加FreeRTOSConfig.h

6、修改FreeRTOSConfig.h配置文件，直到工程编译无错误

### 3.2.2 创建任务Task/Thread

```c
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,

                        Const char * const pcName,

                        Const configSTACK_DEPTH_TYPE usStackDepth,

                        Void * const pvParameters,

                        UBaseType_t uxPriority,

                        TaskHandle_t * const pxCreatedTask)
```

pxTaskCode:指向任务函数的指针，注意，任务函数不能返回（即死循环）

pcName：任务名，字符串

usStackDepth：栈深，即任务的栈大小（单位是字，1字 = 4字节）

pvParameters：任务的参数指针（即FreeRTOS 允许你给任务函数传一个“通用指针”）

uxPriority：任务的优先级，最低优先级是0，数字越大，优先级越高

pxCreatedTask：任务的句柄，用于控制任务

在调用的时候使用
```c
xTaskCreate(led_blink, "led_blink", 256, (void*)&LED0, 1, NULL);

xTaskCreate(led_blink, "led_blink", 256, (void*)&LED1, 1, NULL);

xTaskCreate(led_blink, "led_blink", 256, (void*)&LED2, 1, NULL);
```

进行调用，注意到第四个参数是(void*)&LED0

&LED0表示：取 LED0 这个结构体变量的地址。

因为 LED0 是一个结构体变量：

```c
LED_TypeDef LED0 =
{
	GPIOB, GPIO_Pin_0, RCC_AHB1Periph_GPIOB
};
```

那么：&LED0类型就是：LED_TypeDef* 也就是“指向 LED_TypeDef 的指针”。(void*)&LED0表示：把 LED_TypeDef* 转换成 void*，传给 FreeRTOS。因为 xTaskCreate 第四个参数规定就是 void*。void* 可以理解为：通用地址类型，什么类型的地址都可以先放进来。然后到了任务函数里面，你再转换回来：
```c
LED_TypeDef *led = (LED_TypeDef *)args;
```

| 函数                       | 含义      | 返回值     |
| :------------------------: | :-------: | :-------: |
| xTaskCreate()            | 创建任务    | 有，成功或失败 |
| vTaskDelay()             | 当前任务延时  | 无       |
| vTaskStartScheduler()    | 启动调度器   | 无       |
| xTaskGetSchedulerState() | 获取调度器状态 | 有       |
| vTaskDelete()            | 删除任务    | 无       |

xTask 和 vTask 不是不同任务，而是 FreeRTOS 的函数命名习惯；v 通常表示无返回值，x 通常表示有返回值，Task 表示任务管理相关函数

### 3.2.3 队列

#### 3.2.3.1 创建队列Queue

```c
QueueHandle_t xQueueCreat(UBaseType_t uxQueueLength,

                           UBaseType_t uxItemSize);  
```

xQueueCreat函数有两个参数uxQueueLength和 uxItemSize

uxQueueLength：队列能够存储的最大消息数目，即队列长度

uxItemSize：队列中消息的大小，一字节为单位

返回值：如果创建成功则返回一个队列句柄（就是队列结构体的地址），用于访问创建的队列

如果创建不成功则返回NULL，可能原因是创建队列需要的RAM无法分配成功。

| 内容          | 任务 Task          | 队列 Queue      |
| :-----------:| :----------------: | :-------------: |
| 本质          | 一段独立运行的代码        | 一个数据缓冲区       |
| 是否会被 CPU 执行 | 会                | 不会            |
| 作用          | 执行功能             | 传递数据          |
| 例子          | 读取传感器、BLE发送、控制电机 | 传颜色、传传感器值、传命令 |
| 由谁使用        | 调度器调度任务运行        | 任务之间读写队列      |

相当于创建了一个大数组

#### 3.2.3.2  队列发送函数
```c
BaseType_t xQueueSend(QueueHandle_t xQueue,

                      const void * pvItemToQueue,

                      TickType_t xTicksToWait);
```

xQueue：要写入的队列

pvItemToQueue：要写入的消息（数据的地址）

xTicksToWait：阻塞超时时间（当队列为满，是否需要进行阻塞等待）

返回值：

1.pdTRUE：写入成功

2.errQUEUE_FULL：队列满，写入失败

#### 3.2.3.3  队列接收函数
```c
BaseType_t xQueueReceive(QueueHandle_t xQueue,

                         const void * pvBuffer,

                         TickType_t xTicksToWait);
```

xQueue：要写入的队列

pvBuffer：要写入的消息（数据的地址）

xTicksToWait：阻塞超时时间（当队列为空，是否需要进行阻塞等待）

返回值:

1.pdTRUE：写入成功

2.errQUEUE_FULL：队列为空，写入失败

#### 3.2.3.4 队列发送/接收函数中断版本

```c
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue,

                             const void * pvItemToQueue,

                             BaseType_t * pvHigherPriorityTaskWoken);  

BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue,

                                 const void * pvBuffer,

                                 BaseType_t * pvHigherPriorityTaskWoken);  
```


中断中是不能进入阻塞态

中断中不能立马切换任务

中断是快进快出，执行的代码越少越好

### 3.2.4 信号量Semaphore

信号量 Semaphore 本质上是 RTOS 里用来做同步和资源控制的机制。信号量不是用来传具体数据的，而是用来告诉任务：“某件事发生了” 或 “某个资源现在可以用了”

我们希望任务都是互斥的，同一个时间段一个任务只能被一个人调用

在多任务里，每个任务一定是顺序执行的，他们各自独立，以不可预知的速度向前推进，但有时候我们希望多个任务能密切合作以实现一个共同的任务。绒布就是在多任务的一些关键点上可能需要互相等待和互通消息

Task = 人

Queue = 快递柜，用来放数据

Semaphore = 门铃，用来通知有人可以行动

Mutex （互斥）= 厕所门锁，同一时间只能一个人用

Handle = 钥匙 / 编号 / 地址

Scheduler = 管理员，决定谁先执行

