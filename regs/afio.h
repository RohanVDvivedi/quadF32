#ifndef AFIO_REGS_H
#define AFIO_REGS_H

#include<stdint.h>

typedef struct AFIO_t AFIO_t;
struct AFIO_t
{
	uint32_t AFIO_EVCR;
	uint32_t AFIO_MAPR;
	uint32_t AFIO_EXTICR1;
	uint32_t AFIO_EXTICR2;
	uint32_t AFIO_EXTICR3;
	uint32_t AFIO_EXTICR4;
	uint32_t AFIO_MAPR2;
};

#define AFIO ((volatile AFIO_t*)(0x40010000))

#endif