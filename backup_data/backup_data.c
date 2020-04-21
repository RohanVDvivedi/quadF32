#include<backup_data/backup_data.h>

void backup_data_init()
{
	RCC->RCC_APB1ENR |= (3<<27);
	PWR->PWR_CR |= (1<<8);
}

#define FLOAT_FACTOR 700

int write_backup_data(uint8_t index, double data)
{
	int16_t data_to_store = ((int16_t)(data * FLOAT_FACTOR));
	if(index <= 9)
	{
		BKP->BKP_DR_1[index] = data_to_store;
	}
	else if(10 <= index && index <= 42)
	{
		BKP->BKP_DR_2[index-10] = data_to_store;
	}
	else
	{
		return 0;
	}
	return 1;
}

double read_backup_data(uint8_t index)
{
	int16_t data_stored = 0;
	if(index <= 9)
	{
		data_stored = BKP->BKP_DR_1[index];
	}
	else if(10 <= index && index <= 42)
	{
		data_stored = BKP->BKP_DR_2[index-10];
	}
	double data = ((double)(data_stored)) / FLOAT_FACTOR;
	return data;
}