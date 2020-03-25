#include<i2c/i2c.h>

int i2c_init()
{
	RCC->RCC_APB1ENR |= (1<<21);
	RCC->RCC_APB2ENR |= (1<<3);
	GPIOB->GPIO_CRL |= (0xff<<24);
}

int i2c_read(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_read)
{

}

int i2c_write(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_write)
{

}

int i2c_read_blocking(uint8_t device_address, void* buffer, unsigned int bytes_to_read)
{

}

int i2c_write_blocking(uint8_t device_address, void* buffer, unsigned int bytes_to_write)
{

}

int i2c_read_dma(uint8_t device_address, void* buffer, unsigned int bytes_to_read)
{

}

int i2c_destroy()
{
	RCC->RCC_APB1ENR &= ~(1<<21);
}