#include "osKernel.h"
#define QUANTA	100
uint32_t count0,count1,count2;
int32_t semaphore1,semaphore2;
uint32_t volatile shared_resource=0;

void Task0(void){
	while(1)	{
		count0++;
		//osThreadYeild();
	}
}
void Task1(void){
	while(1){
		count1++;
	  osSignalWait(&semaphore2);
		shared_resource++;
		osSignalSet(&semaphore2);
		
	}	
}
void Task2(void){
	while(1){
		count2++;
		osSignalWait(&semaphore2);
		shared_resource--;
		osSignalSet(&semaphore2);
	}	
}
int main(void){
		//both semaphores are set.
	osSemaphoreInit(&semaphore1,1);
	osSemaphoreInit(&semaphore2,1);
	osKernelInit();
	osKernelAddThreads(&Task0,&Task1,&Task2);
	osKernelLaunch(QUANTA);
	
}

