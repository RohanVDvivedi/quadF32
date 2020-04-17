#ifndef PWR_REGS_H
#define PWR_REGS_H

#include<stdint.h>

typedef struct PWR_t PWR_t;
struct PWR_t
{
	uint32_t PWR_CR;
	uint32_t PWR_CSR;
};

#define PWR ((volatile PWR_t*)(0x40007000))

#endif