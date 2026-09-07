#ifndef __W25Q128_H__
#define __W25Q128_H__

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
