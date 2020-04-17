#include<rtcclock/rtcclock.h>

void rtc_init()
{
	RCC->RCC_CSR |= (1<<0);
	while(!(RCC->RCC_CSR & (1<<1))){}

	RCC->RCC_BDCR |= (1<<16);

	RCC->RCC_BDCR = (RCC->RCC_BDCR & ~(0x3<<8)) | (0x2<<8);

	RCC->RCC_BDCR |= (1<<15);

	while(!(RTC->RTC_CRL & (1<<5)));
	RTC->RTC_CRL |= (1<<4);

	uint32_t PRL = 0;
	RTC->RTC_PRLH = (PRL >> 16) & 0xffff;
	RTC->RTC_PRLL = PRL & 0xffff;

	uint32_t CNT = 0;
	RTC->RTC_CNTH = (CNT >> 16) & 0xffff;
	RTC->RTC_CNTL = CNT & 0xffff;

	RTC->RTC_CRL &= ~(1<<4);
	while(!(RTC->RTC_CRL & (1<<5)));
}

uint64_t get_now_micros()
{
	return ((RTC->RTC_CNTH << 16) | RTC->RTC_CNTL) * 25;
}