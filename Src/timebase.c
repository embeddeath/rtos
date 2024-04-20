
#include "timebase.h"
#include <stm32g0b1xx.h>

/* Calculated HSI16 -> HSISYS -> SYSCLK -> AHB PRESC -> HCLK
    HSI16 = 16MHz
    To get a tick every 1ms : SYST_RVR = 1ms/(1/16MHz) = 16000
*/
#define TICK_FREQUENCY_MS 1
#define HSI16_FREQ 16000000
#define SYSTICK_RELOAD_VALUE (TICK_FREQUENCY_MS * (HSI16_FREQ / 1000))



static volatile uint32_t tick = 0; 

static void tickIncrement(void)
{
    tick++; 
}

void timebaseInit(void)
{
    /* Initialize SysTick in interrupt mode */
    SysTick -> CTRL |= (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk); 

    /* Set reload value so that an interrupt is requested every ms*/
    SysTick -> LOAD = SYSTICK_RELOAD_VALUE - 1;

    /* Enable SysTick Timer*/
    SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk; 

    __enable_irq();
} 

bool isSystickExpired(void)
{
    bool return_value = false; 

    if (SysTick -> CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    {
        return_value = true; 
    }

    return return_value; 
}

uint32_t getCurrentSysTickValue(void)
{
    return (SysTick -> VAL);
}

void SysTick_Handler (void)
{
    tickIncrement(); 
} 

uint32_t getTick(void)
{
    return tick; 
}

void delay_ms (uint32_t delay_ms)
{
    uint32_t start_tick = getTick(); 
    
    while (getTick() - start_tick <= delay_ms)
    {
        /* Do nothing. */
    }
}
