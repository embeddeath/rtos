
#include "osKernel.h"
#include "stdbool.h"

typedef enum 
{
    TASK_0_INDEX = 0,  
    TASK_1_INDEX, 
    TASK_2_INDEX,
    MAX_NUM_OF_TASKS,
}task_index_t; 

#define     STACKSIZE                       100


/* Os Stackframe does not necessarily need to be in the same order that the AAPCS specifies

    AAPCS Stack: 
        _________________________
        |         xPSR          |   Program status register
        |_______________________|
        |          PC           |   Current instruction
        |_______________________|
        |          LR           |   Return addresss
        |_______________________|
        |          R12          |   Intra procedure-call scratch register
        |_______________________|
        |          R3           |   
        |_______________________|
        |          R2           |   
        |_______________________|
        |          R1           |   
        |_______________________|
        |          R0           |   
        |_______________________|

    OS Stack: 

        _________________________
        |         xPSR          |   Program status register
        |_______________________|
        |          PC           |   Current instruction
        |_______________________|
        |          LR           |   Return addresss
        |_______________________|
        |          R12          |   Intra procedure-call scratch register
        |_______________________|
        |          R3           |   
        |_______________________|
        |          R2           |   
        |_______________________|
        |          R1           |   
        |_______________________|
        |          R0           |   
        |_______________________|
        |          R11          |   
        |_______________________|
        |          R10          |   
        |_______________________|
        |          R9           |   
        |_______________________|
        |          R8           |   
        |_______________________|
        |          R7           |   
        |_______________________|
        |          R6           |   
        |_______________________|
        |          R5           |   
        |_______________________|
        |          R4           |   
        |_______________________|


*/
/* Os Stackframe offset definitions. */
#define     STACKFRAME_xPSR_OFFSET          STACKSIZE -1
#define     STACKFRAME_PC_OFFSET            STACKSIZE -2
#define     STACKFRAME_LR_OFFSET            STACKSIZE -3
/*#define     STACKFRAME_SP_OFFSET            STACKSIZE -4  Stack pointer is saved in TCB*/
#define     STACKFRAME_R12_OFFSET           STACKSIZE -4 
#define     STACKFRAME_R3_OFFSET            STACKSIZE -5
#define     STACKFRAME_R2_OFFSET            STACKSIZE -6
#define     STACKFRAME_R1_OFFSET            STACKSIZE -7
#define     STACKFRAME_R0_OFFSET            STACKSIZE -8
#define     STACKFRAME_R11_OFFSET           STACKSIZE -9
#define     STACKFRAME_R10_OFFSET           STACKSIZE -10
#define     STACKFRAME_R9_OFFSET            STACKSIZE -11
#define     STACKFRAME_R8_OFFSET            STACKSIZE -12
#define     STACKFRAME_R7_OFFSET            STACKSIZE -13
#define     STACKFRAME_R6_OFFSET            STACKSIZE -14
#define     STACKFRAME_R5_OFFSET            STACKSIZE -15
#define     STACKFRAME_R4_OFFSET            STACKSIZE -16

#define     STACKFRAME_DUMMY_INIT_VAL       0xDEADBEEF
/* Used to init top of the stack for debugging purposes. */
#define     STACKFRAME_TOP_DUMMY_INIT_VAL   0xCAFEBABE

/* xPSR bit definitions */
#define xPSR_THUMB_BIT_POS                  24
#define xPSR_THUMB_BIT_MASK                 (1U << xPSR_THUMB_BIT_POS)


/* Task control block. */
typedef struct tcb
{
    int32_t *stackPointer; 
    struct tcb *nextTcbPointer; 
} tcb_t; 

tcb_t tcbs[MAX_NUM_OF_TASKS]; 
tcb_t *currentTcbPointer; 

int32_t tcbs_stack[MAX_NUM_OF_TASKS][STACKSIZE]; 

/* Timing related variables and definitions. */
#define HSI16_FREQ 16000000
#define BUS_FREQUENCY HSI16_FREQ

#define PERIODIC_TASK_PERIOD_MS 100U
uint32_t periodic_task_tick; 
uint32_t MILLIS_PRESCALER; 
uint32_t osKernel_quanta; 
bool taskYield_flag; 

void osKernel_StackInit(task_index_t index); 
void osKernel_SchedulerLaunch(void); 
void osKernel_RoundRobinScheduler(void);

/* 
    Function Name: osKernel_StackInit
    Arguments: index of the task for which stack initialization is needed.
    Return type: void
    Description: 
    - This function initializes stack for a given task by performing the following actions:  
        - Setting thumb bit within PSR to force UC to execute instructions in thumb mode. 
        - Initializing the stack pointer within the task control block to point to the stack pointer within the tcb stack. 
        - Initialize rest of the core processor registers to dummy values for debugging purposes. 

        Stack frame, refer to AAPCS for more information. 
        _________________________
        |         xPSR          |   Program status register
        |_______________________|
        |          PC           |   Current instruction
        |_______________________|
        |          LR           |   Return addresss
        |_______________________|
        |          R12          |   Intra procedure-call scratch register
        |_______________________|
        |          R3           |   
        |_______________________|
        |          R2           |   
        |_______________________|
        |          R1           |   
        |_______________________|
        |          R0           |   
        |_______________________|

    Caveats: 
        - This function does not initialize the program counter. 
        - This function should only be called by osKernel_AddThreads. 
    
*/

void osKernel_StackInit(task_index_t index)
{
    /*  Set the Thumb bit within EPSR register for UC to work in thumb mode*/
    tcbs_stack[index][STACKFRAME_xPSR_OFFSET] = xPSR_THUMB_BIT_MASK;  

    /* Program counter is initialized in osKernel_AddThreads() */  

    /* Initialize stack pointer within the thread control block 
    (Make it point to the top of the stack):  STACKFRAME_R4_OFFSET*/
    tcbs[index].stackPointer = &tcbs_stack[index][STACKFRAME_R4_OFFSET]; 

    /* Registers that need to be preserved during a context switch. */
    tcbs_stack[index][STACKFRAME_LR_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R12_OFFSET] = STACKFRAME_DUMMY_INIT_VAL; 
    tcbs_stack[index][STACKFRAME_R3_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R2_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R1_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R0_OFFSET] = STACKFRAME_DUMMY_INIT_VAL; 

    /* Dummy register initialization, this are not needed for context switch but are here for debugging purposes*/
    tcbs_stack[index][STACKFRAME_R11_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R10_OFFSET] = STACKFRAME_DUMMY_INIT_VAL; 
    tcbs_stack[index][STACKFRAME_R9_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R8_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R7_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R6_OFFSET] = STACKFRAME_DUMMY_INIT_VAL; 
    tcbs_stack[index][STACKFRAME_R5_OFFSET] = STACKFRAME_DUMMY_INIT_VAL;  
    tcbs_stack[index][STACKFRAME_R4_OFFSET] = STACKFRAME_TOP_DUMMY_INIT_VAL; 

}


/* 
    Function Name: osKernel_AddTasks
    Arguments: Function pointers for the task functions 0 1 and 2.
    Return type: uint8. 
    Description: This function sets up the tasks by performing the following actions: 
        - Setting nextTcbPointer within the tcb of each task to point to the next task: 
                Task0 -> Task1 -> Task2 -> Task0
        - Initialize stack for each one of the tasks. 
        - Initialize program counter for each one of the tasks (pointing to the task functions). 
        - Set the current task control block pointer to the first task's control block. 
    Caveats:
        - None 
    
*/
uint8_t osKernel_AddTasks( void(*task0)(void), void(*task1)(void), void(*task2)(void) )
{
    /* Enter critical section */

    __disable_irq();

    /* Round robin scheduler, task0 links to task1, etc.*/
    tcbs[TASK_0_INDEX].nextTcbPointer = &(tcbs[TASK_1_INDEX]); 
    tcbs[TASK_1_INDEX].nextTcbPointer = &(tcbs[TASK_2_INDEX]); 
    tcbs[TASK_2_INDEX].nextTcbPointer = &(tcbs[TASK_0_INDEX]); 

    /* Initialize stack: */
    osKernel_StackInit(TASK_0_INDEX); 
    /* Initialize program counter to point towards task function. */
    tcbs_stack[TASK_0_INDEX][STACKFRAME_PC_OFFSET] = (int32_t)(task0); 

    /* Initialize stack: */
    osKernel_StackInit(TASK_1_INDEX); 
    /* Initialize program counter to point towards task function. */
    tcbs_stack[TASK_1_INDEX][STACKFRAME_PC_OFFSET] = (int32_t)(task1); 

    /* Initialize stack: */
    osKernel_StackInit(TASK_2_INDEX); 
    /* Initialize program counter to point towards task function. */
    tcbs_stack[TASK_2_INDEX][STACKFRAME_PC_OFFSET] = (int32_t)(task2); 

    /* Start from thread 0 */
    currentTcbPointer = &tcbs[TASK_0_INDEX]; 


    /* Exit critical section */
    __enable_irq();
    return 0; 
}

void osKernel_Init (void)
{
    /* 
        Conversion factor:

        16,000,000 cycles/sec
    -----------------------------   = 16,000 cycles/ms 
            1000 ms/sec  

    */
    MILLIS_PRESCALER = (BUS_FREQUENCY/1000); 
}

void osKernel_Launch(uint32_t quanta)
{
    /* Reset SysTick*/
    SysTick->CTRL = 0; 

    /* Clear SysTick Current Value Register*/
    SysTick->VAL = 0; 

    /* Load quanta, substracting one because timer starts counting from zero.*/
    osKernel_quanta = quanta; 
    SysTick->LOAD = (quanta * MILLIS_PRESCALER) - 1; 

    /* Set systick interrupt to low priority */
    NVIC_SetPriority(SysTick_IRQn, 15); 

    /* Enable SysTick, select internal clock source*/
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk); 
    
    /* Enable SysTick interrupt. */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; 

    /* Launch Scheduler */
    osKernel_SchedulerLaunch(); 
}


/* When an exception occurs, theser registers {r0, r1, r2, r3, r12, lr, pc} 
are automatically saved into the stack.*/
__attribute__((naked)) void SysTick_Handler (void)
{

  /* 
    The __attribute__((naked)) attribute in C and C++ is used to declare a function 
    without prologue/epilogue sequences, which are the code sequences that set up and 
    tear down the stack frame for a function. This attribute is typically used in  
    performance-critical code, where precise control over the function's execution context is required.
  */  

    /*  Enter critical section using inline assembly. 
        CPSID causes interrupts to be disabled by setting PRIMASK.
        The PRIMASK register prevents activation of all exceptions with configurable priority.
    */
    __asm("CPSID   I"); 

    /* Suspend current thread. */

    /*  Save R4-R11:
    -----------------------------------------------------------------------------------------------------------
            In Cortex-M0+, Low registers (R0-R7) can be accessed through directly through the 16-bit thumb
            instruction set (PUSH and POP are 16-bit thumb instructions).  
    */
    __asm("PUSH {R4, R5, R6, R7}");  

    /*  In Cortex-M0+, The high registers cannot be accessed directly through 16-bit thumb instructions, 
        therefore we need to MOV the data from the high registers into low registers before pushing them
        to the stack 
    */
    __asm("MOV R4, R8"); 
    __asm("PUSH {R4}"); 
    __asm("MOV R4, R9"); 
    __asm("PUSH {R4}");
    __asm("MOV R4, R10"); 
    __asm("PUSH {R4}");
    __asm("MOV R4, R11"); 
    __asm("PUSH {R4}"); 

    /* -----------------------------------------------------------------------------------------------------------*/


    /*  Load address of currentTcbPointer into R0. When using LDR with the '=' operator,
    you save the address of a label to a register. */
    __asm("LDR R0, =currentTcbPointer"); 

    /*  Load R1 from address equals R0, i.e r1 ) currentTcbPointer.
        LDR Rx, [Rn]
        When using the LDR instruction with "[Rn]", you are indicating that the processor should load the 
        value forom the memory address contained in Rn into Rx.  
    */
    __asm("LDR R1,[R0]"); 

    /* Store Cortex-M SP at address equals r1, i.e Save SP into TCB*/
    __asm("MOV R4, SP");
    __asm("STR R4,[R1]"); 

    /* Choose next Thread*/
    /* Not a good way to time periodic tasks with systick since it is modified by the taskYield function
    and might not give exact time. */
    __asm("PUSH {R0,LR}");
    __asm("BL osKernel_RoundRobinScheduler"); 
  
    /* Restore R0 */
    __asm("POP {R0}");

    /* Pop LR data*/
    __asm("POP {R1}");
    __asm("MOV LR, R1"); 

    __asm("LDR R1, [R0]"); 

    /* Load Cortex-M SP(R13) from address equals R1, i.e SP = currentTcbPointer->stackPointer*/
    __asm("LDR R4,[R1]");
    __asm("MOV SP,R4");


    /*-----------------------------------------------------------------------------------------------------------
    Restore R11-R4 (High registers cannot be directly accessed by the POP instruction so manipulating the stack pointer
    manually is necessary. )

    Restore R11 through R8 by loading the value pointed by the STACK pointer and incrementing stack
    pointer by 4 bytes to point to the next register (Stack grows downwards in memory)
    */
    __asm("LDR R4, [SP]");
    __asm("MOV R11, R4"); 
    __asm("ADD SP, SP, #4"); 
    __asm("LDR R4, [SP]");
    __asm("MOV R10, R4");
    __asm("ADD SP, SP, #4"); 
    __asm("LDR R4, [SP]");
    __asm("MOV R9, R4"); 
    __asm("ADD SP, SP, #4"); 
    __asm("LDR R4, [SP]");
    __asm("MOV R8, R4");
    __asm("ADD SP, SP, #4"); 
    __asm("LDR R4, [SP]");

    /* Low registers can be popped directly. */
    __asm("POP {R4-R7}");
    /* -----------------------------------------------------------------------------------------------------------*/

    /* Exit critical section. */
    __asm("CPSIE   I"); 
    
    /* Return from exception and restore r0, r1, r2, r3, r12, lr, pc, consider GNU assembly language course for the future.*/
    __asm("BX   LR");
    
} 

/* 
    Expand comment later on, this function loads the fist stask for execution, copying the tcb stack 
    to according registers. 
*/
void osKernel_SchedulerLaunch (void)
{

    /*  Enter critical section using inline assembly. 
        CPSID causes interrupts to be disabled by setting PRIMASK.
        The PRIMASK register prevents activation of all exceptions with configurable priority.
    */
    __asm("CPSID   I"); 

    /*Load address of currentTcbPointer into R0. */
    __asm("LDR R0, =currentTcbPointer"); 

    /* Load R2 from address equals R0, r2 =currentTcbPointer*/  
    __asm("LDR R2, [R0]");

    /* Load Cortex-M SP(R13) from address equals R1, i.e SP = currentTcbPointer->stackPointer*/
    __asm("LDR R4,[R2]");
    __asm("MOV SP,R4");

    /*-----------------------------------------------------------------------------------------------------------
    Restore R11-R4 (High registers cannot be directly accessed by the POP instruction so manipulating the stack pointer
    manually is necessary. )*/
    

    /*Low registers can be popped directly. */
    
    __asm("POP {R4-R7}");

    /* Restore R8 - R12 by loading the value pointed by the STACK pointer and incrementing stack
    pointer by 4 bytes to point to the next register (Stack grows downwards in memory)
    */

    __asm("LDR R4, [SP]"); 
    __asm("MOV R8, R4");
    __asm("ADD SP, SP, #4"); /* Point to next value in stack. */


    __asm("LDR R4, [SP]");
    __asm("MOV R9, R4"); 
    __asm("ADD SP, SP, #4"); /* Point to next value in stack. */

    __asm("LDR R4, [SP]");
    __asm("MOV R10, R4");
    __asm("ADD SP, SP, #4"); /* Point to next value in stack. */

    __asm("LDR R4, [SP]");
    __asm("MOV R11, R4"); 
    __asm("ADD SP, SP, #4"); /* Point to next value in stack. */

    __asm("LDR R4, [SP]");
    __asm("MOV R12, R4"); 
    __asm("ADD SP, SP, #4"); /* Point to next value in stack. */

    /* Re-init R4 since we were using R4 as a scratch pad register to copy values to other registers. */
    __asm("LDR R4, =0xCAFEBABE" ); 

    /* -----------------------------------------------------------------------------------------------------------*/

    /* Restore R0, R1, R2, R3*/
    __asm("POP {R0-R3}");


    /* Skip LR and point towards PC*/
    __asm("ADD SP,SP,#4"); 

    /* Save PC value (task0 function address) from the stack to LR so that we return from SchedulerLaunch to task0 */
    __asm("POP {R4}");
    __asm("MOV LR, R4"); 

    /* Skip PSR by adding 4 to SP*/
    __asm("ADD SP,SP,#4");

    /* Re-init R4 since we were using R4 as a scratch pad register to copy values to other registers. */
    __asm("LDR R4, =0xCAFEBABE" ); 

    /* Exit critical section. */
    __asm("CPSIE   I");  

    /* Jump towards task0 function.*/
    __asm("BX   LR"); 

}


/* 
    This function triggers a systick exception, forcing
    the next task to run, therefore making the task cooperative. 

    In a cooperative scheduler, the operating ystem never initiates a context switch 
    from a running process to another process. Instead, in order to run multiple applications
    concurrently, processes voluntarily yield control periodically or when idle or logically blocked. 

    This type of multitasking is called cooperative becauseall programs must cooperatefor scheduling
    scheme to work. 

*/
void osKernel_TaskYield (void)
{
    /*Clear the systick value register and trigger a systick exception. */
    SysTick->VAL = 0; /* Potentially not a good idea to clear manually*/
    SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;


    
}


extern void task3(void); 

void osKernel_RoundRobinScheduler(void)
{
    if ((++periodic_task_tick) >= (10))
    {
        (*task3)();
        periodic_task_tick = 0; 
    }

    currentTcbPointer = currentTcbPointer->nextTcbPointer; 
}

