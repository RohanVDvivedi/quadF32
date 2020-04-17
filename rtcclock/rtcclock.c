#include<rtcclock/rtcclock.h>

void rtc_init()
{
	
}

uint32_t get_now_micros()
{
	return (RTC->RTC_CNTH << 16) | RTC->RTC_CNTL;
}