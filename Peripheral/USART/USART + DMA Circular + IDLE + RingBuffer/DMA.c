#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "USART.h"
#include "DMA.h"
#include "RingBuffer.h"

extern USART_t usart1;

DMA_t dma1 =
{
    .DMAy_Streamx = DMA2_Stream2
};

void dma_init(DMA_t * dma1)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    
    DMA_InitTypeDef DMA_InitStruct;

    DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_BufferSize = DMABufferSize;
    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;

    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)dma1->DMABuffer;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) &usart1.USARTx->DR;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    
    DMA_Init(dma1->DMAy_Streamx, &DMA_InitStruct);
    DMA_Cmd(dma1->DMAy_Streamx, ENABLE);
}
