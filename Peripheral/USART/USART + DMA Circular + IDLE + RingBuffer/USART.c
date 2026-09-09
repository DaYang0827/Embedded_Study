#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "USART.h"
#include "DMA.h"
#include "RingBuffer.h"

volatile uint16_t old_pos = 0;
volatile uint16_t new_pos = 0;

extern RingBuffer_t rb;
extern DMA_t dma1;

USART_t usart1 =
{
    .RCC_AHB1Periph_GPIO = RCC_AHB1Periph_GPIOA,
    .RCC_APB2Periph_USART = RCC_APB2Periph_USART1,
    
    .GPIOx = GPIOA,
    .USARTx = USART1,

    .GPIO_Pin_RX = GPIO_Pin_10,
    .GPIO_Pin_TX = GPIO_Pin_9,
    .GPIO_PinSource_RX = GPIO_PinSource10,
    .GPIO_PinSource_TX = GPIO_PinSource9,

    .USART_IT = USART_IT_IDLE,
};

void usart_init(USART_t * usart1)
{
    RCC_AHB1PeriphClockCmd(usart1->RCC_AHB1Periph_GPIO, ENABLE);
    RCC_APB2PeriphClockCmd(usart1->RCC_APB2Periph_USART, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = usart1->GPIO_Pin_RX | usart1->GPIO_Pin_TX;
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
    USART_Cmd(usart1->USARTx,ENABLE);

    USART_DMACmd(usart1->USARTx, USART_DMAReq_Rx, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(usart1->USARTx, usart1->USART_IT, ENABLE);
}


void usart_sendbyte(USART_t* usart1, uint8_t data)
{
    while(USART_GetFlagStatus(usart1->USARTx,USART_FLAG_TXE) == RESET)
    {
    }
    USART_SendData(usart1->USARTx, data);
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(usart1.USARTx, usart1.USART_IT) == SET)
    {
        volatile uint32_t temp;

        temp = USART1->SR;
        temp = USART1->DR;
        (void)temp;

        new_pos = DMABufferSize - DMA_GetCurrDataCounter(dma1.DMAy_Streamx);
        
        if(new_pos > old_pos)
        {
            for(int i = old_pos; i < new_pos; i++)
            {
                rb_write(&rb, dma1.DMABuffer[i]);
            }
        }
        
        if(new_pos < old_pos)
        {
            for(int i = old_pos; i < DMABufferSize; i++)
            {
                rb_write(&rb, dma1.DMABuffer[i]);
            }
            for(int i = 0; i < new_pos; i++)
            {
                rb_write(&rb, dma1.DMABuffer[i]);
            }
        }

        old_pos = new_pos;
    }
}
