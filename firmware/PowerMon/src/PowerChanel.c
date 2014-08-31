//*****************************************************************************
//*****************************************************************************

#include "PowerChanel.h"

//*****************************************************************************
//*****************************************************************************
struct PowerChanel
{
  AD7787_Ptr adc;
  uint32_t volts;
  int32_t current;
  int32_t curr_offset;
  unsigned poll_volt:1;
};
#define CALIBRATION_CONSTANT  512

//*****************************************************************************
PowerChanel_Ptr PowerChan_Init(Spi *p_spi, uint32_t cs_pin, struct spi_device *device)
{
	PowerChanel_Ptr ptr = malloc(sizeof(struct PowerChanel));
	if ( ptr != NULL ) {
		ptr->adc = AD7787_Init(p_spi, cs_pin, device);		
		ptr->volts = 0;
		ptr->current = 0;
		ptr->poll_volt = 0;
		ptr->curr_offset = 0x800A;
	}
	return ptr;	
}

//*****************************************************************************
void PowerChan_Run(PowerChanel_Ptr ptr)
{
//	uint32_t value;
//	if ( ptr->poll_volt == 1 ) {
//		ptr->poll_volt = 0;
		ptr->volts   = AD7787_Run(ptr->adc, AD_CH_2) / 256;
//	} else {
//		ptr->poll_volt = 1;		
//		value = AD7787_Run(ptr->adc, AD_CH_1) / 256;
//		ptr->current = value - ptr->curr_offset;
//	}
}

//*****************************************************************************
uint32_t PowerChan_GetVolts(PowerChanel_Ptr ptr)
{
	return (ptr->volts * 428) / 2048;
}

//*****************************************************************************
uint32_t PowerChan_GetVoltsRaw(PowerChanel_Ptr ptr)
{
	return ptr->volts;
}

//*****************************************************************************
int32_t PowerChan_GetCurrent(PowerChanel_Ptr ptr)
{
	return (ptr->current / 4096);
}

//*****************************************************************************
//*****************************************************************************
