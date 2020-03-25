#ifndef I2C_REGS_H
#define I2C_REGS_H

#include<stdint.h>

typedef struct I2C_t I2C_t;
struct I2C_t
{
	uint32_t I2C_CR1;
	uint32_t I2C_CR2;
	uint32_t I2C_OAR1;
	uint32_t I2C_OAR2;
	uint32_t I2C_DR;
	uint32_t I2C_SR1;
	uint32_t I2C_SR2;
	uint32_t I2C_CCR;
	uint32_t I2C_TRISE;
};

#define I2C1 ((volatile I2C_t*)(0x40005400))
#define I2C2 ((volatile I2C_t*)(0x40005800))

#endif