/*
Author:
Swanand Sapre
scsapre@gmail.com
*/

#include "osKernel.h"
#include "stm32f4xx.h"                  // Device header



#define NUM_OF_THREADS	3
#define STACK_SIZE			100
#define BUS_FREQ				16000000
#define SYSPRI3         (*((volatile uint32_t *)0xE000ED20))
#define INTRCTRL				(*((uint32_t volatile *)0xE000ED04))

void osSchedulerLaunch(void);
uint32_t MILLIS_PRESCALAR;

//Thread Control Block
typedef struct tcb{
	int32_t *sp; 		//stack pointer 
	struct tcb *next;	
}tcb_t;

//Similar to process descriptor table;
//This one is thread descriptor table/Array of TCB.
tcb_t tcbs[NUM_OF_THREADS];

//Holds Pointer to current running Process
tcb_t *current_tcb;

//stack area for all the threads in form of 2-d matrix
int32_t TCB_STACK[NUM_OF_THREADS][STACK_SIZE];

/*
Initialses the stack,
PC and xPSR for each Thread.
thread_num: 0,1,2
*/
void osKernelStackInit(int thread_num){
	
	//Stack pointer initialised to bottom of its stack. 
	tcbs[thread_num].sp = & TCB_STACK[thread_num][STACK_SIZE-16];
	
	//Runining in the thumb mode | xpsr rx Bit 24.
	TCB_STACK[thread_num][STACK_SIZE-1]=1<<24;	
			
}
/*
This function decides which thread to run,
this function must be atomic, its critical section
*/
uint8_t osKernelAddThreads(void (*task0)(),void (*task1)(),void (*task2)()){
	
	//connected all the tcbs in circular LL fashion		
		__disable_irq();
	
		tcbs[0].next=&tcbs[1];
		tcbs[1].next=&tcbs[2];
		tcbs[2].next=&tcbs[0];		
	
	//initialise PC with task0,callback function	
	osKernelStackInit(0);
	TCB_STACK[0][STACK_SIZE-2]= (int32_t)(task0);
	
	//initialise PC with task1,callback function
	osKernelStackInit(1);
	TCB_STACK[1][STACK_SIZE-2]= (int32_t)(task1);
	
	//initialise PC with task2,callback function
	osKernelStackInit(2);
	TCB_STACK[2][STACK_SIZE-2]= (int32_t)(task2);
	
	//First Task to be scheduled is task0
	current_tcb= &tcbs[0];
	__enable_irq();
	
	
	return 1;											 
}
void osKernelInit(){
	
	__disable_irq();
	MILLIS_PRESCALAR=BUS_FREQ/1000;
}

void osKernelLaunch(uint32_t quanta){
	//initialising SysTick
	SysTick->CTRL=0;
	SysTick->VAL=0;
	SYSPRI3 =(SYSPRI3&0x00FFFFFF)|0xE0000000; //priority to 7
	
	//SysTick Interrupts after @ LOAD value | quanta
	SysTick->LOAD=(quanta*MILLIS_PRESCALAR)-1;
	
	SysTick->CTRL = 0x00000007;
	
	//This is assembly Function
	osSchedulerLaunch();
}

/*
API for Cooperative Scheduler
to perform task Yeild.
Voluntarily perform a context switch before quanta time expires.
*/
void osThreadYeild(){
	
	//reset the counter value to 0;
	SysTick->VAL=0;
	//this will generate a 
	//sysTick Intr and then sysTick Handler will perform a context SW.
	INTRCTRL= 1<<26;
	
}