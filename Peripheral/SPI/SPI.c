#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "SPI.h"

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
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = spi->MISO_Pin | spi->MOSI_Pin | spi->SCK_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi->GPIOx, &GPIO_InitStruct);

    GPIO_PinAFConfig(spi->GPIOx, spi->SCK_PinSource, spi->GPIO_AF);
    GPIO_PinAFConfig(spi->GPIOx, spi->MOSI_PinSource, spi->GPIO_AF);
    GPIO_PinAFConfig(spi->GPIOx, spi->MISO_PinSource, spi->GPIO_AF);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = spi->CS_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi->CS_GPIOx, &GPIO_InitStruct);

    GPIO_SetBits(spi->CS_GPIOx, spi->CS_Pin);

    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_BaudRatePrescaler = spi->BaudRatePrescaler;
    SPI_InitStruct.SPI_CPHA = spi->CPHA;
    SPI_InitStruct.SPI_CPOL = spi->CPOL;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(spi->SPIx, &SPI_InitStruct);

    SPI_NSSInternalSoftwareConfig(spi->SPIx, SPI_NSSInternalSoft_Set);

    SPI_Cmd(spi->SPIx, ENABLE);
}

void spi_cs_low(typedef_spi*spi)
{
    GPIO_ResetBits(spi->CS_GPIOx, spi->CS_Pin);
}

void spi_cs_high(typedef_spi *spi)
{
    uint32_t timeout = SPI_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_BSY) == SET)
    {
        if (timeout-- == 0)
        {
            break;
        }
    }

    GPIO_SetBits(spi->CS_GPIOx, spi->CS_Pin);
}


uint8_t spi_transfer_byte(typedef_spi *spi, uint8_t tx_data)
{
    uint32_t timeout = SPI_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_TXE) == RESET)
    {
        if(timeout -- ==0)
        {
            return 0xFF;
        }
    }

    SPI_I2S_SendData(spi->SPIx, tx_data);

    timeout = SPI_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if(timeout -- ==0)
        {
            return 0xFF;
        }
    }

    return (uint8_t)SPI_I2S_ReceiveData(spi->SPIx);
}

void spi_send_byte(typedef_spi *spi, uint8_t data)
{
    (void) spi_transfer_byte (spi, data);
}

uint8_t spi_read_byte(typedef_spi *spi)
{
    return spi_transfer_byte (spi, 0xFF);
}

void spi_send_data( typedef_spi *spi, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        spi_send_byte(spi, data[i]);
    }
}

void spi_read_data(typedef_spi *spi, uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        data[i] = spi_read_byte(spi);
    }
}

