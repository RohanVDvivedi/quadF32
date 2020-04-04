#include<rc_receiver/rc_receiver.h>

// channel_start with -1 means it is uninitialized or had an error in last read
static volatile int32_t channel_start[6] = {-1, -1, -1, -1, -1, -1};

static volatile uint32_t channel_value[6];

void init_rc_receiver()
{
	// turn on timer 5 clock and the PB10-PB15 pins ports clocks
	RCC_APB1ENR |= (1<<3);
	RCC_APB2ENR |= (1<<3);

	// setup timer 5 ticking every microsecond
	TIM2->TIM_CNT = 0;

	TIM5->TIM_CR1 |= (1<<0);

	// setup six interrupts on the port
}

void edge_interrupt_rc_channel(void)
{
	uint32_t timer_counter_value = TIM5->TIM_CNT;

	int channel_no;
	for(channel_no = 0; channel_no <= 15; channel_no++)
	{
		int channel_pin = channel_no + 10;
		// if the interrupt came from this pin
		{
			if(GPIOB->INPUT_REGISTER | (1<<channel_pin))
			{
				channel_start[channel_no] = timer_counter_value
			}
			else
			{
				if(timer_counter_value < channel_start)
				{
					timer_counter_value += 65536
				}
				channel_value[channel_no] = timer_counter_value - channel_start[channel_no];
				channel_start[channel_no] = -1;
			}
		}
	}
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

void get_rc_channels(uint32_t chan_ret[6])
{
	int iter_ch;
	for(iter_ch = 0; iter_ch < 6; iter_ch++)
	{
		chan_ret[iter_ch] = compare_and_map_and_range(channel_value[iter_ch]);
	}
}