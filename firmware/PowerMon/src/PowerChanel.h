#ifndef __POWER_CHANEL__
#define __POWER_CHANEL__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "AD7787_Driver.h"

//*****************************************************************************
//*****************************************************************************
typedef struct PowerChanel *PowerChanel_Ptr;

PowerChanel_Ptr PowerChan_Init(Spi *p_spi, uint32_t cs_pin, struct spi_device *device);
void PowerChan_Run(PowerChanel_Ptr ptr);

uint32_t PowerChan_GetVolts(PowerChanel_Ptr ptr);
uint32_t PowerChan_GetVoltsRaw(PowerChanel_Ptr ptr);

int32_t PowerChan_GetCurrent(PowerChanel_Ptr ptr);

void PowerChan_SetVoltCalib(PowerChanel_Ptr ptr, uint32_t factor);
uint32_t PowerChan_GetVoltCalib(PowerChanel_Ptr ptr);

void PowerChan_SetVoltOffset(PowerChanel_Ptr ptr, int32_t offset);
int32_t PowerChan_GetVoltOffset(PowerChanel_Ptr ptr);


//*****************************************************************************
//*****************************************************************************
#ifdef __cplusplus
}
#endif
#endif


