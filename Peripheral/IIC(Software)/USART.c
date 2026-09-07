#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "USART.h"

typedef_usart usart1 = 
{
    .USARTx = USART1,
    .USART_CLK = RCC_APB2Periph_USART1,

    .GPIOx = GPIOA,
    .GPIO_CLK = RCC_AHB1Periph_GPIOA,

    .TX_Pin = GPIO_Pin_9,
    .RX_Pin = GPIO_Pin_10,

    .TX_PinSource = GPIO_PinSource9,
    .RX_PinSource = GPIO_PinSource10,

    .GPIO_AF = GPIO_AF_USART1,

    .IRQ_Channel = USART1_IRQn,

    .BaudRate = 115200
};

volatile uint8_t usart1_rx_byte = 0;

char usart1_rx_buf[USART_RX_BUF_SIZE] = {0};

volatile uint16_t usart1_rx_len = 0;

volatile uint8_t usart1_rx_flag = 0;

void usart_init (typedef_usart * usart)
{
    RCC_AHB1PeriphClockCmd(usart->GPIO_CLK, ENABLE);
    if (usart->USARTx == USART1 || usart->USARTx == USART6)
    {
        RCC_APB2PeriphClockCmd(usart->USART_CLK, ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(usart->USART_CLK, ENABLE);
    }

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Pin = usart->TX_Pin | usart->RX_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(usart->GPIOx, &GPIO_InitStruct);

    GPIO_PinAFConfig(usart->GPIOx, usart->RX_PinSource, usart->GPIO_AF);
    GPIO_PinAFConfig(usart->GPIOx, usart->TX_PinSource, usart->GPIO_AF);
    
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = usart->BaudRate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;

    USART_Init(usart->USARTx, &USART_InitStruct);

    USART_ITConfig(usart->USARTx, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = usart->IRQ_Channel;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(usart->USARTx, ENABLE);
}

void usart_send_byte(typedef_usart * usart, uint8_t data)
{
    uint32_t timeout = 1000000;

    while (USART_GetFlagStatus(usart->USARTx, USART_FLAG_TXE) == RESET)
    {
        if (timeout-- == 0)
        {
            return;
        }
    }

    USART_SendData(usart->USARTx, data);
}

void usart_send_string(typedef_usart * usart, char* str)
{
    while(*str != '\0')
    {
        usart_send_byte(usart, *str);
        str++;
    }
}

void usart_send_data(typedef_usart * usart, uint8_t * data, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i ++)
    {
        usart_send_byte(usart, data[i]);
    }
}

void usart1_clear_buffer(void)
{
    uint16_t i;

    for (i = 0; i < USART_RX_BUF_SIZE; i++)
    {
        usart1_rx_buf[i] = 0;
    }

    usart1_rx_len = 0;
    usart1_rx_flag = 0;
}

void usart1_process_data(void)
{
    if (usart1_rx_flag == 1)
    {
        usart_send_string(&usart1, "Received: ");
        usart_send_string(&usart1, usart1_rx_buf);
        usart_send_string(&usart1, "\r\n");
        usart1_clear_buffer();
    }
}

void USART1_IRQHandler(void)
{
    uint8_t data;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);

        usart1_rx_byte = data;

        if(data == '\r' || data == '\n')
        {
            if (usart1_rx_len > 0)
            {
                usart1_rx_buf[usart1_rx_len] = '\0';
                usart1_rx_flag = 1;
            }
        }
        else if(usart1_rx_flag == 0)
        {
            if (usart1_rx_len < USART_RX_BUF_SIZE - 1 )
            {
                usart1_rx_buf[usart1_rx_len] = data;
                usart1_rx_len++;
            }
            else
            {
                usart1_clear_buffer();
            }
        }
    }
}

int fputc(int ch, FILE *f)
{
    usart_send_byte(&usart1, (uint8_t)ch);
    return ch;
}


