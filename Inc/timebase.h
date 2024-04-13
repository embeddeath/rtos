#pragma once

#include <stdint.h>
#include <stdbool.h>

void timebaseInit(void); 
bool isSystickExpired(void); 
uint32_t getCurrentSysTickValue(void);