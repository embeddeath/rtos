/*
 * uart.c
 *
 *  Created on: Apr 9, 2024
 *      Author: marqu
 */




#include "uart.h"
#include <stm32g0b1xx.h>
#include <stddef.h>
#include <stdbool.h>


static uint32_t uart_compute_brr (uint32_t periph_clock, uint32_t baudrate); 
static void uart2_set_baudrate (uint32_t baudrate); 

#define SYS_FREQ		16000000
#define APB_CLK		    SYS_FREQ
#define UART_2_BAUDRATE	250000

/* This function initializes Uart2
    Uart2 is connected to the embedded debugger on the Nucleo Board
    through pins PA2 and PA3
*/

void uart_2_tx_init (void)
{
    /* Enable clock for Port A*/
    RCC -> IOPENR |= RCC_IOPENR_GPIOAEN;

    /* Set PA2 Mode to Alternate Mode*/
    GPIOA -> MODER &= ~(GPIO_MODER_MODE2_0); 
    GPIOA -> MODER |= (GPIO_MODER_MODE2_1); 

    GPIOA -> AFR[0] |= (GPIO_AFRL_AFSEL2_0); 
    GPIOA -> AFR[0] &= ~(GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_3); 

    /* Set PA3 to Mode Alternate Mode*/
    GPIOA -> MODER &= ~(GPIO_MODER_MODE3_0);
    GPIOA -> MODER |= (GPIO_MODER_MODE3_1); 

    GPIOA -> AFR[0] |= (GPIO_AFRL_AFSEL3_1); 
    GPIOA -> AFR[0] &= ~(GPIO_AFRL_AFSEL3_0| GPIO_AFRL_AFSEL3_2| GPIO_AFRL_AFSEL3_3); 

    /* Enable Clock for USART2*/
    RCC -> APBENR1 |= RCC_APBSMENR1_USART2SMEN; 

    /* Configure UART, 1 Start Bit, 8 Data Bits n Stop Bits*/
    USART2 -> CR1 &= ~(USART_CR1_M0 | USART_CR1_M1); 

    /* Configure Stop bits (1 Stop Bit)*/
    USART2 -> CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1); 

    /* Set Oversampling mode by 16 */
    USART2 -> CR1 &= ~USART_CR1_OVER8; 

    
    uart2_set_baudrate(UART_2_BAUDRATE);  

    USART2 -> CR1 |=  (USART_CR1_UE); 
    /* Enable Transmitter and Global Enable*/
    USART2 -> CR1 |=  (USART_CR1_TE); 


}


void uart_2_char_tx (uint8_t character_to_send)
{
    /* Wait until previous transmission is complete*/
    while (false == (USART2 -> ISR  & USART_ISR_TC) ); 
   
    USART2 -> TDR = character_to_send; 
 
}


void uart_2_put_string(char *string_ptr)
{
    int string_index = 0; 

    while(string_ptr[string_index] != '\0')
    {

        uart_2_char_tx(string_ptr[string_index]);
        string_index++; 
    }

}

static uint32_t uart_compute_brr (uint32_t periph_clock, uint32_t baudrate)
{
    /*
    Uart2 BaudRate Calculation 
                        periph_clock  
    USARTDIV(BRR)   =  ---------------------------
                        baudrate
    */

    return (periph_clock / baudrate); 
} 

static void uart2_set_baudrate (uint32_t baudrate)
{
    USART2 -> BRR = uart_compute_brr(APB_CLK ,baudrate); 
}