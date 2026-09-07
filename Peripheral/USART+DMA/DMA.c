#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "USART.h"
#include "DMA.h"

extern USART_t usart1;

volatile bool dma_done = false;

DMA_t dma1 = 
{
    .DMAy_Streamx = DMA2_Stream2,
    .DMA_IT = DMA_IT_TC,
    .DMA_FLAG = DMA_FLAG_TCIF2,
    .RCC_DMAx = RCC_AHB1Periph_DMA2,
};

void dma_init(DMA_t* dma1)
{
    RCC_AHB1PeriphClockCmd(dma1->RCC_DMAx, ENABLE);
    
    DMA_InitTypeDef DMA_InitStruct;

    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_BufferSize = DmaBufferSize;
    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;

    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)(dma1->dma_buffer);
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&usart1.USARTx->DR;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    
    DMA_Init(dma1->DMAy_Streamx, &DMA_InitStruct);

    DMA_Cmd(dma1->DMAy_Streamx,ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&NVIC_InitStruct);

    DMA_ITConfig(dma1->DMAy_Streamx, dma1->DMA_IT, ENABLE);
}

void DMA2_Stream2_IRQHandler(void)
{
    if(DMA_GetFlagStatus(dma1.DMAy_Streamx, dma1.DMA_FLAG))
    {
        DMA_ClearITPendingBit(dma1.DMAy_Streamx, dma1.DMA_FLAG);
        dma_done = true;
    }
}



