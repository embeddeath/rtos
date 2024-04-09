/*

 */

#include <stdint.h>
#include <stm32g0b1xx.h>
#include "led.h"

void delay (void);

int main(void)
{
	LED_init();
	volatile uint8_t test;  
	while (1)
	{

		test = 1; 
		LED_on();
		delay();
		LED_off();
		test = 0; 
		delay();
		delay();

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
