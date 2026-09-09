#ifndef __RingBuffer_H__
#define __RingBuffer_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define buffersize 128

typedef struct 
{
    uint8_t RingBuffer[buffersize];
    volatile uint16_t read;
    volatile uint16_t write;
}RingBuffer_t;

void rb_init(RingBuffer_t* rb);
bool rb_isempty(RingBuffer_t* rb);
bool rb_isfull(RingBuffer_t* rb);

bool rb_write(RingBuffer_t* rb, uint8_t data);
bool rb_read(RingBuffer_t* rb, uint8_t* data);

#endif
