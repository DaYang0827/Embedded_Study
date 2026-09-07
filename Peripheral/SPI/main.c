#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "USART.h"
#include "W25Q128.h"
#include "SPI.h"
#include "LED.h"

extern typedef_spi spi1_flash1;

int main(void)
{
    uint32_t jedec_id;

    usart_init(&usart1);
    
    usart_send_string(&usart1, "\r\n[BOOT] USART1 ready\r\n");

    spi_init(&spi1_flash1);
    usart_send_string(&usart1, "[BOOT] SPI1 init done\r\n");

    jedec_id = w25q128_read_jedec_id(&spi1_flash1);

    printf("[BOOT] JEDEC ID = %02X %02X %02X\r\n",
           (uint8_t)(jedec_id >> 16),
           (uint8_t)(jedec_id >> 8),
           (uint8_t)jedec_id);

    while (1)
    {
        
    }
}
