#include "led.h"

void LED_init(void)
{

	/*Enable clock to led port (PORT A). */
	RCC -> IOPENR |= RCC_IOPENR_GPIOAEN;

	/*Set LED pin as output. */
	GPIOA -> MODER &= ~GPIO_MODER_MODE5_1;
	GPIOA -> MODER |= GPIO_MODER_MODE5_0;

}


void LED_on (void)
{
	/*Set LED pin HIGH*/
	GPIOA -> ODR |= GPIO_ODR_OD5;
}

void LED_off (void)
{
	/*Set LED pin LOW*/
	GPIOA -> ODR &= ~GPIO_ODR_OD5;
}
