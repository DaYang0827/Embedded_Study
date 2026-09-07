#include "stm32f4xx.h"
#include "USART.h"
#include "IIC.h"

int main(void)
{
    uint8_t write_data = 0xA5;
    uint8_t read_data = 0;

    usart_init(&usart1);
    iic_init(&iic1);

    usart_send_string(&usart1, "IIC BL24C512 Test Start\r\n");

    bl24c512_write_byte(&iic1, 0x0010, write_data);

    read_data = bl24c512_read_byte(&iic1, 0x0010);

    if (read_data == write_data)
    {
        usart_send_string(&usart1, "BL24C512 Write/Read Success\r\n");
    }
    else
    {
        usart_send_string(&usart1, "BL24C512 Write/Read Failed\r\n");
    }

    while (1)
    {
    }
}
