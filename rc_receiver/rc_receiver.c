#include<rc_receiver/rc_receiver.h>

// channel_start with -1 means it is uninitialized or had an error in last read
static volatile uint32_t channel_start[6];
static volatile uint32_t channel_value[6];

static volatile uint32_t temp;

void edge_interrupt_rc_channel(void);

#define VECTOR_TABLE_ENTRY_POSITION_EXTI_10_15 		40
#define VECTOR_TABLE_BASE	((volatile uint32_t*)(0x08000040))
#define EXTI_10_15_VECTOR_TABLE_ENTRY_REFERENCE  	((volatile uint32_t*)(VECTOR_TABLE_BASE + VECTOR_TABLE_ENTRY_POSITION_EXTI_10_15))

void init_rc_receiver()
{
	// turn on timer 5 clock and the PB10-PB15 pins ports clocks
	// since we are using alternate functions i.e. interrupts from port B, turn on afio clock
	RCC->RCC_APB1ENR |= (1<<2);
	RCC->RCC_APB2ENR |= ((1<<3) | (1<<0));

	// setup timer 5 ticking every microsecond
	TIM4->TIM_CR1 = 0;
	TIM4->TIM_SMCR = 0;
	TIM4->TIM_CNT = 0;
	TIM4->TIM_PSC = 71;
	TIM4->TIM_ARR = 65535;
	TIM4->TIM_CR1 |= (1<<0);

	// setup gpio to input mode, and configure them to use as input to EXTI interrupt module
	GPIOB->GPIO_CRH = (GPIOB->GPIO_CRH & ~(0xffffff00)) | 0x88888800;
	AFIO->AFIO_EXTICR3 = 0x00001100;
	AFIO->AFIO_EXTICR4 = 0x00001111;

	// setup six interrupts on the port
	int channel_no;
	for(channel_no = 0; channel_no < 6; channel_no++)
	{
		int channel_pin = channel_no + 10;

		EXTI->EXTI_IMR  |= (1<<channel_pin);
		EXTI->EXTI_RTSR |= (1<<channel_pin);
		EXTI->EXTI_FTSR |= (1<<channel_pin);

		GPIOB->GPIO_ODR &= ~(1<<channel_pin);
	}

	// setting up NVIC controller registers
	//(*((volatile uint32_t*)(EXTI_10_15_VECTOR_TABLE_ENTRY_REFERENCE))) = ((uint32_t)(edge_interrupt_rc_channel));
	NVIC->NVIC_ISER[1] |= (1<<(VECTOR_TABLE_ENTRY_POSITION_EXTI_10_15-32));
}

void update_rc_channel(int channel_no)
{
	uint32_t timer_counter_value = TIM4->TIM_CNT;
	int channel_pin = channel_no + 10;
	if(GPIOB->GPIO_IDR & (1<<channel_pin))
	{
		channel_start[channel_no] = timer_counter_value;
	}
	else if(timer_counter_value > channel_start[channel_no])
	{
		channel_value[channel_no] = timer_counter_value - channel_start[channel_no];
	}
}

void edge_interrupt_rc_channel(void)
{
	uint32_t TIMR4_value = TIM4->TIM_CNT;
	uint32_t GPIOB_value = GPIOB->GPIO_IDR;
	uint32_t EXTI_PR_val = EXTI->EXTI_PR;

	uint32_t clear_pending = 0;

	int channel_no;
	int channel_pin;
	for(channel_no = 0; channel_no < 6; channel_no++)
	{
		channel_pin = channel_no + 10;
		if(EXTI_PR_val & (1<<channel_pin))
		{
			if(GPIOB_value & (1<<channel_pin))
			{
				channel_start[channel_no] = TIMR4_value;
			}
			else if(TIMR4_value > channel_start[channel_no])
			{
				channel_value[channel_no] = TIMR4_value - channel_start[channel_no];
			}
			clear_pending |= (1<<channel_pin);
			temp |= (1 << (channel_no * 4));
		}
	}

	EXTI->EXTI_PR |= clear_pending;
	NVIC->NVIC_ICPR[1] |= (1<<(VECTOR_TABLE_ENTRY_POSITION_EXTI_10_15-32));
}

static uint32_t compare_and_map_and_range(uint32_t value)
{
	if(value < 1000)
	{
		value = 1000;
	}
	else if(value > 2000)
	{
		value = 2000;
	}
	value = value - 1000;
	return value;
}

uint32_t get_rc_channels(uint32_t chan_ret[6])
{
	int iter_ch;
	for(iter_ch = 0; iter_ch < 6; iter_ch++)
	{
		chan_ret[iter_ch] = compare_and_map_and_range(channel_value[iter_ch]);
	}
	uint32_t ret = temp;
	temp = 0;
	return ret;
}