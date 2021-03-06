#ifndef SYS_CLOCK_H
#define SYS_CLOCK_H

#include<regs/rcc.h>
#include<regs/flash_control.h>

// the oscillator connected to the controller on my board
// change this as it suits your needs
#define HSE_CLOCK 8000000

// if your microcontroller is stm32f103 do not modify anything beyond this point
#define HSI_CLOCK 8000000

#define MAX_AHB_CLOCK 72000000
#define MAX_APB2_CLOCK 72000000
#define MAX_APB1_CLOCK 36000000

#define MAX_SYS_CLOCK 72000000

typedef enum clk_source clk_source;
enum clk_source
{
	HSI = 0x0,
	HSE = 0x1,
	PLL = 0x2
};

void setup_pll_module(clk_source source, uint32_t frequency);

void change_sys_clock_source(clk_source source, uint32_t frequency);

clk_source get_sys_clock_source();

uint32_t get_sys_clock_frequency();

uint32_t get_AHB_clock_frequency();

uint32_t get_APB2_clock_frequency();

uint32_t get_APB1_clock_frequency();

#endif