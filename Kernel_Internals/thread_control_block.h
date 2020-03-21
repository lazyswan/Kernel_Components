
#include <stdint.h>

#define NUMBER_OF_THREAD 4
#define STACK_SIZE 100
//each entry in stack is of 4-Byte (32-bit addressable)

typedef struct tcb{
	int32_t *pstack;
	struct tcb *next_tcb;
	
}tcb_t;
tcb_t tcbs[NUMBER_OF_THREAD];
tcb_t *current_tcb;
int32_t TCBS_Stacks[NUMBER_OF_THREAD][STACK_SIZE];