#include<stdint.h>
void osSemaphoreInit(int32_t *semaphore, int32_t value){
	*semaphore=value;
}

void osSignalSet(int32_t *semaphore){
	__disable_irq();
	*semaphore+=1;
	__enable_irq();
}

void osSignalWait(int32_t *semaphore){
	__disable_irq();
	while(*semaphore<=0){
	__disable_irq();
	__enable_irq();
	}
	*semaphore-=1;
	__enable_irq();
}