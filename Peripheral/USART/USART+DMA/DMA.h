#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>

#define DmaBufferSize 8

typedef struct  
{
    uint8_t dma_buffer[DmaBufferSize];
    uint32_t RCC_DMAx;
    DMA_Stream_TypeDef* DMAy_Streamx;

    uint32_t DMA_IT;
    uint32_t DMA_FLAG;
}DMA_t;


void dma_init(DMA_t* dma1);

#endif
