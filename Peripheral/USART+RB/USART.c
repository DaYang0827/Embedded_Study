#include "stm32f4xx.h"
#include "USART.h"
#include "RingBuffer.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

extern RingBuffer_t rb;

USART_t usart1 = 
{
    .RCC_GPIO = RCC_AHB1Periph_GPIOA,
    .RCC_USART = RCC_APB2Periph_USART1,

    .GPIOx = GPIOA,
    .GPIO_Pin_rx = GPIO_Pin_10,
    .GPIO_Pin_tx = GPIO_Pin_9,
    .GPIO_PinSource_rx = GPIO_PinSource10,
    .GPIO_PinSource_tx = GPIO_PinSource9,
    .GPIO_AF = GPIO_AF_USART1,

    .USARTx = USART1,
};

void gpio_init(USART_t * usart1)
{
    RCC_AHB1PeriphClockCmd(usart1->RCC_GPIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = usart1->GPIO_Pin_rx | usart1->GPIO_Pin_tx;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Medium_Speed;

    GPIO_Init(usart1->GPIOx, &GPIO_InitStruct);

    GPIO_PinAFConfig(usart1->GPIOx, usart1->GPIO_PinSource_rx,usart1->GPIO_AF);
    GPIO_PinAFConfig(usart1->GPIOx, usart1->GPIO_PinSource_tx,usart1->GPIO_AF);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

}

void usart_init(USART_t * usart1)
{
    RCC_APB2PeriphClockCmd(usart1->RCC_USART, ENABLE);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(usart1->USARTx, &USART_InitStruct);

    USART_Cmd(usart1->USARTx, ENABLE);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void USART_SendByte(USART_t * usart1, uint8_t data)
{
    while(USART_GetFlagStatus(usart1->USARTx,
                              USART_FLAG_TXE) == RESET)
    {
    }

    USART_SendData(usart1->USARTx, data);
}

void USART1_IRQHandler(void)
{
    uint8_t data = 0;

    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
    {
        data = USART_ReceiveData(USART1);
        rb_write(&rb, data);
    }
}

int fputc(int ch, FILE *f)
{
    USART_SendByte(&usart1, (uint8_t)ch);
    return ch;
}

