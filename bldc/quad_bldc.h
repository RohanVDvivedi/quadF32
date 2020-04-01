#ifndef QUAD_BLDC_H
#define QUAD_BLDC_H

#include<regs/rcc.h>
#include<regs/gpio.h>
#include<regs/timer.h>

void init_bldc();

void set_motors(uint32_t m1, uint32_t m2, uint32_t m3, uint32_t m4);

#endif