#include<delay/delay.h>

#include<regs/rcc.h>
#include<regs/gpio.h>

#include<sysclock/sysclock.h>
#include<uart/uart.h>
#include<i2c/i2c.h>
#include<bldc/quad_bldc.h>
#include<rc_receiver/rc_receiver.h>

char hex_to_char(uint8_t n)
{
	if(0x0 <= n && n <= 0x9)
	{
		return n + '0';
	}
	else if(0xa <= n && n <= 0xf)
	{
		return n - 0xa + 'a';
	}
}

uint8_t char_to_hex(char c)
{
	if('0' <= c && c <= '9')
	{
		return c - '0';
	}
	else if('a' <= c && c <= 'f')
	{
		return c - 'a' + 0xa;
	}
	else if('A' <= c && c <= 'F')
	{
		return c - 'A' + 0xa;
	}
}

void stringify_32(char* num, uint32_t n)
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
}

uint32_t numify_32(char* num)
{
	uint32_t n = 0;
	int state = -2;
	int i = 0;
	for(i = 0; i < 8; i++)
	{
		n = (n<<4);
		if('0' <= num[i] && num[i] <= '9')
		{
			n |= ((num[i]-'0') | 0x0f);
		}
		else if('A' <= num[i] && num[i] <= 'F')
		{
			n |= ((num[i]-'A' + 0xa) | 0x0f);
		}
		else if('a' <= num[i] && num[i] <= 'f')
		{
			n |= ((num[i]-'a' + 0xa) | 0x0f);
		}
		else
		{
			break;
		}
	}
	return n;
}

void stringify_8(char* num, uint8_t n)
{
	num[0] = hex_to_char((n>>4) & 0xf);
	num[1] = hex_to_char(n & 0xf);
}

uint32_t numify_8(char* num)
{
	return (char_to_hex(num[0])<<4) | char_to_hex(num[1]);
}

void main(void)
{
	change_sys_clock_source(HSE, 8000000);
	setup_pll_module(HSE, 72000000);
	change_sys_clock_source(PLL, 72000000);

	RCC->RCC_APB2ENR |= (1<<4);

	GPIOC->GPIO_CRH &= 0xFF0FFFFF;
	GPIOC->GPIO_CRH |= 0x00200000;
	GPIOC->GPIO_ODR |= (1 << 13);

	uart_init(9600);
	i2c_init();
	init_bldc();
	init_rc_receiver();

	uint8_t device_address = 0x68;

	while(1)
	{
		char c = uart_read_byte();

		GPIOC->GPIO_ODR &= (~(1 << 13));

		char c_addr[3];
		char c_data[3];
		char c_data_count[2];

		uint8_t addr;
		uint8_t data_count;
		uint8_t data[0xf];

		if(c == 'r')
		{
			uart_write_blocking("Please enter read address\n", 26);
			c_addr[0] = uart_read_byte();
			c_addr[1] = uart_read_byte();
			addr = numify_8(c_addr);

			uart_write_blocking("Please enter number of bytes to read\n", 37);
			c_data_count[0] = '0';
			c_data_count[1] = uart_read_byte();
			data_count = numify_8(c_data_count);

			// read data from i2c
			i2c_read(device_address, addr, &data, data_count);

			uint8_t i = 0;
			for(; i < data_count; i++)
			{
				stringify_8(c_data, data[i]);
				c_data[2] = '\n';

				uart_write_blocking("Your data : ", 12);
				uart_write_blocking(c_data, 3);
			}
		}
		else if(c == 'w')
		{
			uart_write_blocking("Please enter write address\n", 27);
			c_addr[0] = uart_read_byte();
			c_addr[1] = uart_read_byte();
			addr = numify_8(c_addr);

			uart_write_blocking("Please enter number of bytes to write\n", 38);
			c_data_count[0] = '0';
			c_data_count[1] = uart_read_byte();
			data_count = numify_8(c_data_count);

			uint8_t i = 0;
			for(; i < data_count; i++)
			{
				uart_write_blocking("Please enter data to write\n", 27);
				c_data[0] = uart_read_byte();
				c_data[1] = uart_read_byte();
				data[i] = numify_8(c_data);
			}

			// write data to i2c
			i2c_write(device_address, addr, &data, data_count);
			
			uart_write_blocking("Data written\n", 13);
		}
		else if(c == 'd')
		{
			uint8_t device_detection_address = 0;
			char c_dev[3];
			for(device_detection_address = 0; device_detection_address <= 0x7f; device_detection_address++)
			{
				if(i2c_detect(device_detection_address))
				{
					c_dev[2] = '\n';
					stringify_8(c_dev, device_detection_address);
					uart_write_blocking(c_dev, 3);
				}
			}

			uart_write_blocking("Please enter new address\n", 25);
			c_dev[0] = uart_read_byte();
			c_dev[1] = uart_read_byte();
			device_address = numify_8(c_dev);
		}
		else if(c == 'M')
		{
			uart_write_blocking("Please enter value to write to motors\n", 38);

			char c_m_val[2];
			c_m_val[0] = uart_read_byte();
			c_m_val[1] = uart_read_byte();
			uint8_t m_val = numify_8(c_m_val);

			uint32_t m = m_val * 4;
			set_motors(m, m, m, m);
		}
		else if(c == 'R')
		{
			uint32_t chan_ret[6];
			int it;
			for(it = 0; it < 6; it++){chan_ret[it] == 0xffff;}
			get_rc_channels(chan_ret);

			uart_write_blocking("Printing RC channel values\n", 27);

			char c_chan_val[11];
			c_chan_val[10] = '\n';

			for(it = 0; it < 6; it++)
			{
				stringify_32(c_chan_val, chan_ret[it]);
				uart_write_blocking(c_chan_val, 11);
			}
		}

		delay_for(500000);

		GPIOC->GPIO_ODR |= (1 << 13);

		delay_for(500000);
	}
}