#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define SPI_TIMEOUT                     1000000U

typedef struct  
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

    GPIO_TypeDef * CS_GPIOx;
    uint32_t CS_GPIO_CLK;
    uint16_t CS_Pin;

    uint16_t BaudRatePrescaler;
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

