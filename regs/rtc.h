#ifndef RTC_REGS_H
#define RTC_REGS_H

#include<stdint.h>

typedef struct RTC_t RTC_t;
struct RTC_t
{
	uint32_t RTC_CRH;
	uint32_t RTC_CRL;
	uint32_t RTC_PRLH;
	uint32_t RTC_PRLL;
	uint32_t RTC_DIVH;
	uint32_t RTC_DIVL;
	uint32_t RTC_CNTH;
	uint32_t RTC_CNTL;
	uint32_t RTC_ALRH;
	uint32_t RTC_ALRL;
};

#define RTC ((volatile RTC_t*)(0x40002800))

#endif