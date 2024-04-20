/*

 */

#include <stdint.h>
#include <stm32g0b1xx.h>
#include "led.h"
#include "uart.h"
#include "stdlib.h"
#include "timebase.h"
#include <stdbool.h>

void delay (void);

int main(void)
{
	LED_init();
	uart_2_tx_init(); 
	timebaseInit(); 

	while (1)
	{
		LED_on();
		uart_2_put_string("Led ON\n"); 
		delay_ms(1000);  
		LED_off();
		uart_2_put_string("Led OFF\n");
		delay_ms(1000);  			
	}
}



