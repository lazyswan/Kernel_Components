#include "osKernel.h"

#define QUANTA	10

uint32_t count0,count1,count2;
int32_t semaphore1,semaphore2;
uint32_t volatile shared_resource=0;
void Task0(void)
{
	while(1)
	{
		count0++;
		osThreadYeild();
	}
	
}


void Task1(void)
{
	while(1)
	{
		osSignalWait(&semaphore2);
		shared_resource++;
		osSignalSet(&semaphore1);
	}
	
}


void Task2(void)
{

	while(1)
	{
		osSignalWait(&semaphore1);
		shared_resource--;
		osSignalSet(&semaphore2);
	}
	
}


int main(void)
{
	osSemaphoreInit(&semaphore1,0);
	osSemaphoreInit(&semaphore2,1);
	osKernelInit();
	osKernelAddThreads(&Task0,&Task1,&Task2);
	osKernelLaunch(QUANTA);
	//both semaphores are set.
	
	
}

