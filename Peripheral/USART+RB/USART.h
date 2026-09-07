#ifndef __USART_H__
#define __USART_H__
#include "stm32f4xx.h"
#include <stdint.h>

typedef struct 
{
    uint32_t RCC_GPIO;
    uint32_t RCC_USART;

    GPIO_TypeDef* GPIOx;
    USART_TypeDef* USARTx;

    uint32_t GPIO_Pin_rx;
    uint32_t GPIO_Pin_tx;
    uint16_t GPIO_PinSource_rx;
    uint16_t GPIO_PinSource_tx;
    uint8_t GPIO_AF;
}USART_t;


void gpio_init(USART_t * usart1);
void usart_init(USART_t * usart1);

//void USART1_IRQn(USART_t * usart1, RingBuffer* rb);
void USART_SendByte(USART_t * usart1, uint8_t data);

#endif
