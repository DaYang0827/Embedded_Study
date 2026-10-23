# 基本信息

在科技与通信领域，**BLE** 是 **Bluetooth Low Energy** 的缩写，中文通常翻译为 **“蓝牙低功耗”**（也曾被称为 Bluetooth Smart）。它是现代无线通信中最核心的技术之一，广泛应用于智能手机、可穿戴设备和智能家居中。

1. 什么是 BLE？（与传统蓝牙的区别）

蓝牙技术在 4.0 版本时引入了一个巨大的分水岭，将其拆分为两种模式：

- **经典蓝牙 (Classic Bluetooth)**：主要用于需要**大数据量、连续传输**的场景。例如：蓝牙耳机听歌、蓝牙音箱播放音频、车载免提电话。它的缺点是功耗较高。
- **低功耗蓝牙 (BLE)**：专门为**低数据量、间歇性传输、对电量极其敏感**的设备设计。

📊 经典蓝牙 vs BLE 直观对比

|    特性    |  经典蓝牙 (Classic)   |       低功耗蓝牙 (BLE)       |
| :------: | :---------------: | :---------------------: |
|  **功耗**  | 较高（几毫秒到几秒就能耗尽小电池） | **极低**（一颗纽扣电池可运行数月甚至数年） |
| **连接速度** |   较慢（需要几秒钟配对连接）   | **极快**（几毫秒内完成启动、连接和传输）  |
| **数据速率** |   较高 (2-3 Mbps)   | 较低 (约 1-2 Mbps，实际传输更小)  |
| **主要应用** |    音频传输、大文件传输     |   智能手表、健康手环、定位标签、传感器    |

---

2. BLE 的核心工作原理与技术特点

- “睡眠”与“唤醒”机制（低功耗的秘密）

BLE 之所以省电，是因为它在绝大多数时间都处于**深度睡眠状态**。它不会像经典蓝牙那样维持一个持续的“在线连接”，而是：

> **平时休眠**：不消耗电量。
> **定时醒来**：广播（Broadcast）一段非常微小的信号。
 >**快速传输**：如果有数据要发，花几毫秒发完。
 >**立刻秒睡**：重新进入休眠。

-  工作角色：中心设备与外围设备

>**外围设备 (Peripheral)**：通常是传感器、手环或防丢器（如 AirTag）。它们电量有限，通过不断发出**广播包 (Advertising)** 来告诉世界：“我在这里，我有数据”。
>**中心设备 (Central)**：通常是手机、平板或电脑。它们电量充足，负责**扫描 (Scanning)** 周围的外围设备，并主动发起连接。

- 属性协议 (GATT)

BLE 传输数据使用一种结构化的方式，叫做 **GATT (Generic Attribute Profile)**。它把数据分门别类：

> **Service（服务）**：一类功能的集合（例如：“心率服务”）。
> **Characteristic（特征）**：服务下的具体数据指标（例如：“当前心率值：每分钟 72 次”）。手机就是通过读取这些“特征”来获取设备数据的。

---

3. 现实生活中的典型应用场景

- **可穿戴与健康设备**：智能手环、智能手表、血糖仪、心率带。它们只需要每隔几秒向手机同步一次步数或心率。
- **位置追踪与 Beacon（信标）**：Apple 的 **AirTag** 或商场内的室内导航基站。它们不断广播自己的 ID，手机路过时就能感知到具体位置。
- **智能家居**：智能门锁、温度计、照明控制。你可以用手机直接连接并开锁，或者传感器几个月才上报一次温度。
- **无线外设**：无线蓝牙鼠标、键盘、翻页笔。

---

4. 💡 拓展：BLE 的发展（Bluetooth 5.0 及以后）

随着蓝牙 5.0 及更高版本（如蓝牙 5.4）的普及，BLE 得到了极大的增强：

- **速度翻倍**：传输速率提升到 2Mbps。
- **距离极远**：引入了编码物理层（Coded PHY），理论传输距离可达数百米。
- **Mesh 组网**：允许成百上千个 BLE 设备互相转发信号，实现超大范围的智能建筑控制。

整体架构：

```text
BLE
│
├── Controller
│   ├── PHY
│   └── Link Layer
│
└── Host
    ├── HCI
    ├── L2CAP
    ├── ATT
    ├── GATT
    ├── SMP
    └── GAP
```

# GAP

GAP `Generic Access Profile` 它主要解决BLE 设备**之间怎么发现彼此、怎么建立连接、设备扮演什么角色**。最重要的几个角色：

```text
Advertiser
Scanner
Peripheral
Central
```

可以理解为：

```text
ESP32设备
↓
Advertising
↓
手机扫描到
↓
手机发起连接
↓
ESP32 = Peripheral
手机 = Central
```

注意：

> Advertiser / Scanner 是广播阶段角色。  
> Peripheral / Central 是连接阶段角色。

---

# Advertising：广播

广播就是：

```text
设备不建立连接
↓
周期性发送Advertising Packet
↓
附近设备可以扫描到
```

例如：

```text
ESP32
每100ms广播一次
↓
手机扫描
↓
看到Manufacturer Data
```

把递增序号放在 Manufacturer Data，本质就是：

```text
Advertising Payload
↓
AD Structures
↓
Manufacturer Specific Data
```

## 广播包

Advertising Data 不是随便塞。通常由多个 `AD Structure`，组成：

```text
Length
Type
Data
```

例如：

```
[02][01][06]
```

大概：

```
Length = 2
Type   = Flags
Data   = 0x06
```

再比如 `Manufacturer Specific Data`，可以放：

```text
Company ID
+
自定义数据
```

sequence number，就属于这一类。

## 广播容易丢包

广播是 `Connectionless`，设备只是发、发、发，**并不知道对方到底有没有收到**，所以没有天然：

```text
ACK
重传
可靠传输
```

所以增加递增8位序号非常有意义。比如收到：

```text
10
11
13
```

就能判断 12 丢了，所以**广播适合低频状态、Beacon、发现设备，不适合直接做可靠 OTA**。

## 广播和连接模式

广播是：

```text
设备A
↓
广播
↓
多个设备都可能收到
```

特点是**无连接、低功耗、低开销、不保证可靠、数据量小**。

连接是：

```text
Central
↔
Peripheral
```

特点是**有连接状态、可以双向通信、有ACK/重传机制、可以GATT读写、更适合OTA**。

因此 BLE OTA 一般不靠普通广播传完整固件。而是：

```text
建立连接
↓
通过GATT Characteristic传输固件
```

# GATT

GATT （Generic Attribute Profile）是通用属性配置文件，它解决**BLE 连接以后，数据以什么形式组织**。结构为：

```text
Server
│
├── Service
│   ├── Characteristic
│   ├── Characteristic
│   └── Characteristic
│
└── Service
```

可以把它理解成：

```text
设备
↓
多个模块
↓
每个模块有多个变量/接口
```

## Service 

Service **一组相关功能的集合**。例如 OTA Service，下面可以有：

```text
Control Characteristic
Data Characteristic
Status Characteristic
```

于是：

```text
OTA Service
├── Control
├── Data
└── Status
```

这就是很典型的 OTA GATT 设计。

## Characteristic

Characteristic 可以理解成一个**可以被读取、写入或通知的数据项**。例如：

```text
Battery Service
└── Battery Level Characteristic
```

手机可以 Read 得到 `80%` 或者设备主动 Notify 告诉手机 Battery = 79%

### Characteristic Property

Characteristic 可以有不同属性：

```text
Read
Write
Write Without Response
Notify
Indicate
```

### Read

```text
Central
↓
Read Request
↓
Peripheral返回数据
```

适合**设备版本、设备状态、固件版本、电池电量**。比如 OTA `Read Firmware Version`

### Write

```text
Central
↓
Write Request
↓
Peripheral
↓
Response
```

特点是**有响应、更可靠、但吞吐较低**，适合：

```text
控制命令
BEGIN_UPDATE
FINISH_UPDATE
```

### Write Without Response

```text
Central
↓
直接写
↓
不等待ATT响应
```

特点为**更快、吞吐更高**，非常适合 `OTA Firmware Data Chunk`，比如：

```text
Chunk1
Chunk2
Chunk3
...
```

但是需要考虑流控、序号、重传、接收缓存。

### Notify

Notify：

```text
Peripheral
↓
主动通知Central
```

不需要每次 Central 来 Read。例如：

```text
OTA progress
ACK
Error
Result
```

设备可以：

```text
Notify:
offset = 4096
status = OK
```

手机就知道目前收到4096 Byte

### Indicate

Indicate 和 Notify 很像，但 `Indicate → 对方需要确认`，所以**可靠性更高，但速度更慢**。

简单理解：

```
Notify
→ 快，不等确认

Indicate
→ 慢，但有确认
```

# ATT 

ATT （Attribute Protocol），GATT 实际是建立在 ATT 上面的。可以理解：

```text
GATT
→ 规定数据怎么组织

ATT
→ 规定怎么访问这些数据
```

比如：

```text
Read
Write
Notify
Indicate
```

本质都是 ATT 操作。

# UUID 

UUID是通用唯一标识符（Universally Unique Identifier），BLE Service 和 Characteristic 都有 UUID。例如：

```text
Service UUID
Characteristic UUID
```

标准服务 `16-bit UUID`，自定义服务通常 `128-bit UUID`

OTA一般就是自定义：

```text
OTA Service UUID

Control Characteristic UUID
Data Characteristic UUID
Status Characteristic UUID
```

所以手机知道：

```text
哪个Characteristic负责控制
哪个负责数据
哪个负责状态
```

---

# 十九、Client / Server

这个也很容易和 Central / Peripheral 混。

它们不是完全同一个概念。

```
Central / Peripheral
→ GAP角色

Client / Server
→ GATT角色
```

典型：

```
手机
Central
GATT Client

设备
Peripheral
GATT Server
```

但概念上：

```
Central ≠ Client
Peripheral ≠ Server
```

只是实际项目里经常这么搭配。

---

# 二十、Connection Interval

BLE 建立连接后，不是一直持续传输。

而是按照：

```
Connection Event
```

周期通信。

比如：

```
Connection Interval = 30 ms
```

大概：

```
每30ms有一次通信机会
```

这个会影响：

```
吞吐量
延迟
功耗
```

OTA通常希望：

```
更短Connection Interval
```

提高吞吐。

但：

```
功耗会上升
```

---

# 二十一、Slave Latency / Peripheral Latency

Peripheral 不一定每个 Connection Event 都参与。

可以跳过一些。

这叫：

```
Peripheral Latency
```

适合低功耗设备。

但 OTA 时一般：

```
不希望跳太多
```

因为会降低传输速度。

---

# 二十二、Supervision Timeout

表示：

> 多长时间没有成功通信，就认为连接断了。

例如：

```
Connection lost
↓
Supervision Timeout
↓
Disconnect
```

OTA 里这很重要。

如果 OTA 正传到 60%，连接断了：

```
你需要决定：
重新开始
还是断点续传
```

---

# 二十三、MTU 是 OTA 重点

MTU：

```
Maximum Transmission Unit
```

ATT 默认 MTU 常见：

```
23 Byte
```

但不是说你能传 23 Byte payload。

因为 ATT 自己需要 header。

典型 ATT Write：

```
ATT_MTU = 23
```

真正 Value 最大大约：

```
23 - 3 = 20 Byte
```

所以老 BLE 经常看到：

```
20 Byte一包
```

---

# 二十四、为什么现在很多 BLE 可以超过 20 Byte

因为可以：

```
MTU Exchange
```

例如：

```
MTU = 247
```

那么每个 ATT payload 可以大很多。

例如：

```
247 - 3 = 244 Byte
```

所以 OTA 吞吐大幅提升。

---

# 二十五、MTU 和你的 Chunk Size

你以后 OTA 不能随便：

```
一帧固定32 Byte
```

而应该：

```
根据Negotiated MTU
↓
计算最大Chunk
```

例如：

```
chunk_size <= ATT_MTU - overhead
```

然后你的 OTA 协议可能：

```
offset
length
payload
```

实际 payload 长度动态变化。

---

# 二十六、Data Length Extension

除了 ATT MTU，还有：

```
Link Layer Data Length Extension
DLE
```

它控制 Link Layer 单个 packet 能放多少数据。

第一遍你只需要知道：

```
MTU
→ ATT层

DLE
→ Link Layer层
```

OTA吞吐和两者都有关。

---

# 二十七、PHY

BLE 常见：

```
1M PHY
2M PHY
Coded PHY
```

OTA：

```
2M PHY
```

可能提高速度。

Coded PHY：

```
更远距离
更强抗干扰
但速度慢
```

第一遍你先知道即可。

---

# 二十八、L2CAP

L2CAP：

```
Logical Link Control and Adaptation Protocol
```

作用可以先理解为：

```
上层数据
↓
L2CAP
↓
Link Layer
```

负责：

```
复用
分片/重组
逻辑通道
```

你暂时不用深挖 L2CAP CoC。

但要知道：

> GATT/ATT 数据最终也是通过 L2CAP 承载。

---

# 二十九、BLE 自己已经可靠，为什么 OTA 还要 CRC

这是非常重要的。

BLE 底层已经有：

```
CRC
ACK
重传
```

那为什么 Firmware 还要：

```
CRC32
```

因为两者不是一个层次。

BLE Link Layer CRC：

```
保证某一个无线Packet传输没坏
```

Firmware CRC：

```
保证整个最终Flash Image完整
```

例如：

```
100 KB Firmware
```

即使无线每一包都正常，你仍然可能发生：

```
写Flash错误
漏写
地址错误
软件Bug
固件截断
```

所以：

```
BLE packet integrity
≠ Firmware image integrity
```

OTA最终仍然必须：

```
expected_crc
vs
actual_crc
```

---

# 三十、OTA 的 BLE GATT 设计

你以后完全可以设计：

```
OTA Service
│
├── Control Characteristic
│
├── Data Characteristic
└── Status Characteristic
```

推荐职责：

```
Control
→ Write
→ BEGIN / FINISH / ABORT

Data
→ Write Without Response
→ Firmware Chunk

Status
→ Notify
→ ACK / offset / error / progress
```

这个结构非常经典，也很适合你现在 Bootloader 协议迁移过去。

---

# 三十一、你现在 Bootloader CMD 怎么映射到 BLE

你现在串口：

```
AA 55 | LEN | CMD | DATA | SUM
```

以后 BLE 不一定还要整个套一层：

```
AA 55
```

因为 GATT 本身已经提供边界。

你可以把逻辑层保留：

```
BEGIN_UPDATE
WRITE_APP
FINISH_UPDATE
GET_STATUS
```

但 BLE 层变成：

```
Control Characteristic
→ BEGIN_UPDATE

Data Characteristic
→ WRITE_APP payload

Control Characteristic
→ FINISH_UPDATE
```

也就是说：

> OTA 的核心命令可以复用，但传输封装不一定照搬 UART Frame。

---

# 三十二、BLE OTA 数据包怎么设计

例如：

```
typedef struct
{
    uint32_t offset;
    uint16_t length;
    uint8_t data[];
} OtaChunk_t;
```

发送：

```
offset = 0
payload = 第0~243 Byte

offset = 244
payload = 下一块

offset = 488
...
```

设备：

```
收到Chunk
↓
检查offset
↓
写Flash
↓
更新received_size
```

---

# 三十三、为什么 offset 很重要

没有 offset：

```
默认：
每来一包
write_addr += length
```

如果：

```
某包重复
某包漏了
某包乱序
```

就麻烦。

有 offset：

```
Chunk:
offset = 4096
```

设备明确知道：

```
APP_START + 4096
```

该写哪里。

这也是你现在 Bootloader 应该提前改的地方。

---

# 三十四、ACK 怎么做

不要每一小包都 ACK，否则 BLE OTA 很慢。

第一版可以：

```
每 N 个 Chunk
↓
设备 Notify
↓
current_offset
```

例如：

```
PC/Phone发送：
0~4095

Device Notify:
ACK offset = 4096
```

如果中间断线：

```
重新连接
↓
GET_STATUS
↓
offset = 4096
↓
继续
```

这就是以后断点续传。

---

# 三十五、第一版 OTA 可以不做断点续传

你现在不要一开始搞这么复杂。

第一版：

```
断线
↓
重新连接
↓
重新BEGIN_UPDATE
↓
Erase
↓
从0开始
```

已经够用了。

第二版再加：

```
current_offset
```

---

# 三十六、BLE OTA 状态机

设备端建议：

```
IDLE
↓
RECEIVING
↓
VERIFYING
↓
VALID
```

异常：

```
RECEIVING
↓
ERROR
```

或者：

```
ABORTED
```

你现有 Bootloader Metadata：

```
EMPTY
RECEIVING
VALID
```

正好可以复用。

---

# 三十七、BLE断开怎么办

BLE OTA 必须设计：

```
Disconnect Event
```

例如：

```
OTA进行中
↓
BLE disconnected
↓
state仍保持 RECEIVING
↓
禁止Jump
```

第一版：

```
重新连接以后
重新升级
```

以后：

```
GET_STATUS
↓
从offset继续
```

---

# 三十八、安全：Pairing / Bonding

OTA 以后最好不能：

```
附近任何人
↓
连接
↓
刷Firmware
```

所以需要考虑：

```
Pairing
Bonding
Authentication
Encryption
Authorization
```

第一版学习：

```
Pairing
→ 建立安全关系

Bonding
→ 保存长期密钥

Encryption
→ 链路数据加密
```

但是要注意：

> BLE加密不等于固件可信。

---

# 三十九、为什么 OTA 还要数字签名

CRC：

```
检测损坏
```

但攻击者完全可以：

```
修改Firmware
↓
重新算一个CRC
```

所以安全 OTA 应该：

```
Hash
+
Digital Signature
```

Bootloader 内置公钥：

```
Firmware
↓
Hash
↓
Verify Signature
↓
合法开发者发布？
```

这才叫：

```
Authenticity
```

所以：

```
CRC
→ Integrity against accidental corruption

Signature
→ Authenticity / security
```

你现在第一版做 CRC 足够，后面知道签名即可。

---

# 四十、OTA Version

BEGIN_UPDATE 建议未来包括：

```
version
image_size
expected_crc
```

例如：

```
typedef struct
{
    uint32_t version;
    uint32_t image_size;
    uint32_t expected_crc;
} OtaInfo_t;
```

Bootloader 可以判断：

```
新版本 <= 当前版本
↓
拒绝
```

这样可以避免：

```
意外降级
```

安全 OTA 里还涉及：

```
Anti-rollback
```

以后再学。

---

# 四十一、BLE OTA 和 Bootloader 到底怎么分工

这是你一定要理解的架构问题。

BLE 协议栈往往运行在：

```
Application
```

而 Bootloader 尽量简单。

所以一种很常见思路：

```
APP负责：
BLE连接
下载Firmware
写入备用区域
验证基础信息

Bootloader负责：
最终验证
选择Image
切换版本
Rollback
```

而不是：

```
Bootloader里面塞完整BLE Stack
```

因为那会让 Bootloader：

```
非常大
非常复杂
攻击面更大
难维护
```

---

# 四十二、如果只有一个 APP 区怎么办

你现在目前还是：

```
Bootloader
+
Single APP
```

那可以：

```
BLE APP
↓
接收固件
↓
通知Bootloader进入升级模式
↓
Bootloader从某个暂存区升级APP
```

但如果内部 Flash 没有足够备用空间，就比较麻烦。

所以 OTA 更自然的结构是：

```
Bootloader
+
Slot A
+
Slot B
+
Metadata
```

---

# 四十三、BLE OTA 最终理想架构

```
Phone
│
│ BLE
▼
Application
│
├── BLE Stack
├── OTA Service
├── UpdateTask
└── Download Manager
        │
        ▼
      Slot B
        │
        ▼
    CRC / Signature
        │
        ▼
      Metadata
        │
       Reboot
        ▼
    Bootloader
        │
        ├── Verify
        ├── Select Slot
        └── Boot
```

这就是你以后真正想达到的架构。

---

# 四十四、你当前阶段 BLE 应该掌握到什么程度

你现在没必要学：

```
PHY调制细节
Channel Map算法
Frequency Hopping数学
LL state machine所有PDU
HCI每个Opcode
L2CAP CoC深层实现
SMP密码学细节
```

你当前最重要的是：

```
第一优先级
GAP
Advertising
Scanning
Connection
Central / Peripheral

第二优先级
GATT
Service
Characteristic
UUID
Read / Write
Write Without Response
Notify / Indicate

第三优先级
ATT MTU
Connection Interval
Supervision Timeout
DLE基本概念
吞吐量

第四优先级
BLE OTA架构
Chunk
Offset
ACK
CRC
Metadata
Disconnect Recovery

第五优先级
Pairing
Bonding
Encryption
Signature
A/B
Rollback
```

---

# 四十五、给你整理成真正的学习路线

我建议你接下来 BLE 按这个顺序：

```
Day 1
BLE整体架构
GAP
Advertising
Scanning
Central / Peripheral

Day 2
GATT
Service
Characteristic
UUID
Client / Server

Day 3
Read
Write
Write Without Response
Notify
Indicate
ATT

Day 4
MTU
Connection Interval
Supervision Timeout
DLE
吞吐

Day 5
做一个：
Phone ↔ BLE Device
Control + Data + Notify

Day 6
设计OTA Service
Control Char
Data Char
Status Char

Day 7
把：
BEGIN_UPDATE
WRITE_APP
FINISH_UPDATE
映射到BLE

Day 8+
Chunk
Offset
CRC
Metadata
断线恢复

之后
A/B
Rollback
Signature
```

---

# 四十六、你简历里的 ESP32 BLE 项目应该补到什么程度

你现在至少应该能回答这些：

```
为什么选择Advertising？
Manufacturer Data是什么？
广播包最大能放多少有效数据？
广播为什么会丢？
8位Sequence Number有什么作用？
为什么广播不适合Firmware OTA？
Central / Peripheral是什么？
GATT Service是什么？
Characteristic是什么？
Notify和Write有什么区别？
MTU为什么影响吞吐？
```

等你把这些学完，你之前那个项目就不会再是：

> “我用了 BLE 广播。”

而会变成：

> “我知道为什么这么设计，以及这个设计的局限在哪里。”

---

最后我给你压缩成你现在最值得背的一张图：

```
BLE
│
├── GAP
│   ├── Advertising
│   ├── Scanning
│   ├── Central
│   └── Peripheral
│
├── GATT
│   ├── Service
│   ├── Characteristic
│   ├── Read
│   ├── Write
│   ├── Notify
│   └── Indicate
│
├── ATT
│   ├── Handle
│   └── MTU
│
├── Connection
│   ├── Interval
│   ├── Latency
│   └── Supervision Timeout
│
└── OTA
    ├── BEGIN_UPDATE
    ├── Chunk
    ├── Offset
    ├── WRITE
    ├── ACK
    ├── CRC
    ├── Metadata
    ├── VALID
    ├── Disconnect Recovery
    └── A/B + Rollback
```

你现在最合适的下一步就是：**先把 GAP + Advertising + GATT + Characteristic 这四块彻底学透，然后再进入 BLE OTA**