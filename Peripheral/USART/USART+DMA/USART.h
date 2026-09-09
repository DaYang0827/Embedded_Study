#ifndef __USART_H__
#define __USART_H__

#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>

typedef struct 
{
   uint32_t RCC_GPIO;
   uint32_t RCC_USART;

   GPIO_TypeDef* GPIOx;
   USART_TypeDef* USARTx;

   uint16_t GPIO_PinSource_RX;
   uint16_t GPIO_PinSource_TX;

}USART_t;

void usart_init(USART_t* usart1);
void usart_sendbyte(USART_t* usart1, uint8_t data);

#endif
