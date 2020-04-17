#include<d_serializer/d_serializer.h>

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