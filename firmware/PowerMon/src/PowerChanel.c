//*****************************************************************************
//*****************************************************************************

#include "PowerChanel.h"

//*****************************************************************************
//*****************************************************************************
struct PowerChanel
{
  AD7787_Ptr adc;
  uint32_t volts;
  uint32_t v_calib;
};
#define CALIBRATION_CONSTANT  512

//*****************************************************************************
PowerChanel_Ptr PowerChan_Init(Spi *p_spi, uint32_t cs_pin, struct spi_device *device)
{
	PowerChanel_Ptr ptr = malloc(sizeof(struct PowerChanel));
	if ( ptr != NULL ) {
		ptr->adc = AD7787_Init(p_spi, cs_pin, device);		
		ptr->volts = 0;
		ptr->v_calib = 1;
	}
	return ptr;	
}

//*****************************************************************************
//*****************************************************************************
void PowerChan_SetVoltCalib(PowerChanel_Ptr ptr, uint32_t factor)
{
	ptr->v_calib = factor;
}

/* ========================================================================= */
uint32_t PowerChan_GetVoltCalib(PowerChanel_Ptr ptr)
{
	return ptr->v_calib;
}

//*****************************************************************************
//*****************************************************************************
void PowerChan_Run(PowerChanel_Ptr ptr)
{
	ptr->volts = AD7787_Run(ptr->adc, AD_CH_2) / 256;
}

//*****************************************************************************
uint32_t PowerChan_GetVolts(PowerChanel_Ptr ptr)
{
	return (ptr->volts * ptr->v_calib) / 4096;
}

//*****************************************************************************
uint32_t PowerChan_GetVoltsRaw(PowerChanel_Ptr ptr)
{
	return ptr->volts;
}

//*****************************************************************************
int32_t PowerChan_GetCurrent(PowerChanel_Ptr ptr)
{
//	return (ptr->current / 4096);
	return 0;
}

//*****************************************************************************
//*****************************************************************************
