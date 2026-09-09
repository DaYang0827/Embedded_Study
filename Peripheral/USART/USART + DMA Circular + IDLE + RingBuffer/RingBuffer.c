#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "USART.h"
#include "DMA.h"
#include "RingBuffer.h"

RingBuffer_t rb;

void rb_init(RingBuffer_t *rb)
{
    rb->read = 0;
    rb->write = 0;
}

bool rb_isfull(RingBuffer_t *rb)
{
    if(((rb->write + 1U) % BufferSize)== rb->read)
    return true;
    else
    return false;
}
bool rb_isempty(RingBuffer_t *rb)
{
    if(rb->read == rb->write)
    return true;
    else
    return false;
}

bool rb_write(RingBuffer_t *rb, uint8_t data)
{
    if(rb_isfull(rb))
    return false;
    else
    {
        rb->RingBuffer[rb->write] = data;
        rb->write = (rb->write + 1U) % BufferSize;
        return true;
    }
}

bool rb_read(RingBuffer_t *rb, uint8_t *data)
{
    if(rb_isempty(rb))
    return false;
    else
    {
        *data = rb->RingBuffer[rb->read];
        rb->read = (rb->read + 1U) % BufferSize;
        return true;
    }
}
