
#ifndef __AD7787_Driver__
#define __AD7787_Driver__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <asf.h>
#include "spi_master.h"

//*****************************************************************************
typedef struct AD7787_Data *AD7787_Ptr;

typedef enum {
	AD_CH_1,
	AD_CH_2
} AD7787_Chanel;

AD7787_Ptr AD7787_Init(Spi *p_spi, uint32_t cs_pin, struct spi_device *device);
uint32_t AD7787_Run(AD7787_Ptr ptr, AD7787_Chanel ch);


//*****************************************************************************
#ifdef __cplusplus
}
#endif
#endif
