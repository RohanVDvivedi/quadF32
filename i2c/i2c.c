#include<i2c/i2c.h>

#include<uart/uart.h>

static uint32_t temp[16];
static char num[11];
static void stringify_32(char* num, uint32_t n)
{
	int j = 0; int i = 0;
	num[j++] = '0';
	num[j++] = 'x';
	for(i = 1; i <= 8; i++, j++)
	{
		num[j] = (char)((n>>((8-i)*4))&0x0f);
		if(num[j] <= 0x09)
		{
			num[j] += '0';
		}
		else if(num[j] <= 0x0f)
		{
			num[j] -= 0x0a;
			num[j] += 'A';
		}
	}
	num[j++] = '\n';
}

static void init_temps()
{
	uint32_t i = 0;
	for(i = 0; i < 16; i++)
	{
		temp[i] = 0xf1f2f3f4;
	}
}

static void print_temps()
{
	uint32_t i = 0;
	for(i = 0; i < 16; i++)
	{
		uart_write_through_dma("addr -> ", 8);
		stringify_32(num, i);num[10] = '=';uart_write_through_dma(num, 11);
		uart_write_through_dma(" ", 1);
		stringify_32(num, temp[i]);uart_write_through_dma(num, 11);
	}
	uart_write_through_dma("\n", 1);
}

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
	if(addr_acked)
	{
		send_stop_bit();
	}
	return addr_acked;
}

void i2c_read(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_read)
{
	init_temps();static int first = 0;
	if(first == 1)
	{
		temp[0] = I2C1->I2C_SR1;
		goto PRINT;
	}
	send_start_bit();GPIOC->GPIO_ODR |= (1 << 13);

	int addr_acked = i2c_send_address(device_address << 1);

	if(addr_acked)
	{
		if(bytes_to_read > 0)
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

			unsigned int i = 0;
			for(i = 0; i < bytes_to_read; i++)
			{
				i2c_byte_read_from_bus(((char*)(buffer)) + i);
				if((i + 1) == (bytes_to_read - 1))
				{
					I2C1->I2C_CR1 &= ~(1<<10);
				}
			}
		}

		send_stop_bit();
	}
	PRINT:;
	first++;
	print_temps();
}

void i2c_write(uint8_t device_address, uint8_t reg_address, void* buffer, unsigned int bytes_to_write)
{
	send_start_bit();

	i2c_send_address(device_address << 1);

	if(bytes_to_write > 0)
	{
		i2c_byte_write_on_bus(reg_address);

		int i = 0;
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