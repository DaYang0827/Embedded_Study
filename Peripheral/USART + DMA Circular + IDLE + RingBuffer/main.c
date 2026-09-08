#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "USART.h"
#include "RingBuffer.h"
#include "DMA.h"

extern USART_t usart1;
extern DMA_t dma1;
extern RingBuffer_t rb;

int main(void)
{
	rb_init(&rb);
	dma_init(&dma1);
	usart_init(&usart1);
    
    uint8_t data = 0;
	
    while(1)
    {
        if(rb_read(&rb, &data))
        {
            usart_sendbyte(&usart1, data);
        }
    }
}

