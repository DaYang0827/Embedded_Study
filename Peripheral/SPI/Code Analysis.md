# 1.SPI.h
``` c
#define SPI_TIMEOUT                     1000000U

typedef struct                                              //硬件配置层
{
    SPI_TypeDef* SPIx;
    uint32_t SPI_CLK;

    GPIO_TypeDef* GPIOx;
    uint32_t GPIO_CLK;

    uint16_t SCK_Pin;
    uint16_t MISO_Pin;
    uint16_t MOSI_Pin;

    uint8_t SCK_PinSource;
    uint8_t MISO_PinSource;
    uint8_t MOSI_PinSource;

    uint8_t GPIO_AF;

    GPIO_TypeDef * CS_GPIOx;                                //配置CS，通过CS片选设备，CS = 0选中设备
    uint32_t CS_GPIO_CLK;
    uint16_t CS_Pin;

    uint16_t BaudRatePrescaler;                            //SPI分频
    uint16_t CPOL;
    uint16_t CPHA;
}typedef_spi;

extern typedef_spi spi1_flash1;

void spi_init(typedef_spi* spi);

void spi_cs_low(typedef_spi *spi);
void spi_cs_high(typedef_spi *spi);

uint8_t spi_transfer_byte(typedef_spi *spi, uint8_t tx_data);

void spi_send_byte(typedef_spi *spi, uint8_t data);
uint8_t spi_read_byte(typedef_spi *spi);

void spi_send_data( typedef_spi *spi, const uint8_t *data, uint16_t len);
void spi_read_data(typedef_spi *spi, uint8_t *data, uint16_t len);

#endif
```

# 2.SPI.c
``` c
typedef_spi spi1_flash1 = {
    .SPIx = SPI1,
    .SPI_CLK = RCC_APB2Periph_SPI1,

    .GPIOx = GPIOA,
    .GPIO_CLK = RCC_AHB1Periph_GPIOA,
    .GPIO_AF = GPIO_AF_SPI1,
    
    .MISO_Pin = GPIO_Pin_6,
    .MISO_PinSource = GPIO_PinSource6,

    .MOSI_Pin = GPIO_Pin_7,
    .MOSI_PinSource = GPIO_PinSource7,

    .SCK_Pin = GPIO_Pin_5,
    .SCK_PinSource = GPIO_PinSource5,
    
    .CS_GPIOx = GPIOE,
    .CS_GPIO_CLK = RCC_AHB1Periph_GPIOE,
    .CS_Pin = GPIO_Pin_13,
    
    .CPHA = SPI_CPHA_1Edge,
    .CPOL = SPI_CPOL_Low,

    .BaudRatePrescaler = SPI_BaudRatePrescaler_16,  
};

void spi_init(typedef_spi* spi)
{
    RCC_AHB1PeriphClockCmd(spi->GPIO_CLK | spi->CS_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(spi->SPI_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                                  //SCK 和 MOSI 需要主动输出高电平和低电平
    GPIO_InitStruct.GPIO_Pin = spi->MISO_Pin | spi->MOSI_Pin | spi->SCK_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;                                //内部不使用上拉或下拉
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi->GPIOx, &GPIO_InitStruct);

    GPIO_PinAFConfig(spi->GPIOx, spi->SCK_PinSource, spi->GPIO_AF);             //SPI1自动输出SCK
    GPIO_PinAFConfig(spi->GPIOx, spi->MOSI_PinSource, spi->GPIO_AF);            //SPI1自动输出MOSI
    GPIO_PinAFConfig(spi->GPIOx, spi->MISO_PinSource, spi->GPIO_AF);            //SPI1自动输出MISO

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = spi->CS_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi->CS_GPIOx, &GPIO_InitStruct);

    GPIO_SetBits(spi->CS_GPIOx, spi->CS_Pin);                                    //CS默认为1，表示不选中W25Q128

    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_BaudRatePrescaler = spi->BaudRatePrescaler;                //SPI进行16分频     84MHz/16
    SPI_InitStruct.SPI_CPHA = spi->CPHA;                                          //上升沿采样数据，下降沿修改数据
    SPI_InitStruct.SPI_CPOL = spi->CPOL;                                          //SCK空闲时为低电平
    SPI_InitStruct.SPI_CRCPolynomial = 7;                                         //没有设置CRC，默认为7
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                                //每次传输8个bite，一个字节
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;               //全双工模式，两根数据线，支持同时接收和发送
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                               //表示高位先发，bit7先发送，bit0最后发送
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                                    //STM32为主机，负责产生SCK、控制CS、发送命令、决定读取时机
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;                                        //NSS 本质上就是 SPI 外设内部使用的片选管理功能
    SPI_Init(spi->SPIx, &SPI_InitStruct);

    SPI_NSSInternalSoftwareConfig(spi->SPIx, SPI_NSSInternalSoft_Set);            //相当于把 SPI 外设内部的 NSS 保持为高电平

    SPI_Cmd(spi->SPIx, ENABLE);
}


/*CS拉低，片选设备*/
void spi_cs_low(typedef_spi*spi)                                                 
{
    GPIO_ResetBits(spi->CS_GPIOx, spi->CS_Pin);
}


/*CS拉高，结束设备选择*/
void spi_cs_high(typedef_spi *spi)
{
    uint32_t timeout = SPI_TIMEOUT;

    /*先等待BSY清零，
      如果BSY为1代表：1.SPI 移位寄存器仍然在发送或接收数据 2.SCK 波形可能还没有结束
      这里不能用TXE做为判断标志，TXE = 1 不代表总线已经完全发送结束，TXE 只表示发送数据寄存器可以写入新数据
      真正判断最后一位bit是否完成应该看BSY(BUSY)*/
    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_BSY) == SET)            
    {
        if (timeout-- == 0)
        {
            break;
        }
    }

    GPIO_SetBits(spi->CS_GPIOx, spi->CS_Pin);
}

/*核心函数，通过此函数，SPI进行数据交换
  通过 MOSI 发送 1 个字节，同时通过 MISO 接收 1 个字节。*/
uint8_t spi_transfer_byte(typedef_spi *spi, uint8_t tx_data)
{
    uint32_t timeout = SPI_TIMEOUT;

    /*等待TXE
      TXE = 1：可以写入新的发送数据，TXE = 0：发送数据寄存器还没有空
      发送数据前必须等待 TXE 为 1*/
    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_TXE) == RESET)
    {
        if(timeout -- ==0)                                                    //超时保护，没有超时保护时，如果 SPI 外设异常，程序可能永远卡在 while 里
        {
            return 0xFF;                                                      //FLASH擦除后的默认值通常为0xFF
        }
    }

    SPI_I2S_SendData(spi->SPIx, tx_data);                                     //把 tx_data 写入 SPI 数据寄存器，SPI 硬件随后会自动：产生 8 个 SCK 脉冲，把数据从 MOSI 发出去，同时从 MISO 接收 8 bit

    timeout = SPI_TIMEOUT;

    /*等待RXNE
      Receive buffer not empty 接收数据寄存器非空
      RXNE = 1：已经接收到完整的一个字节
      RXNE = 0：接收还没完成*/
    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_RXNE) == RESET)        
    {
        if(timeout -- ==0)
        {
            return 0xFF;
        }
    }

    return (uint8_t)SPI_I2S_ReceiveData(spi->SPIx);                        //把接收到的数据读出来，这一步还可以清除 RXNE 标志。因为 SPI 是全双工，即使只想发送，也会接收到一个字节。因此每次发送后都应该读取接收寄存器，避免接收溢出。
}


//发送一个字节，因为不需要接收数据，使用（void）主动丢弃返回值
void spi_send_byte(typedef_spi *spi, uint8_t data)
{
    (void) spi_transfer_byte (spi, data);
}

/*读取数据，同时发送0xFF
   SPI 的时钟只能由主机产生
   如果 STM32 不发送任何数据：就没有 SCK，从机也无法把数据移出来
   所以主机需要发送一个无意义的字节：0xFF*/
uint8_t spi_read_byte(typedef_spi *spi)
{
    return spi_transfer_byte (spi, 0xFF);
}

//连续发送数据
void spi_send_data( typedef_spi *spi, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        spi_send_byte(spi, data[i]);
    }
}

//连续发送数据
void spi_read_data(typedef_spi *spi, uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        data[i] = spi_read_byte(spi);
    }
}
```

# 3. W25Q128.h
``` c

/* 读取 JEDEC ID */
#define W25Q128_CMD_JEDEC_ID        0x9F

/* 读取状态寄存器 1 */
#define W25Q128_CMD_READ_STATUS1    0x05

/* 写使能 */
#define W25Q128_CMD_WRITE_ENABLE    0x06

/* 普通读取数据 */
#define W25Q128_CMD_READ_DATA       0x03

/* 页编程 */
#define W25Q128_CMD_PAGE_PROGRAM    0x02

/* 擦除 4KB 扇区 */
#define W25Q128_CMD_SECTOR_ERASE    0x20


#define W25Q128_STATUS_BUSY         0x01

#define W25Q128_PAGE_SIZE           256U
#define W25Q128_SECTOR_SIZE         4096U

extern typedef_spi spi1_flash1;

uint32_t w25q128_read_jedec_id(typedef_spi *spi);

uint8_t w25q128_read_status1(typedef_spi *spi);

void w25q128_write_enable(typedef_spi *spi);

uint8_t w25q128_wait_busy(typedef_spi *spi, uint32_t timeout);

void w25q128_read_data(typedef_spi *spi, uint32_t address, uint8_t *data, uint16_t len);

uint8_t w25q128_page_program(typedef_spi *spi, uint32_t address, const uint8_t *data, uint16_t len);

uint8_t w25q128_sector_erase(typedef_spi *spi, uint32_t address);

#endif
```

# 4. W25Q128.c
``` c
uint32_t w25q128_read_jedec_id(typedef_spi *spi)
{
    uint32_t id = 0;

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_JEDEC_ID);

    id |= ((uint32_t)spi_read_byte(spi) << 16);

    id |= ((uint32_t)spi_read_byte(spi) << 8);

    id |= ((uint32_t)spi_read_byte(spi));

    spi_cs_high(spi);


    return id;
}

uint8_t w25q128_read_status1(typedef_spi *spi)
{
    uint8_t status;

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_READ_STATUS1);

    status = spi_read_byte(spi);

    spi_cs_high(spi);

    return status;
}

void w25q128_write_enable(typedef_spi *spi)
{
    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_WRITE_ENABLE);

    spi_cs_high(spi);
}

uint8_t w25q128_wait_busy(typedef_spi *spi, uint32_t timeout)
{
    while (timeout-- > 0)
    {
        if ((w25q128_read_status1(spi) & W25Q128_STATUS_BUSY) == 0)
        {
            return 0;
        }
    }

    return 1;
    
}

void w25q128_read_data(typedef_spi *spi, uint32_t address, uint8_t *data, uint16_t len)
{
    uint16_t i;

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_READ_DATA);

    spi_send_byte(spi, (uint8_t)(address >> 16));

    spi_send_byte(spi, (uint8_t)(address >> 8));

    spi_send_byte(spi, (uint8_t)address);

    for (i = 0; i < len; i++)
    {
        data[i] = spi_read_byte(spi);
    }

    spi_cs_high(spi);
}

uint8_t w25q128_page_program(typedef_spi *spi, uint32_t address, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint16_t page_offset;

    if (len == 0 || len > W25Q128_PAGE_SIZE)
    {
        return 1;
    }

    page_offset = (uint16_t)(address & 0xFF);

    if (page_offset + len > W25Q128_PAGE_SIZE)
    {
        return 1;
    }

    w25q128_write_enable(spi);

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_PAGE_PROGRAM);

    spi_send_byte(spi, (uint8_t)(address >> 16));

    spi_send_byte(spi, (uint8_t)(address >> 8));

    spi_send_byte(spi, (uint8_t)address);

    for (i = 0; i < len; i++)
    {
        spi_send_byte(spi, data[i]);
    }

    spi_cs_high(spi);

    return w25q128_wait_busy(spi, 1000000U);
}

uint8_t w25q128_sector_erase(typedef_spi *spi, uint32_t address)
{
    address &= 0xFFF000UL;

    w25q128_write_enable(spi);

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_SECTOR_ERASE);

    spi_send_byte(spi, (uint8_t)(address >> 16));

    spi_send_byte(spi, (uint8_t)(address >> 8));

    spi_send_byte(spi, (uint8_t)address);

    spi_cs_high(spi);

    return w25q128_wait_busy(spi, 10000000U);
}
```

