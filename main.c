#include<regs/rcc.h>
#include<regs/gpio.h>

#include<sysclock/sysclock.h>
#include<uart/uart.h>
#include<i2c/i2c.h>

void delay_for(volatile int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

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
	/*change_sys_clock_source(HSE, 8000000);
	setup_pll_module(HSE, 72000000);
	change_sys_clock_source(PLL, 72000000);*/

	RCC->RCC_APB2ENR |= (1<<4);

	GPIOC->GPIO_CRH &= 0xFF0FFFFF;
	GPIOC->GPIO_CRH |= 0x00200000;
	GPIOC->GPIO_ODR |= (1 << 13);

	uart_init(9600);
	i2c_init();

	while(1){}

	uint8_t A = 0;
	char c_A[3];
	for(A = 0; A < 256; A++)
	{
		GPIOC->GPIO_ODR &= (~(1 << 13));
			uart_write_through_dma("Detecting for device address ", 29);
			stringify_8(c_A, A);
			c_A[2] = '\n';
			uart_write_through_dma(c_A, 3);
		uint32_t sr = i2c_detect(A);
		if(!sr)
		{
			uart_write_through_dma(c_A, 3);
		}
		else
		{
			char c_sr[11];
			stringify_32(c_sr, sr);
			c_sr[10] = '\n';
			uart_write_through_dma(c_sr, 11);
		}
		GPIOC->GPIO_ODR |= (1 << 13);
		delay_for(500000);
	}

	uint8_t device_address = 0x68;

	while(1)
	{
		char c = uart_read_byte();

		GPIOC->GPIO_ODR &= (~(1 << 13));

		char c_addr[3];
		char c_data[3];

		uint8_t addr;
		uint8_t data;

		if(c == 'r')
		{
			uart_write_through_dma("Please enter read address\n", 26);
			c_addr[0] = uart_read_byte();
			c_addr[1] = uart_read_byte();
			uint8_t addr = numify_8(c_addr);

			// read data from i2c
			i2c_read(device_address, addr, &data, 1);

			stringify_8(c_data, data);
			c_data[2] = '\n';

			uart_write_through_dma("Your data : ", 12);
			uart_write_through_dma(c_data, 3);
		}
		else if(c == 'w')
		{
			uart_write_through_dma("Please enter write address\n", 27);
			c_addr[0] = uart_read_byte();
			c_addr[1] = uart_read_byte();
			uint8_t addr = numify_8(c_addr);

			uart_write_through_dma("Please enter data to write\n", 27);
			c_data[0] = uart_read_byte();
			c_data[1] = uart_read_byte();
			uint8_t data = numify_8(c_data);

			// write data to i2c
			i2c_write(device_address, addr, &data, 1);
			
			uart_write_through_dma("Data written\n", 13);
		}

		delay_for(500000);

		GPIOC->GPIO_ODR |= (1 << 13);

		delay_for(500000);
	}
}