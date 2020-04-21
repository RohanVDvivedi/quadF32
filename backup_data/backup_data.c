#include<backup_data/backup_data.h>

void backup_data_init()
{
	RCC->RCC_APB1ENR |= (3<<27);
	PWR->PWR_CR |= (1<<8);
}

// each bit in the 16 bit number weights 1/FLOAT_FACTOR times
// FLOAT_FACTOR of 328 gives us range of (approx) -99.9 to +99.9, with precission of (approx) 0.003 decimal value
#define FLOAT_FACTOR 328

int write_backup_data(uint8_t index, double data)
{
	int16_t data_to_store = ((int16_t)(data * FLOAT_FACTOR));
	if(index <= 9)
	{
		BKP->BKP_DR_1[index] = data_to_store;
	}
	else
	{
		return 0;
	}
	return 1;
}

double read_backup_data(uint8_t index)
{
	int16_t data_stored;
	if(index <= 9)
	{
		data_stored = BKP->BKP_DR_1[index];
	}
	double data = ((double)(data_stored)) / FLOAT_FACTOR;
	return data;
}