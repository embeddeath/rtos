#pragma once
#include <stdint.h>
#include <stm32g0b1xx.h>

extern uint8_t osKernel_AddTasks( void(*task0)(void), void(*task1)(void), void(*task2)(void) ); 
extern void osKernel_Init (void); 
extern void osKernel_Launch(uint32_t quanta); 
extern void osKernel_TaskYield(void);