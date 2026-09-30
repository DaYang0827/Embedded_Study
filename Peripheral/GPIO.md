# 1 基本概念

先从最核心的硬件模型开始。可以把一个 MCU GPIO 引脚简化成这样：

```
                        VDD
                         |
                    [内部上拉]
                         |
外部引脚 Pin -------------+------------------> 输入缓冲器 --> IDR
                         |
                    [内部下拉]
                         |
                        GND

                         |
                         +--> 输出驱动级
                               |
                         +-----+-----+
                         |           |
                       PMOS        NMOS
                         |           |
                        VDD         GND
```

软件实际上是在控制这些硬件开关。最重要的第一层概念就是$\boxed{\text{GPIO 不是简单的“0和1”，而是一套可配置的输入/输出电路}}$

# 2 输入模式

当 GPIO 配成输入时，MCU 主要是“看外面的电压”。比如：

```c
GPIOA->MODER &= ~(3 << (0 * 2));
```

把 PA0 配成输入以后，引脚电平会经过输入缓冲器，然后反映到GPIOA->IDR 例如：

```
if (GPIOA->IDR & (1 << 0))
{
    // PA0 是高电平
}
```

这里有个关键点：**输入模式下，MCU 不主动把引脚拉高或拉低**。所以如果这个引脚什么都没接，就会出现$\boxed{\text{Floating Input，浮空输入}}$  

浮空就是**引脚电压没有被可靠地固定在高或低**。此时它可能受到周围电磁噪声影响，读出来：

```
0
1
0
1
1
0
```

乱跳。所以输入脚经常需要$\boxed{\text{上拉或下拉}}$ 例如按键：

```
VDD
 |
[上拉电阻]
 |
 +------ GPIO
 |
按键
 |
GND
```

没按`GPIO = 1` 按下`GPIO = 0`  这叫$\boxed{\text{Active Low}}$ 也就是“低电平有效”。

## 2.1 上拉和下拉

上拉就是通过一个电阻把引脚“偏向高电平”。

```
VDD
 |
[R]
 |
GPIO
```

下拉则相反：

```
GPIO
 |
[R]
 |
GND
```

所以可以记$\boxed{\text{Pull-up：默认 1}}$ $\boxed{\text{Pull-down：默认 0}}$ STM32 内部通常有弱上拉/弱下拉，通过 `PUPDR` 控制。

为什么一定要有电阻，而不是直接接 VDD？

因为如果直接`GPIO ---- VDD`然后外部某个器件又把它拉到GND `VDD ----- GND`相当于短路。加电阻以后$I=\frac{V}{R}$ 电流被限制住。这就是硬件里一个非常重要的思想$\boxed{\text{电阻不仅是“改电压”，更经常用来限制电流和定义状态}}$

# 3 输出模式

输出时 MCU 主动控制引脚。典型有两种$\boxed{\text{Push-Pull}}$ 和$\boxed{\text{Open-Drain}}$这是 GPIO 面试里非常高频的知识。

## 3.1 推挽输出

推挽结构可以简化成：

```
     VDD
      |
    PMOS
      |
GPIO PIN
      |
    NMOS
      |
     GND
```

输出高：

```
PMOS ON
NMOS OFF
```

引脚主动连接到 VDD。所以$V_{OUT}\approx VDD$ 输出低：

```
PMOS OFF
NMOS ON
```

引脚主动连接 GND。所以$V_{OUT}\approx 0V$

因此推挽$\boxed{\text{既可以主动拉高，也可以主动拉低}}$ 优点就是**速度快、驱动能力强**。比如：

```
LED
SPI Clock
普通数字控制线
UART TX
```

很多都可以用推挽。

## 3.2 开漏输出

Open-Drain 可以理解成：

```text
GPIO PIN
   |
 NMOS
   |
  GND
```

注意没有上面的 PMOS。所以它只能：

```
NMOS ON
→ 主动输出低

NMOS OFF
→ 不输出
→ 高阻态
```

因此$\boxed{\text{开漏不能主动输出高电平}}$ 它的高电平一般靠外部上拉：

```text
VDD
 |
[R]
 |
 +------ GPIO
 |
NMOS
 |
GND
```

当 NMOS 关闭：

```
VDD
 |
R
 |
GPIO
```

所以 GPIO 被电阻拉高。当 NMOS 导通`GPIO → GND`于是变成低电平。所以$\boxed{ Open\ Drain= \text{LOW 或 Hi-Z} }$ 不是$\text{LOW 或 HIGH}$ 这个区别特别重要。

### 3.2.1 I²C 使用开漏

这是经典面试题。因为 I²C 有**多个设备共享SDA、SCL**。

假设两个设备用推挽模式，会发生“**总线短路**”

I2C 是一根总线上挂载**多个设备**（一个主设备，多个从设备）的通信协议。假设总线上挂了设备 A 和设备 B，它们都配置为**推挽输出**：

- **正常时：** 大家都不说话。
- **冲突时：** 某一时刻，设备 A 想要发送逻辑 `1`（推挽输出**高电平**，内部 PMOS 导通，引脚强力接 3.3V）；而设备 B 恰好想要发送逻辑 `0`（推挽输出**低电平**，内部 NMOS 导通，引脚强力接地 0V）。

**结果：** 3.3V 通过设备 A 的 PMOS 和设备 B 的 NMOS **直接连在了一起，形成没有电阻的死短路！**  
由于推挽模式的驱动能力极强（电阻接近 0），总线上会产生巨大的瞬间电流，**直接烧毁设备 A 或设备 B 的引脚**。因此，推挽模式绝对不能直接用于多设备共用的总线。

而开漏则不一样。设备只能：

```
拉低
或者
松开
```

所以：

```text
所有人松开
→ 上拉电阻把总线拉高

任何一个人拉低
→ 总线就是低
```

也就是$\boxed{ 0 \text{ 有统治权} }$ 这叫 wired-AND / wired-OR 类似的总线结构。所以 I²C 可以安全地让多个设备共享总线。

### 3.2.2 高阻态 Hi-Z

会非常频繁遇到$\boxed{\text{High Impedance}}$ 高阻态**不是HIGH，而是基本不驱动这个引脚**。可以想成`开关断开内部电路   X   GPIO` 这时候 GPIO 电压由外部电路决定。所以：

```
HIGH
LOW
Hi-Z
```

其实是三种不同状态。

其中：

```
HIGH → 主动/被动高电平
LOW  → 低电平
Hi-Z → 不主动控制
```

这就是“三态输出”里的第三态。

# 4 Alternate Function

STM32 里经常看到：

```
GPIO_Mode_AF
```

AF = Alternate Function。意思不是 GPIO 消失了，而是**这个物理引脚不再由普通 GPIO 外设控制，而交给 USART / SPI / TIM / I²C 等片上外设控制**。比如`PA9`可以是：

```
普通 GPIO
或者
USART1_TX
```

当配置`GPIO_Mode_AF`再设置`AF7 = USART1`内部相当于做了一个多路选择器：

```
                GPIO controller
                    |
                    |
Pin <------ MUX ----+
                    |
                    |
                 USART1
```

所以$\boxed{\text{AF 本质是引脚复用}}$这也是为什么 STM32 一个引脚能有很多功能。

# 5 Analog 模式

还有`GPIO_Mode_AN` Analog。主要给：

```text
ADC
DAC
COMP
```

等模拟外设使用。这时候数字输入缓冲器通常会关闭。因为模拟信号可能停留在：

```
1.2V
1.7V
2.1V
```

这种“既不是明确 0 也不是明确 1”的区域。如果数字输入缓冲器一直开着，可能：

- 增加功耗
- 产生不必要的数字翻转
- 干扰模拟测量

所以模拟模式通常会把数字路径关掉。

# 6 输入输出电压

## 6.1 输入高低电平

这是特别重要的一点。很多人会认为：

```
0V = 0
3.3V = 1
```

但真实芯片不是这么理想。Datasheet 通常会定义：

```
VIL = maximum low-level input voltage
VIH = minimum high-level input voltage
```

比如假设：

```
VIL <= 0.3VDD
VIH >= 0.7VDD
```

如果`VDD=3.3V`那么大概`VIL<0.99V` 可以认为是低。`VIH>2.31V`可以认为是高。中间`0.99V ~ 2.31V`属于$\boxed{\text{Undefined Region}}$不保证读成什么。

所以数字电路不是：

```
只有 0V / 3.3V
```

而是：

```
低电平范围
不确定范围
高电平范围
```

## 6.2 输出电压

输出高也不一定正好3.300V因为输出晶体管有电阻。如果 GPIO 输出高，并且外部负载拉了较大电流$V_{OH}$会下降。输出低时$V_{OL}$也不会绝对等于 0。所以 datasheet 会给：

```
VOH minimum
VOL maximum
```

并且附带测试电流。这就是为什么$\boxed{\text{GPIO 能输出高低电平，不代表能无限带负载}}$。例如不能让 GPIO 直接带：

```
电机
继电器
大功率 LED
```

而应该通过：

```
MOSFET
三极管
Driver IC
```

来驱动。

## 6.3 LED 要串电阻

比如：

```
3.3V
 |
GPIO
 |
LED
 |
GND
```

如果没有限流电阻，LED 导通以后电流可能太大。应该：

```
GPIO
 |
LED
 |
R
 |
GND
```

假设$V_{GPIO}=3.3V LED$ ，压降$V_F=2.0V$，希望$I=5mA$。那么$R=\frac{3.3-2.0}{0.005}$ ，$R=260\Omega$。实际可以选：

```
270Ω
330Ω
```

所以看到板子上 LED 前面那个电阻，不要认为“教材规定要加。”而要知道$\boxed{\text{它是在控制 LED 和 GPIO 的电流}}$

# 7 GPIO 输出速度

STM32 还有：

```
Low speed
Medium speed
High speed
Very high speed
```

这个经常让人误解。它不是CPU 执行 `GPIO_SetBits()` 的速度。而主要是$\boxed{\text{GPIO 输出边沿 Slew Rate}}$也就是`0 → 1`。电压爬升有多快。边沿越快：

- 高频通信更容易工作
- EMI 更强
- 串扰更明显
- 功耗可能更高

所以不是永远Very High Speed最好。原则是$\boxed{\text{够用就行}}$普通 LED：Low Speed就足够。高速 SPI 可能需要更高。

# 8 按键抖动

机械按键按下时，并不是`0 → 1`瞬间完成。真实可能是：

```
0
1
0
1
0
1
1
1
```

持续几毫秒。这叫\boxed{\text{Switch Bounce}}所以软件要消抖，比如：

```c
if (key_pressed())
{
    delay_ms(20);

    if (key_pressed())
    {
        // 真正按下
    }
}
```

或者用：

- 定时器采样
- 状态机
- RC 硬件滤波

# 9 去耦电容

看 MCU 原理图会看到 VDD 旁边很多100nF电容。例如：

```
VDD ----+---- MCU
        |
      100nF
        |
       GND
```

这是\boxed{\text{Decoupling Capacitor}}芯片内部数字电路切换时，会瞬间需要电流。电源线和 PCB 不是理想导线，会有电感和阻抗。所以电容在芯片旁边提供瞬时电流：

```
电容
↓
MCU
```

可以理解成一个非常小的“本地储能池”。以后看到：

```
每个 VDD pin 旁边一个 100nF
```

不是装饰，而是非常重要。

# 10 总结

现在可以把 GPIO 整体知识压缩成下面这张脑图：

```
GPIO
│
├── Input
│   ├── Floating
│   ├── Pull-up
│   └── Pull-down
│
├── Output
│   ├── Push-Pull
│   │    ├── 主动 HIGH
│   │    └── 主动 LOW
│   │
│   └── Open-Drain
│        ├── LOW
│        └── Hi-Z
│
├── Alternate Function
│   ├── UART
│   ├── SPI
│   ├── I2C
│   └── TIM
```