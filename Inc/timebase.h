#pragma once

#include <stdint.h>
#include <stdbool.h>

void timebaseInit(void); 
bool isSystickExpired(void); 
uint32_t getCurrentSysTickValue(void);
void delay_ms (uint32_t delay_ms); 
uint32_t getTick(void); 