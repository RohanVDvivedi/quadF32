#include<i2c/i2c.h>

#include<uart/uart.h>

int i2c_init()
{
	RCC->RCC_APB1ENR |= (1<<21);
	RCC->RCC_APB2ENR |= (1<<3);

	GPIOB->GPIO_CRL |= (0xff<<24);

	/*I2C1->I2C_CR2 = (I2C1->I2C_CR2 & ~(0x3f)) | 36;
	I2C1->I2C_CCR = 0b100000000011110;
	I2C1->I2C_CR1 |= ((1<<8) | (1<<0));*/

	I2C1->I2C_CR2 = (I2C1->I2C_CR2 & ~(0x3f)) | 8;
	I2C1->I2C_CCR = 0x28;
	I2C1->I2C_TRISE = 0x09;
	I2C1->I2C_CR1 |= (1<<8);
	I2C1->I2C_CR1 |= (1<<0);
}

static void send_start_bit()
{
	I2C1->I2C_CR1 |= (1<<8);
	while(!(I2C1->I2C_SR1 & (1<<0)));
}

// returns 1 if device found
static int i2c_send_address(uint8_t slave_address)
{
	I2C1->I2C_DR = slave_address;
	while(!(I2C1->I2C_SR1 & (1<<1)) && !(I2C1->I2C_SR1 & (1<<10)));
	int success = 0;
	if(I2C1->I2C_SR1 & (1<<1))
	{
		volatile uint32_t dummy_read = I2C1->I2C_SR2;
		success = 1;
	}
	if(I2C1->I2C_SR1 & (1<<10))
	{
		I2C1->I2C_SR1 &= ~(1<<10);
		success = 0;
	}
	return success;
}

static int i2c_byte_read_from_bus(char* r)
{
	if(!(I2C1->I2C_SR2 & (1<<2)))
	{
		while(!(I2C1->I2C_SR1 & (1<<6))){}
		(*r) = I2C1->I2C_DR;
		return 1;
	}
	return 0;
}

static int i2c_byte_write_on_bus(char s)
{
	if(I2C1->I2C_SR2 & (1<<2))
	{
		while(!(I2C1->I2C_SR1 & (1<<7))){}
		I2C1->I2C_DR = s;
		return 1;
	}
	return 0;
}

static void wait_for_byte_to_be_sent()
{
	while(!(I2C1->I2C_SR1 & (1<<2))){}
}

static void send_stop_bit()
{
	I2C1->I2C_CR1 |= (1<<9);
	while(I2C1->I2C_SR2 & (1<<0));
}

int i2c_detect(uint8_t device_address)
{
	send_start_bit();
	int addr_acked = i2c_send_address(device_address << 1);
	send_stop_bit();
	return addr_acked;
}

void i2c_read(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_read)
{
	send_start_bit();

	int addr_acked = i2c_send_address(device_address << 1);

	if(bytes_to_read > 0 && addr_acked)
	{
		i2c_byte_write_on_bus(reg_address);

		wait_for_byte_to_be_sent();

		send_start_bit();

		i2c_send_address((device_address << 1) | 1);

		// turn on ack, for slave
		if(bytes_to_read > 1)
		{
			I2C1->I2C_CR1 |= (1<<10);
		}
		else
		{
			I2C1->I2C_CR1 |= (1<<9);
		}

		unsigned int i = 0;
		for(i = 0; i < bytes_to_read; i++)
		{
			i2c_byte_read_from_bus(((char*)(buffer)) + i);
			if((i + 1) == (bytes_to_read - 1))
			{
				I2C1->I2C_CR1 &= ~(1<<10);
				I2C1->I2C_CR1 |= (1<<9);
			}
		}
	}
	else
	{
		I2C1->I2C_CR1 |= (1<<9);
	}

	while(I2C1->I2C_SR2 & (1<<0));
}

void i2c_write(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_write)
{
	send_start_bit();

	int addr_acked = i2c_send_address(device_address << 1);

	if(bytes_to_write > 0 && addr_acked)
	{
		i2c_byte_write_on_bus(reg_address);

		unsigned int i = 0;
		for(i = 0; i < bytes_to_write; i++)
		{
			i2c_byte_write_on_bus(((char*)(buffer))[i]);
		}

		wait_for_byte_to_be_sent();
	}

	send_stop_bit();
}

void i2c_read_using_dma(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_read)
{

}

int i2c_destroy()
{
	RCC->RCC_APB1ENR &= ~(1<<21);
}