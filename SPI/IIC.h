#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

#endif
