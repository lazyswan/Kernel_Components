
/*
Author:
Swanand Sapre
scsapre@gmail.com
*/


#define COOPERATIVE 0
#if COOPERATIVE==1
#include<stdint.h>
#include "osKernel.h"
void osSemaphoreInit(int32_t *semaphore, int32_t value){
	*semaphore=value;
}

//must be atomic
void osSignalSet(int32_t *semaphore){
	__disable_irq();
	*semaphore+=1;
	__enable_irq();
}

//instead of busywaiting for entire Quanta, task yeilds to another task
void osSignalWait(int32_t *semaphore){
	__disable_irq();
	while(*semaphore<=0){
		__disable_irq();
		osThreadYeild();
		__enable_irq();
		
	}
	*semaphore-=1;
	__enable_irq();
}
#endif