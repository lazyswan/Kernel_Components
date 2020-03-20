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
*/

#include "stm32f4xx.h"                  // Device header

#define GPIO_DRIVER 

#ifdef GPIO_DRIVER
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