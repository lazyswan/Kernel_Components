#ifndef _OS_KERNEL_H
#define _OS_KERNEL_H
#include <stdint.h>
//APIs

void osKernelLaunch(uint32_t quanta);
void osKernelInit();
//Only can take 3 Static Threads
//This is a static RR-scheduler
uint8_t osKernelAddThreads(void (*thread1)(),
													 void (*therad2)(),
													 void (*thread3)());
#endif