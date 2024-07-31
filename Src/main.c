/*

 */

#include <stdint.h>
#include <stm32g0b1xx.h>
#include "osKernel.h"
#include "led.h"
#include "uart.h"
#include "stdlib.h"
#include "debugPins.h"
#include <stdbool.h>

#define TIME_QUANTA 10U

typedef uint32_t taskProfiler_t; 

taskProfiler_t task0_Profiler, task1_Profiler, task2_Profiler, task3_Profiler; 



void task0 (void)
{
	while (1)
	{
		task0_Profiler++;
		debugPinToggle(DEBUG_PIN_0);
		uart_2_put_string("Task0 running \n");
		osKernel_TaskYield();  

	}
}

void task1 (void)
{
	while (1)
	{
		task1_Profiler++;
		debugPinToggle(DEBUG_PIN_1);
		uart_2_put_string("Task1 running \n");

	}
}

void task2 (void)
{
	while (1)
	{
		task2_Profiler++;
		debugPinToggle(DEBUG_PIN_2);
		uart_2_put_string("Task2 running \n");
	}
}

void task3 (void)
{
	task3_Profiler++; 
	uart_2_put_string("Task3 - 100ms\n"); 
}

int main(void)
{
	/* Initialize device drivers */
	LED_init();
	uart_2_tx_init();
	debugPinsInit();
	
	/* Initialize Kernel. */
	osKernel_Init();

	/* Initialize Tasks*/
	osKernel_AddTasks(task0, task1, task2);

	/* Set RoundRobin time quanta. */ 
	osKernel_Launch(TIME_QUANTA); 


	while (1); 
}



