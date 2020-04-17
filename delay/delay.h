#ifndef DELAY_H
#define DELAY_H

#include<rtcclock/rtcclock.h>

void delay_for(volatile unsigned int clocks);

void delay_for_ms(unsigned int millis);

void delay_for_us(unsigned int micros);

#endif