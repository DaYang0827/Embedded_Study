#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include "USART.h"

USART_t usart1 =
{
    .RCC_GPIO = RCC_AHB1Periph_GPIOA,
    .RCC_USART = RCC_APB2Periph_USART1,
    
    .GPIOx = GPIOA,
    .USARTx = USART1,
    
    .GPIO_PinSource_RX = GPIO_PinSource10,
    .GPIO_PinSource_TX = GPIO_PinSource9,
};

void usart_init(USART_t* usart1)
{
    RCC_AHB1PeriphClockCmd(usart1->RCC_GPIO, ENABLE);
    RCC_APB2PeriphClockCmd(usart1->RCC_USART, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(usart1->GPIOx, &GPIO_InitStruct);

    GPIO_PinAFConfig(usart1->GPIOx, usart1->GPIO_PinSource_RX, GPIO_AF_USART1);
    GPIO_PinAFConfig(usart1->GPIOx, usart1->GPIO_PinSource_TX, GPIO_AF_USART1);
    
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(usart1->USARTx, &USART_InitStruct);

    USART_Cmd(usart1->USARTx, ENABLE);

    USART_DMACmd(usart1->USARTx, USART_DMAReq_Rx, ENABLE);
}

void usart_sendbyte(USART_t* usart1, uint8_t data)
{
    USART_SendData(usart1->USARTx, data);
	while(USART_GetFlagStatus(usart1->USARTx, USART_FLAG_TXE) == RESET);
}
