#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "USART.h"
#include "DMA.h"
#include "RingBuffer.h"

extern USART_t usart1;
extern RingBuffer_t rb;
extern DMA_t dma1;

int main(void)
{
	rb_init(&rb);
	dma_init(&dma1);
	usart_init(&usart1);
	
	uint8_t data;

	while(1)
	{
		if(rb_read(&rb, &data))
		{
			usart_sendbyte(&usart1, data);
		}
	}
}
