/*
 * Heartbeat.c
 *
 * Created: 09-Jul-14 21:45:09
 *  Author: thomas
 */ 
#include <asf.h>
#include "Heartbeat.h"

volatile uint8_t ms_counter;
volatile uint32_t alive_counter;
uint8_t count_10ms;
uint8_t count_100ms;
uint16_t count_1s;

void Heartbeat_Tick(void)
{
	ms_counter ++;	
	alive_counter++;	
}

bool Hearbeat_Run(void)
{
	bool result = false;
	
	cpu_irq_disable();
	if ( ms_counter != 0 )
	{
		ms_counter --;
		count_1s++;
		count_10ms++;
		count_100ms++;
		result = true;
	}
	cpu_irq_enable();
	return result;
}

bool Is_10ms(void)
{
	if ( count_10ms >= 10 )
	{
		count_10ms = 0;
		return true;
	}
	return false;
}

bool Is_100ms(void)
{
	if ( count_100ms >= 100 )
	{
		count_100ms = 0;
		return true;
	}
	return false;
}

bool Is_1s(void)
{
	if ( count_1s >= 1000 )
	{
		count_1s = 0;
		return true;
	}
	return false;
}

