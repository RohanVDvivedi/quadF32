#include<i2c/i2c.h>

int i2c_init()
{
	RCC->RCC_APB1ENR |= (1<<21);
	RCC->RCC_APB2ENR |= (1<<3);
	GPIOB->GPIO_CRL |= (0xff<<24);

	I2C1->I2C_CR2 = (I2C1->I2C_CR1 & ~(0x3f)) | 36;
	I2C1->I2C_CCR = 0b100000000011110;
	I2C1->I2C_CR1 |= ((1<<8) | (1<<0));
}

static void send_start_bit()
{
	I2C1->I2C_CR1 |= (1<<8);
	while(!(I2C1->I2C_CR1 & (1<<0)));
}

static void i2c_send_address(uint8_t slave_address)
{
	I2C1->I2C_DR = slave_address;
	while(!(I2C1->I2C_SR1 & (1<<1))){}
}

static char i2c_byte_read_from_bus()
{
	while(!(I2C1->I2C_SR1 & (1<<6))){}
	return I2C1->I2C_DR;
}

static void i2c_byte_write_on_bus(char c)
{
	while(!(I2C1->I2C_SR1 & (1<<7))){}
	I2C1->I2C_DR = c;
}

static void wait_for_byte_to_be_sent()
{
	while(!(I2C1->I2C_SR1 & (1<<2))){}
}

static void send_stop_bit()
{
	I2C1->I2C_CR1 |= (1<<9);
	while(!(I2C1->I2C_SR1 & (1<<4)));
}

void i2c_read(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_read)
{
	send_start_bit();

	i2c_send_address(device_address << 1);

	i2c_byte_write_on_bus(reg_address);

	send_start_bit();

	i2c_send_address((device_address << 1) | 1);

	int i = 0;
	for(i = 0; i < bytes_to_read; i++)
	{
		((char*)(buffer))[i] = i2c_byte_read_from_bus();
	}

	send_stop_bit();
}

void i2c_write(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_write)
{
	send_start_bit();

	i2c_send_address(device_address << 1);

	i2c_byte_write_on_bus(reg_address);

	int i = 0;
	for(i = 0; i < bytes_to_write; i++)
	{
		i2c_byte_write_on_bus(((char*)(buffer))[i]);
	}

	wait_for_byte_to_be_sent();

	send_stop_bit();
}

void i2c_read_using_dma(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_read)
{

}

int i2c_destroy()
{
	RCC->RCC_APB1ENR &= ~(1<<21);
}