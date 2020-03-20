/*
Author : Swanand Sapre

-xxxxxxxxx Stack Frames xxxxxxxxxxxxxxxxx-
 When Interrrupt Occurs,
 Cortex M Processor, Save the content of register in side the stack
 which is in RAM/MM/.
 After finishing the ISR it restores the content of stacks, populates
 its register and resumes at same point where it was interrupted.
 
 "When the processor takes an exception, unless the exception is a tail-chained or a late-arriving
exception, the processor pushes information onto the current stack. This operation is referred to
as stacking and the structure of eight data words(32 bits) is referred as the stack frame."

R0,R1,R2,R3,R12,LR,PC,xPSR are saved into the stacks.
stack-pointer register points to top of stack


-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx-

-xxxxxxxxx Shitty Way of Context Switching xxxxxxxxxxxxxxxxx-
 1.Put a break point at ISR handler,
 2. Once trap, Change the value of PC in the memory/stack frame to the
 adress of function you want to execute.
 3.After execution of ISR , you will execute the the new fucntion 
 4. This is not exactly context switching since processor didnt resume where it started,
 You might Crash.
 
 Whats are Context?
 Set of values of register for each function
 Another set values of Register for another Function
 This Function can be reffered as thread.
 
-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx-
*/


#include "stm32f4xx.h"                  // Device header

#define STACKFRAME 1 

#if STACKFRAME==1
#define RED					(1U<<14)
#define GREEN 			(1U<<12)
#define RED_BIT 		(1U<<28)
#define GREEN_BIT 	(1U<<24)
#define GPIOD_CLOCK (1U<<3)

//Array to hold the values of 8 Wordsize Registers (Context)
uint32_t green_stack[40];
uint32_t red_stack[40];

//Stack Pointers to point to top of the Stack
uint32_t* sp_green_stack=&green_stack[40];
uint32_t* sp_red_stack=&red_stack[40];


volatile uint32_t tick;
volatile uint32_t _tick;



void gpio_init();
void DelayS(uint32_t secounds);

void blink_green_led(){
	while(1){		
		GPIOA->ODR ^=GREEN; //toggle the LED with XOR
			DelayS(1);
	}	
}


void blink_red_led(){
	while(1){		
		GPIOA->ODR ^=RED; //toggle the LED with XOR
			DelayS(1);
	}	
}


int main(){
	gpio_init();
	//Stack Frame for Green_Blink_Function (Context of That Thread)
	*(--sp_green_stack)=(1U<<24); //xPSR
	*(--sp_green_stack)= (uint32_t)&blink_green_led; //PC-->FUNCTIONAL POinter
	*(--sp_green_stack)= (uint32_t)(0X0000000DU);//LR
	*(--sp_green_stack)= (uint32_t)(0X0000000EU);//R12
	*(--sp_green_stack)= (uint32_t)(0X0000000AU);//R3
	*(--sp_green_stack)= (uint32_t)(0X0000000EU);//R2
	*(--sp_green_stack)= (uint32_t)(0X0000000AU);//R1
	*(--sp_green_stack)= (uint32_t)(0X0000000DU);//R0
	
	//Stack Frame for Red_Blink_Function(Context of That Thread)
	*(--sp_red_stack)=(1U<<24); //xPSR
	*(--sp_red_stack)=(uint32_t) &blink_red_led; //PC
	*(--sp_red_stack)=(0x0000000BU); //LR
	*(--sp_red_stack)=(0x0000000EU); //R12
	*(--sp_red_stack)=(0x0000000EU); //R3
	*(--sp_red_stack)=(0x0000000EU); //R2
	*(--sp_red_stack)=(0x0000000FU); //R1
	*(--sp_red_stack)=(0x0000000FU); //R0
	blink_green_led();
	blink_red_led();
	while(1);
}
void gpio_init(){		
	RCC->AHB1ENR |=GPIOD_CLOCK;
	GPIOD->MODER |=RED_BIT | GREEN_BIT;
	SystemCoreClockUpdate(); //Gives Current System Clock
	SysTick_Config(SystemCoreClock/100U); //1000 for ms | 100 for sec --> because clk is in Mhz
	__enable_irq(); //Enables the INTR
}

//Sys tick handler, ISR executed in everytick intr
void SysTick_Handler(){
	++tick;	
}

uint32_t getTick(){
	__disable_irq();
	_tick=tick;
	__enable_irq();	
	return _tick;	
}

//Busy Loop
void DelayS(uint32_t secounds){
	secounds *=100;
	uint32_t temp=getTick();
	while((getTick()-temp)<secounds);
	
}
#endif //GPIO_DRIVER