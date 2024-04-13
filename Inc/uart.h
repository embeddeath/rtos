/*
 * uart.h
 *
 *  Created on: Apr 9, 2024
 *      Author: marqu
 */

#pragma once
#include "stdint.h"


/* This function initializes Uart2
    Uart2 is connected to the embedded debugger on the Nucleo Board
    through pins PA2 and PA3
*/
extern void uart_2_tx_init(void);
extern void uart_2_char_tx (uint8_t character_to_send); 
extern void uart_2_put_string(char *string_ptr);
