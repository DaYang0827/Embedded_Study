#ifndef __USART_H__
#define __USART_H__

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define USART_RX_BUF_SIZE    128

typedef struct
{
    USART_TypeDef* USARTx;

    uint32_t USART_CLK;

    GPIO_TypeDef* GPIOx;
    uint32_t GPIO_CLK;

    uint16_t TX_Pin;
    uint16_t RX_Pin;

    uint8_t TX_PinSource;
    uint8_t RX_PinSource;

    uint8_t GPIO_AF;

    IRQn_Type IRQ_Channel;

    uint32_t BaudRate;
}typedef_usart;

extern volatile uint8_t usart1_rx_byte;
extern char usart1_rx_buf[USART_RX_BUF_SIZE];
extern volatile uint16_t usart1_rx_len;
extern volatile uint8_t usart1_rx_flag;

extern typedef_usart usart1;

void usart_init(typedef_usart * usart);

void usart_send_byte(typedef_usart * usart, uint8_t data);
void usart_send_string(typedef_usart * usart, char* str);
void usart_send_data(typedef_usart * usart,uint8_t * data, uint16_t len);

void usart1_process_data(void);
void usart1_clear_buffer(void);

#endif
