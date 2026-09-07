#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "W25Q128.h"
#include "SPI.h"

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
