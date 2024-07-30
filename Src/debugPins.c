
#include "debugPins.h"
#include "stdbool.h"
#include <stm32g0b1xx.h>


void debugPinsInit(void)
{
    /* Enable clocks for used ports. */
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN; 

    /* Set used pins to output mode. */
    GPIOB->MODER &= ~(GPIO_MODER_MODE4_1 + GPIO_MODER_MODE5_1 + GPIO_MODER_MODE3_1);

}

void debugPinSet(uint32_t pin)
{
    GPIOB->ODR |= pin; 
}

void debugPinReset(uint32_t pin)
{
    GPIOB->BRR |= pin; 
}

void debugPinToggle(uint32_t pin)
{
    if ((GPIOB->ODR & pin))
    {
        GPIOB->BRR |= pin; 
    }
    else
    {
        GPIOB->ODR |= pin;
    }
}