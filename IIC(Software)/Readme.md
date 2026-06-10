# IIC code analysis
---
## 缩写名
---
SDA：Serial Data Line——串行数据线

SCL：Serial Clock Line——串行时钟线

ACK：acknowledge character——应答信号

## 特点
---
IIC是一种**多主机、两线制、低速串行通信总线，广泛用于微控制器和各种外围设备之间的通信**。它使用两条线路：串行数据线（SDA）和串行时钟线（SCL）进行双向传输。

总线结构：
  两线制：使用两条线进行通信，分别是串行数据线（SDA）和串行时钟线（SCL）。
  多主多从结构：支持多个主设备和多个从设备连接到同一总线上。
  
通信方式：
  同步串行：数据传输同步于时钟信号。
  字节格式：每个字节由8位数据构成，加上开始和停止条件以及可选的应答位（ACK）。

##  IIC 开始与停止时序：
---

<img width="792" height="215" alt="image" src="https://github.com/user-attachments/assets/c137d01a-075c-4e9c-9389-59488c8a9a08" />

当SCL 是高电平时，SDA 线从高电平向低电平切换表示起始条件；

当SCL 是高电平时，SDA 线由低电平向高电平切换表示停止条件。

## IIC读写数据
---
### 写数据（主设备给从设备发送/写入数据）：

<img width="692" height="103" alt="image" src="https://github.com/user-attachments/assets/d75d2019-5e0f-44e3-b7ee-50691dd2d0aa" />

1. 主设备发送起始（START）信号

2. 主设备发送设备地址到从设备

3. 等待从设备响应(ACK)

4. 主设备发送数据到从设备，一般发送的每个字节数据后会跟着等待接收来自从设备的响应(ACK)

5. 数据发送完毕，主设备发送停止(STOP)信号终止传输

### 读数据（主设备从从设备接收/读取数据）：

<img width="692" height="110" alt="image" src="https://github.com/user-attachments/assets/54f314a0-70f2-4358-894f-18b157ec82dc" />

1. 设备发送起始（START）信号

2. 主设备发送设备地址到从设备

3. 等待从设备响应(ACK)

4. 主设备接收来自从设备的数据，一般接收的每个字节数据后会跟着向从设备发送一个响应(ACK)

5. 一般接收到最后一个数据后会发送一个无效响应(NACK)，然后主设备发送停止(STOP)信号终止传输

设备地址：从设备地址用来区分总线上不同的从设备，一般发送从设备地址的时候会在最低位加上读/写信号，比如设备地址为0x50，0表示读，1表示写，则读数据就会发送0x50，写数据就会发送0x51。

## 数据有效性：
---

<img width="815" height="266" alt="image" src="https://github.com/user-attachments/assets/539b5487-21a5-4daa-bdf5-f1be4f81b3e8" />

I2C总线进行数据传送时，在SCL的每个时钟脉冲期间传输一个数据位，**时钟信号SCL为高电平期间，数据线SDA上的数据必须保持稳定，只有在时钟线SCL上的信号为低电平期间，数据线SDA上的高电平或低电平状态才允许变化**，因为当SCL是高电平时，数据线SDA的变化被规定为控制命令（START或STOP，也就是前面的起始信号和停止信号）


## 应答信号：
---


接收端收到有效数据后向对方响应的信号，**发送端每发送一个字节(8位)数据，在第9个时钟周期释放数据线去接收对方的应答**。

当SDA是低电平为有效应答(ACK)，表示对方接收成功；

当SDA是高电平为无效应答(NACK)，表示对方没有接收成功。

## 写入数据波形图
---


向 BL24C512 地址 0x0010，写入数据 0xA5：

<img width="1840" height="691" alt="image" src="https://github.com/user-attachments/assets/f7bb0cf7-8d00-4f84-b24e-24366b9444c9" />

1.Start->2.发送设备写地址 0xA0->3.从机 ACK->4.发送 EEPROM 内部地址高 8 位 0x00->5.从机 ACK->6.发送 EEPROM 内部地址低 8 位 0x10->7.从机 ACK->8.发送真正要写入的数据 0xA5->9.从机 ACK->10.Stop

对应函数：bl24c512_write_byte(&iic1, 0x0010, 0xA5);

定义#define BL24C512_ADDR 0x50，及设备地址是0x50，但是IIC在真正的线上传输的时候会变成8位，最低位会有一个读写位。0是写，1是读，所以整体的地址会变为0xA0。之后发送16位的存储地址，0x0010，在IIC中是先发高位再发低位，所以先发00再发10。最后再发真正要发送的内容0xA5.

## 读取数据波形图
---

读 BL24C512 地址 0x0010，读到数据 0xA5：

<img width="1840" height="691" alt="image" src="https://github.com/user-attachments/assets/8b75de38-17c3-4b96-8359-6f73148151de" />

1.Start->2.发送设备写地址 0xA0->3.从机 ACK->4.发送 EEPROM 内部地址高 8 位 0x00->5.从机 ACK->6.发送 EEPROM 内部地址低 8 位 0x10->7.从机 ACK->8.Repeated Start->9.发送设备读地址 0xA1->10.从机 ACK->11.读取数据 0xA5->12.主机发送 NACK->13.Stop

对应函数：read_data = bl24c512_read_byte(&iic1, 0x0010);








