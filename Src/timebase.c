
#include "timebase.h"
#include <stm32g0b1xx.h>

/* Calculated HSI16 -> HSISYS -> SYSCLK -> AHB PRESC -> HCLK8
    HSI16 = 16MHzb
    
    HCLK/8 = 16Mhz/8 = 2MHz

    To get a tick every 1ms : SYST_RVR = 1ms/(1/2MHz) = 2000 = 0x7D0
*/
#define RELOAD_VALUE_TICK_1MS 8000-1

void timebaseInit(void)
{
    /* Initialize SysTick in interrupt mode */
    SysTick -> CTRL |= (/*SysTick_CTRL_TICKINT_Msk |*/ SysTick_CTRL_CLKSOURCE_Msk); 

    /* Set reload value so that an interrupt is requested every ms*/
    SysTick -> LOAD = RELOAD_VALUE_TICK_1MS;

    /* Calibrate SysTick */
    //SysTick -> CALIB |= (SysTick_CALIB_NOREF_Msk);

    /* Enable SysTick Timer*/
    SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk; 
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


