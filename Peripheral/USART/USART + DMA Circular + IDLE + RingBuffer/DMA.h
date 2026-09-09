#ifndef __DMA_H__
#define __DMA_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

#define DMABufferSize 8

typedef struct 
{
   uint8_t DMABuffer[DMABufferSize];
   DMA_Stream_TypeDef *DMAy_Streamx;

}DMA_t;

void dma_init(DMA_t * dma1);


#endif
