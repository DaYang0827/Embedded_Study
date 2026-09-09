#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include "USART.h"
#include "DMA.h"
#include "RingBuffer.h"

extern DMA_t dma1;
extern RingBuffer_t rb;

USART_t usart1 =
{
    .RCC_GPIO = RCC_AHB1Periph_GPIOA,
    .RCC_USART = RCC_APB2Periph_USART1,
    
    .GPIOx = GPIOA,
    .USARTx = USART1,
    
    .GPIO_PinSource_RX = GPIO_PinSource10,
    .GPIO_PinSource_TX = GPIO_PinSource9,

    .USART_IDLE_FLAG = USART_FLAG_IDLE,
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

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(usart1->USARTx, USART_IT_IDLE, ENABLE);
}

void usart_sendbyte(USART_t* usart1, uint8_t data)
{
    USART_SendData(usart1->USARTx, data);
	while(USART_GetFlagStatus(usart1->USARTx, USART_FLAG_TXE) == RESET);
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(usart1.USARTx, USART_IT_IDLE) == SET)
    {
        volatile uint32_t temp;

        DMA_Cmd(dma1.DMAy_Streamx, DISABLE);
        while (DMA_GetCmdStatus(dma1.DMAy_Streamx) == ENABLE);

        temp = USART1->SR;
        temp = USART1->DR;
        (void)temp;

        uint16_t rx_len = DmaBufferSize - DMA_GetCurrDataCounter(dma1.DMAy_Streamx);

        for(int i = 0; i < rx_len; i++)
        {
            rb_write(&rb, dma1.dma_buffer[i]);
        }

        DMA_ClearFlag(dma1.DMAy_Streamx, dma1.DMA_FLAG);
        DMA_SetCurrDataCounter(dma1.DMAy_Streamx, DmaBufferSize); 
        DMA_Cmd(dma1.DMAy_Streamx, ENABLE);
    }
}
