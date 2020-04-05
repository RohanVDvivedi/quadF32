#ifndef RC_RECEIVER_H
#define RC_RECEIVER_H

#include<regs/rcc.h>
#include<regs/gpio.h>
#include<regs/nvic.h>
#include<regs/exti.h>
#include<regs/afio.h>
#include<regs/timer.h>

void init_rc_receiver();

uint32_t get_rc_channels(uint32_t chan_ret[6]);

#endif