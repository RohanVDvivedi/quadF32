#ifndef FLASH_CONTROL_REGS_H
#define FLASH_CONTROL_REGS_H

#include<stdint.h>

#define FLASH_ACR (*((volatile uint32_t*)(0x40022000)))

#endif