#ifndef NVIC_REGS_H
#define NVIC_REGS_H

typedef struct NVIC_t NVIC_t;
struct NVIC_t
{
	uint32_t NVIC_ISER[3];
	uint32_t NVIC_ICER[3];
	uint32_t NVIC_ISPR[3];
	uint32_t NVIC_ICPR[3];
	uint32_t NVIC_IABR[3];
	uint32_t NVIC_IPR[21];
};

#define NVIC ((volatile NVIC_t*)(0xE000E100))

#endif