#ifndef RTCCLOCK_H
#define RTCCLOCK_H

#include<regs/rtc.h>

void rtc_init();

uint32_t get_now_micros();

#endif