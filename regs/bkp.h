#ifndef BKP_REGS_H
#define BKP_REGS_H

#include<stdint.h>

typedef struct BKP_t BKP_t;
struct BKP_t
{
	uint32_t RESERVED1;
	uint32_t BKP_DR_1[10];
	uint32_t BKP_RTCCR;
	uint32_t BKP_CR;
	uint32_t BKP_CSR;
	uint32_t RESERVED2;
	uint32_t RESERVED3;
	uint32_t BKP_DR_2[32];
};

#define BKP ((volatile BKP_t*)(0x40006C00))

#endif