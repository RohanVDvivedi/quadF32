#include<rtcclock/rtcclock.h>

#include<regs/gpio.h>

void rtc_init()
{
	// turn on clock to enable writing to power registers and backup interface
	RCC->RCC_APB1ENR |= (3<<27);

	// disable backup and rtc registers write protection
	PWR->PWR_CR |= (1<<5);

	// turn on LSI clock and wait for it to get stable
	RCC->RCC_CSR |= (1<<0);
	while(!(RCC->RCC_CSR & (1<<1))){}

	// reset backup domain, select LSI as RTC clock and enable RTC clock
	RCC->RCC_BDCR |= (1<<16);
	RCC->RCC_BDCR = (RCC->RCC_BDCR & ~(0x3<<8)) | (0x2<<8);
	RCC->RCC_BDCR |= (1<<15);

	// check if the previous RTC register writes completed
	while(!(RTC->RTC_CRL & (1<<5)));
	// setup RTC in configuration mode
	RTC->RTC_CRL |= (1<<4);

	uint32_t PRL = 0;
	RTC->RTC_PRLH = (PRL >> 16) & 0xffff;
	RTC->RTC_PRLL = PRL & 0xffff;

	uint32_t CNT = 0;
	RTC->RTC_CNTH = (CNT >> 16) & 0xffff;
	RTC->RTC_CNTL = CNT & 0xffff;

	// turn of configuration for RTC registers
	RTC->RTC_CRL &= ~(1<<4);
	// check if the RTC register writes completed
	while(!(RTC->RTC_CRL & (1<<5)));
}

uint64_t get_now_micros()
{
	return ((RTC->RTC_CNTH << 16) | RTC->RTC_CNTL) * 25;
}