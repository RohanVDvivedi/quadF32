#ifndef DELAY_H
#define DELAY_H

#include<rtcclock/rtcclock.h>

void delay_for(volatile unsigned int clocks);

void delay_for_ms(uint64_t millis);

void delay_until_ms(uint64_t until_millis);

void delay_for_us(uint64_t micros);

void delay_until_us(uint64_t until_micros);

#endif