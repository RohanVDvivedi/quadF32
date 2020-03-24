#ifndef I2C_H
#define I2C_H

#include<regs/gpio.h>
#include<regs/i2c.h>
#include<regs/dma.h>

int i2c_init();

int i2c_read(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_read);

int i2c_write(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_write);

int i2c_read_raw(uint8_t device_address, void* buffer, unsigned int bytes_to_read);

int i2c_write_raw(uint8_t device_address, void* buffer, unsigned int bytes_to_write);

int i2c_destroy();

#endif