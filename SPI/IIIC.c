#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "iic.h"

typedef_iic iic1 =
{
    .GPIOx = GPIOB,
    .I2Cx = I2C1,

    .GPIO_Clk = RCC_AHB1Periph_GPIOB,
    .iic_Clk = RCC_APB1Periph_I2C1,

    .SCL_Pin = GPIO_Pin_6,
    .SDA_Pin = GPIO_Pin_7,
};

static void iic_delay(void)
{
    volatile uint32_t i;

    for (i = 0; i < 50; i++)
    {
        ;
    }
}

static void eeprom_write_delay(void)
{
    volatile uint32_t i;

    for (i = 0; i < 500000; i++)
    {
        ;
    }
}

static void iic_scl_high(typedef_iic* iic)
{
    GPIO_SetBits(iic->GPIOx, iic->SCL_Pin);
}

static void iic_scl_low(typedef_iic* iic)
{
    GPIO_ResetBits(iic->GPIOx, iic->SCL_Pin);
}

static void iic_sda_high(typedef_iic* iic)
{
    GPIO_SetBits(iic->GPIOx, iic->SDA_Pin);
}

static void iic_sda_low(typedef_iic* iic)
{
    GPIO_ResetBits(iic->GPIOx, iic->SDA_Pin);
}

static uint8_t iic_read_sda(typedef_iic* iic)
{
    return GPIO_ReadInputDataBit(iic->GPIOx, iic->SDA_Pin);
}

void iic_init(typedef_iic* iic)
{
    RCC_AHB1PeriphClockCmd(iic->GPIO_Clk,ENABLE);
    RCC_APB1PeriphClockCmd(iic->iic_Clk,ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Pin = iic->SCL_Pin | iic->SDA_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(iic->GPIOx, &GPIO_InitStruct);

    iic_scl_high(iic);
    iic_sda_high(iic);
}

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

void iic_send_byte(typedef_iic* iic, uint8_t data)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        iic_scl_low(iic);

        if (data & 0x80)
        {
            iic_sda_high(iic);
        }
        else
        {
            iic_sda_low(iic);
        }

        data <<= 1;

        iic_delay();

        iic_scl_high(iic);
        iic_delay();

        iic_scl_low(iic);
        iic_delay();
    }
}

uint8_t iic_wait_ack(typedef_iic * iic)
{
    uint16_t timeout = 0;

    iic_sda_high(iic);
    iic_delay();

    iic_scl_high(iic);
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

uint8_t iic_read_byte(typedef_iic* iic, uint8_t ack)
{
    uint8_t i;
    uint8_t data = 0;

    iic_sda_high(iic);

    for(i = 0; i < 8; i++)
    {
        data <<= 1;

        iic_scl_low(iic);
        iic_delay();

        iic_scl_high(iic);
        iic_delay();

        if (iic_read_sda(iic))
        {
            data |= 0x01;
        }

        iic_scl_low(iic);
        iic_delay();
    }

    if (ack)
    {
        iic_ack(iic);
    }
    else
    {
        iic_nack(iic);
    }

    return data;
}


uint8_t bl24c512_write_byte (typedef_iic *iic, uint16_t mem_addr, uint8_t data)
{
    iic_start(iic);

    /* 发送设备地址 + 写方向 */
    iic_send_byte(iic, BL24C512_ADDR << 1);
    if (iic_wait_ack(iic))
    {
        return 1;
    }

    /* 发送存储地址高 8 位 */
    iic_send_byte(iic, (uint8_t)(mem_addr >> 8));
    if (iic_wait_ack(iic))
    {
        return 1;
    }

    /* 发送存储地址低 8 位 */
    iic_send_byte(iic, (uint8_t)(mem_addr & 0xFF));
    if (iic_wait_ack(iic))
    {
        return 1;
    }

    /* 发送要写入的数据 */
    iic_send_byte(iic, data);
    if (iic_wait_ack(iic))
    {
        return 1;
    }

    iic_stop(iic);

    /* EEPROM 写入后需要等待内部写周期 */
    eeprom_write_delay();

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
    iic_send_byte(iic, (uint8_t)(mem_addr >> 8));
    if (iic_wait_ack(iic))
    {
        return 0xFF;
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
