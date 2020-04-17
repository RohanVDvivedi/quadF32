#ifndef RTCCLOCK_H
#define RTCCLOCK_H

#include<regs/rcc.h>
#include<regs/rtc.h>
#include<regs/pwr.h>

void rtc_init();

uint64_t get_now_micros();

#endif