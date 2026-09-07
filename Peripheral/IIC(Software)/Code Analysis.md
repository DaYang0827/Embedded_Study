# 1.基础设置
使用软件IIC，相当于用GPIO手动模拟SCL/SDA时序，GPIO使用OUT模式不是AF模式，PB6 和 PB7 要作为普通输出引脚使用

通信对象：BL24C512 EEPROM

总线引脚：SCL PB6，SDA PB7

通信方式：软件模拟 IIC

GPIO模式：**普通输出 OUT**

输出类型：**开漏 OD**

上下拉：**上拉 UP**

设备地址：0x50，代码中使用 7 位地址

写方向地址：**0xA0**

读方向地址：**0xA1**

存储地址长度：0x0010,16 bit，先输入高 8 位，再输入低 8 位

写操作：Start -> 设备写地址 -> 存储地址高位 -> 存储地址低位 -> 数据 -> Stop

读操作：Start -> 设备写地址 -> 存储地址高位 -> 存储地址低位 -> ReStart -> 设备读地址 -> 读数据 -> NACK -> Stop

# 2.main.c
``` c
int main(void)
{
    uint8_t write_data = 0xA5;
    uint8_t read_data = 0;

    usart_init(&usart1);
    iic_init(&iic1);

    usart_send_string(&usart1, "IIC BL24C512 Test Start\r\n");

    bl24c512_write_byte(&iic1, 0x0010, write_data);

    read_data = bl24c512_read_byte(&iic1, 0x0010);

    if (read_data == write_data)
    {
        usart_send_string(&usart1, "BL24C512 Write/Read Success\r\n");
    }
    else
    {
        usart_send_string(&usart1, "BL24C512 Write/Read Failed\r\n");
    }

    while (1)
    {
    }
}
```
# 3.IIC.c
``` c
//定义iic的结构体
typedef_iic iic1 =
{
    .GPIOx = GPIOB,

    .GPIO_Clk = RCC_AHB1Periph_GPIOB,

    .SCL_Pin = GPIO_Pin_6,
    .SDA_Pin = GPIO_Pin_7,
};

//IIC的延迟
static void iic_delay(void)
{
    volatile uint32_t i;

    for (i = 0; i < 50; i++)
    {
        ;
    }
}

//eeprom的写入延迟
static void eeprom_write_delay(void)
{
    volatile uint32_t i;

    for (i = 0; i < 500000; i++)
    {
        ;
    }
}

//将scl拉高
static void iic_scl_high(typedef_iic* iic)
{
    GPIO_SetBits(iic->GPIOx, iic->SCL_Pin);
}

//将scl拉低
static void iic_scl_low(typedef_iic* iic)
{
    GPIO_ResetBits(iic->GPIOx, iic->SCL_Pin);
}

//将sda拉高
static void iic_sda_high(typedef_iic* iic)
{
    GPIO_SetBits(iic->GPIOx, iic->SDA_Pin);
}

//将sda拉低
static void iic_sda_low(typedef_iic* iic)
{
    GPIO_ResetBits(iic->GPIOx, iic->SDA_Pin);
}

//读SDA的状态
static uint8_t iic_read_sda(typedef_iic* iic)
{
    return GPIO_ReadInputDataBit(iic->GPIOx, iic->SDA_Pin);
}

//初始化IIC
void iic_init(typedef_iic* iic)
{
    RCC_AHB1PeriphClockCmd(iic->GPIO_Clk,ENABLE);
    RCC_APB1PeriphClockCmd(iic->iic_Clk,ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;			    //使用OUT，因为需要使用GPIO手动模拟SCL、SDA的时序，如果是硬件模拟就可以使用AF模式	
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;				//IIC 必须使用开漏输出。原因是 IIC 总线上可能挂多个设备。如果这块板子的 I2C2 上就挂了多个传感器，多个设备共用 SDA/SCL 时，不能让某个设备强行输出高电平，否则会和其他设备冲突
    GPIO_InitStruct.GPIO_Pin = iic->SCL_Pin | iic->SDA_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;                //IIC 总线空闲时，SCL 和 SDA 都应该是高电平
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(iic->GPIOx, &GPIO_InitStruct);

    iic_scl_high(iic);
    iic_sda_high(iic);
}

//IIC的开始条件为：当 SCL 保持高电平时，SDA 从高电平变成低电平
void iic_start(typedef_iic * iic)
{
    iic_sda_high(iic);
    iic_scl_high(iic);
    iic_delay();

    iic_sda_low(iic);
    iic_delay();

    iic_scl_low(iic);
    iic_delay();
}

//IIC的停止条件：当SCL保持高电平时，SDA从低电平变成高电平
void iic_stop(typedef_iic* iic)
{
    iic_scl_low(iic);
    iic_sda_low(iic);
    iic_delay();

    iic_scl_high(iic);
    iic_delay();

    iic_sda_high(iic);
    iic_delay();
}

/*IIC的发送逻辑是高位先发，先发bit7，再到bit6，最后bit0
  IIC发送都是按照字节（8个比特）发送，每发送一个字节就需要加一个应答位
  IIC的数据规则是：SCL 低电平时，主机改变 SDA；SCL 高电平时，从机读取 SDA*/
void iic_send_byte(typedef_iic* iic, uint8_t data)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        iic_scl_low(iic);

        if (data & 0x80)							 //判断最高位是不是1，在if中只要内部结果不是0，就代表条件成立
        {
            iic_sda_high(iic);                       //如果最高位是1，就把SDA拉高，及在SDA中写入1
        }
        else
        {
            iic_sda_low(iic);                        //如果最高位是0，把SDA拉低，在SDA中写入0
        }

        data <<= 1;								     //将数据左移一位，把下一位移到最高位，准备下一次发送

        iic_delay();

        iic_scl_high(iic);                           //每次写入一个bit都需要让SCL有一个跳变，当SCL为高的时候，就是从机去读取这个bit
        iic_delay();

        iic_scl_low(iic);
        iic_delay();
    }
}

/*ACK的应答机制，IIC每次发送完8bit，一个字节之后，还需要第九个时钟，第九个时钟用于ACK
  ACK的规则是：SDA = 0：从机应答，ACK
              SDA = 1：从机没有应答，NACK*/
uint8_t iic_wait_ack(typedef_iic * iic)
{
    uint16_t timeout = 0;

    iic_sda_high(iic);                                //主机发完 8 bit 后，要释放 SDA，让从机控制 SDA
    iic_delay();

    iic_scl_high(iic);                                //主机拉高SCL，这个时候读取 SDA的状态
    iic_delay();

    while (iic_read_sda(iic))
    {
        timeout++;

        if (timeout > 250)
        {
            iic_stop(iic);
            return 1;
        }
    }

    iic_scl_low(iic);
    iic_delay();

    return 0;
}

//IIC对应的ACK
void iic_ack(typedef_iic* iic)
{
    iic_scl_low(iic);
    iic_sda_low(iic);
    iic_delay();

    iic_scl_high(iic);
    iic_delay();

    iic_scl_low(iic);
    iic_delay();
}

//IIC对应的NACK
void iic_nack(typedef_iic* iic)
{
    iic_scl_low(iic);
    iic_sda_high(iic);
    iic_delay();

    iic_scl_high(iic);
    iic_delay();

    iic_scl_low(iic);
    iic_delay();
}

/*IIC读取一个字节
  STM32 通过 SCL 产生 8 个时钟脉冲，每次在 SCL 高电平时读取 SDA，最后组合成 1 个字节。
  第二个参数ack，表示在读完这个字节之后，主机要不要发送ACK
  ack = 1：读完后发送 ACK，表示我还要继续读
  ack = 0：读完后发送 NACK，表示我不继续读了*/
uint8_t iic_read_byte(typedef_iic* iic, uint8_t ack)
{
    uint8_t i;
    uint8_t data = 0;

    iic_sda_high(iic);                                //释放 SDA，让 EEPROM 可以控制 SDA

    for(i = 0; i < 8; i++)
    {
        data <<= 1;                                    //data左移一位，给新读到的bit腾出最低位的位置

        iic_scl_low(iic);                              //先拉低SCL，EEPROM 在 SCL 低电平期间准备下一位数据；STM32 在 SCL 高电平期间读取 SDA
        iic_delay();

        iic_scl_high(iic);   
        iic_delay();

        if (iic_read_sda(iic))                        /*在SCL为高的时候读取数据，如果 SDA 是高电平，说明当前 bit 是 1：data |= 0x01; 如果 SDA 是低电平，说明当前 bit 是0，
                                                        那就什么都不做。因为前面已经：data <<= 1;所以最低位默认是 0。*/
        {
            data |= 0x01;
        }

        iic_scl_low(iic);                            //读完之后拉低SCL，准备读取下一位
        iic_delay();
    }

    if (ack)                                        //如果读完一个字节，还想读下一个字节就发送ACK
    {
        iic_ack(iic);
    }
    else
    {
        iic_nack(iic);                               //如果读完这个字节主机不想继续读，就发送NACK
    }

    return data;
}

/*BL24C512 写一个字节的协议流程
  BL24C512 是 EEPROM。它不是普通传感器寄存器那种 8 位地址，而是有 16 位存储地址*/
uint8_t bl24c512_write_byte (typedef_iic *iic, uint16_t mem_addr, uint8_t data)
{
    iic_start(iic);

    iic_send_byte(iic, BL24C512_ADDR << 1);                      //发送设备地址 + 写方向，因为写对应的是0，所以可以整体左移一位
    if (iic_wait_ack(iic))
    {
        return 1;                                                //此时的返回值代表的是对/错布尔状态，1代表失败
    }

    iic_send_byte(iic, (uint8_t)(mem_addr >> 8));                // 发送存储地址高 8 位 
    if (iic_wait_ack(iic))
    {
        return 1;
    }

    iic_send_byte(iic, (uint8_t)(mem_addr & 0xFF));              // 发送存储地址低 8 位 
    if (iic_wait_ack(iic))
    {
        return 1;
    }

    iic_send_byte(iic, data);                                    // 发送要写入的数据 
    if (iic_wait_ack(iic))
    {
        return 1;
    }

    iic_stop(iic);

    eeprom_write_delay();                                        //EEPROM 写入后需要等待内部写周期，EEPROM 写入数据后，不是马上完成。它内部要经历一个写周期，通常几毫秒

    return 0;
}


/**
 * @brief 从 BL24C512 读取一个字节
 * @param mem_addr EEPROM 内部存储地址
 * @retval 读取到的数据
 */
uint8_t bl24c512_read_byte(typedef_iic *iic, uint16_t mem_addr)
{
    uint8_t data;

    iic_start(iic);

    /* 发送设备地址 + 写方向 */
    iic_send_byte(iic, BL24C512_ADDR << 1);                    
    if (iic_wait_ack(iic))
    {
        return 0xFF;
    }

    /* 发送存储地址高 8 位 */
    iic_send_byte(iic, (uint8_t)(mem_addr >> 8));            //（uint8_t）强制类型转化，会强制读低八位的数据。可以将16位的数据直接右移八位，强制类型转换之后就是原来的高八位
    if (iic_wait_ack(iic))                                   //SDA是0，表示ACK；SDA是1，表示NACK
    {
        return 0xFF;                                        //代表 “发生了通信错误，直接退出函数并返回一个‘错误代号’”
    }

    /* 发送存储地址低 8 位 */
    iic_send_byte(iic, (uint8_t)(mem_addr & 0xFF));
    if (iic_wait_ack(iic))
    {
        return 0xFF;
    }

    /*
     * 重复起始信号
     * 从写地址切换到读数据
     */
    iic_start(iic);

    /* 发送设备地址 + 读方向 */
    iic_send_byte(iic,(BL24C512_ADDR << 1) | 0x01);
    if (iic_wait_ack(iic))
    {
        return 0xFF;
    }

    /* 读取一个字节，最后发送 NACK */
    data = iic_read_byte(iic, 0);

    iic_stop(iic);

    return data;
}


/**
 * @brief 连续写入多个字节
 * @note  这里用单字节循环写，速度慢，但最稳，适合初学模板
 */
void bl24c512_write_buffer(typedef_iic *iic, uint16_t mem_addr, uint8_t *data, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        bl24c512_write_byte(iic, mem_addr + i, data[i]);
    }
}


/**
 * @brief 连续读取多个字节
 */
void bl24c512_read_buffer(typedef_iic *iic, uint16_t mem_addr, uint8_t *data, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        data[i] = bl24c512_read_byte(iic, mem_addr + i);
    }
}
```

# 4.IIC.h
```c
#define BL24C512_ADDR    0x50

typedef struct 
{
   I2C_TypeDef *I2Cx;
   GPIO_TypeDef *GPIOx;

   uint32_t GPIO_Clk;
   uint32_t iic_Clk;

   uint16_t SCL_Pin;
   uint16_t SDA_Pin;
}typedef_iic;

extern typedef_iic iic1;

void iic_init(typedef_iic* iic);

void iic_start(typedef_iic *iic);
void iic_stop(typedef_iic *iic);

void iic_send_byte(typedef_iic *iic, uint8_t data);
uint8_t iic_read_byte(typedef_iic *iic, uint8_t ack);

uint8_t iic_wait_ack(typedef_iic *iic);
void iic_ack(typedef_iic *iic);
void iic_nack(typedef_iic *iic);

uint8_t bl24c512_write_byte(typedef_iic *iic, uint16_t mem_addr, uint8_t data);
uint8_t bl24c512_read_byte(typedef_iic *iic, uint16_t mem_addr);

void bl24c512_write_buffer(typedef_iic *iic, uint16_t mem_addr, uint8_t *data, uint16_t len);
void bl24c512_read_buffer(typedef_iic *iic, uint16_t mem_addr, uint8_t *data, uint16_t len);
```
