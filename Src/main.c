/*

 */

#include <stdint.h>
#include <stm32g0b1xx.h>
#include "led.h"
#include "uart.h"
#include "stdlib.h"
#include "timebase.h"

void delay (void);

int main(void)
{
	LED_init();
	uart_2_tx_init(); 
	timebaseInit(); 

	char print_buffer[100] = "SystickExpired\n"; 
	uint32_t current_systick_value; 

	while (1)
	{

		if (isSystickExpired())
		{
			uart_2_put_string(print_buffer); 
		}

		current_systick_value = getCurrentSysTickValue(); 

	}
}




void delay (void)
{
	for (uint32_t i = 0; i < 65000; i++ )
	{
		__NOP();
	}
	for (uint32_t i = 0; i < 65000; i++ )
	{
		__NOP();
	}
	for (uint32_t i = 0; i < 65000; i++ )
	{
		__NOP();
	}
	for (uint32_t i = 0; i < 65000; i++ )
	{
		__NOP();
	}
	for (uint32_t i = 0; i < 65000; i++ )
	{
		__NOP();
	}
}

