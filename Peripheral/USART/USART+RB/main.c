#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "USART.h"
#include "RingBuffer.h"

extern USART_t usart1;
extern RingBuffer_t rb;

int main(void)
{
    rb_init(&rb);

    gpio_init(&usart1);
	usart_init(&usart1);
	
    while(1)
    {
        uint8_t data;

        if(rb_read(&rb, &data))
        {
            USART_SendByte(&usart1, data);
        }
    }
}
