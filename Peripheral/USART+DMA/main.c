#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "USART.h"
#include "RingBuffer.h"
#include "DMA.h"

extern USART_t usart1;
extern DMA_t dma1;

extern volatile bool dma_done;

int main(void)
{
	usart_init(&usart1);
	dma_init(&dma1);

    while(1)
    {
        if(dma_done)
        {
			dma_done = false;
            for(int i = 0; i < 8; i++)
            {
                usart_sendbyte(&usart1, dma1.dma_buffer[i]);
            }
        }
    }
}

