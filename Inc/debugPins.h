#pragma once

#include <stdint.h>
#include <stm32g0b1xx.h>


#define DEBUG_PIN_0 GPIO_ODR_OD4
#define DEBUG_PIN_1 GPIO_ODR_OD5
#define DEBUG_PIN_2 GPIO_ODR_OD3

extern void debugPinsInit(void); 
extern void debugPinSet(uint32_t pin); 
extern void debugPinReset(uint32_t pin); 
extern void debugPinToggle(uint32_t pin); 