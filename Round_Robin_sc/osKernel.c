#include "osKernel.h"
#include "stm32f4xx.h"                  // Device header

void osSchedulerLaunch(void);

#define NUM_OF_THREADS	3
#define STACK_SIZE			100
#define BUS_FREQ				16000000
#define SYSPRI3         (*((volatile uint32_t *)0xE000ED20))
#define INTRCTRL				(*((uint32_t volatile *)0xE000ED04))
uint32_t MILLIS_PRESCALAR;
typedef struct tcb{
	int32_t *stackpt;
	struct tcb *pnext;	
}tcb_t;
//Similar to process descriptor table; This one is thread descriptor table/Array of TCB.
tcb_t tcbs[NUM_OF_THREADS];
tcb_t *current_tcb;
int32_t TCB_STACK[NUM_OF_THREADS][STACK_SIZE];

/*
Initialses the stack,
PC and xPSR for each Thread.
thread_num: 0,1,2
*/
void osKernelStackInit(int thread_num){
	tcbs[thread_num].stackpt = & TCB_STACK[thread_num][STACK_SIZE-16];
	//Runining in the thumb mode | xpsr rx Bit 24.
	TCB_STACK[thread_num][STACK_SIZE-1]=0x01000000;	
			
}
/*
This function decides which thread to run,this function must be atomic, its critical
*/
uint8_t osKernelAddThreads(void (*task0)(),void (*task1)(),void (*task2)()){
	//connected all the tcbs		
		__disable_irq();
	
		tcbs[0].pnext=&tcbs[1];
		tcbs[1].pnext=&tcbs[2];
		tcbs[2].pnext=&tcbs[0];
		
		osKernelStackInit(0);
	//initialise PC with task0,callback function
	TCB_STACK[0][STACK_SIZE-2]= (int32_t)(task0);
		osKernelStackInit(1);
	TCB_STACK[1][STACK_SIZE-2]= (int32_t)(task1);
		osKernelStackInit(2);
	TCB_STACK[2][STACK_SIZE-2]= (int32_t)(task2);
	current_tcb= &tcbs[0];
	__enable_irq();
	return 1;														 
														 
}
void osKernelInit(){
		__disable_irq();
	MILLIS_PRESCALAR=BUS_FREQ/1000;	
	
}

void osKernelLaunch(uint32_t quanta){
	SysTick->CTRL=0;
	SysTick->VAL=0;
	SYSPRI3 =(SYSPRI3&0x00FFFFFF)|0xE0000000; //priority to 7
	SysTick->LOAD=(quanta*MILLIS_PRESCALAR)-1;
	SysTick->CTRL = 0x00000007;
	osSchedulerLaunch();
}
//Cooperative Scheduler
void osThreadYeild(){
	//Creating a systick Interrrupt
	SysTick->VAL=0;
	INTRCTRL= 1<<26;
	
}