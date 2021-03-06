/*
Author : Swanand Sapre
1. Enable Clock Access to Port of the Pin
2.Set the Pin's Mode to IP/OP*
3.Special parameters -- Pullups;Pulldown

Refering to Datasheet, 
PortD-GPIO is connected to AHB1 Bus,
So we need to supply clock and power this bus before using Port D
#	RCC_AHB1ENR -bit:3
# GPIOx_MODER
# GPIOx_ODR

-xxxxxxxxx BackGround ForeGround Thread Design xxxxxxxxxxxxxxxxx-
main() function is fore ground,
isr() (systick handler) is in background.
-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx-

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

#define GPIO_DRIVER 0

#if GPIO_DRIVER==1
#define RED					(1U<<14)
#define GREEN 			(1U<<12)
#define RED_BIT 		(1U<<28)
#define GREEN_BIT 	(1U<<24)
#define GPIOD_CLOCK (1U<<3)

volatile uint32_t tick;
volatile uint32_t _tick;

void gpio_init();
void DelayS(uint32_t secounds);

int main(){
	gpio_init();
	while(1){
			GPIOA->ODR ^=RED; //toggle the LED with XOR
			DelayS(1);
		}
}
void gpio_init(){		
	RCC->AHB1ENR |=GPIOD_CLOCK;
	GPIOD->MODER |=RED_BIT;
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