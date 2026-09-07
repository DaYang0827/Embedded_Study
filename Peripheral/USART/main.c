#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "USART.h"

int main(void)
{
	usart_init(&usart1);

	usart_send_string(&usart1, "USART1 Init Success\r\n");

	while(1)
	{
		usart1_process_data();
	}
}

