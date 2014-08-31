
#include "AD7787_Driver.h"

/****************************************************************************/
struct AD7787_Data {
	Spi *AD_Spi;
	struct spi_device *dev;
	uint32_t CS_Pin;
};

/****************************************************************************/
static void CS_On(AD7787_Ptr ptr)
{
	spi_select_device(ptr->AD_Spi, ptr->dev);
	PIOA->PIO_CODR = ptr->CS_Pin;
}

static void CS_Off(AD7787_Ptr ptr)
{
	spi_deselect_device(ptr->AD_Spi, ptr->dev);
	PIOA->PIO_SODR = ptr->CS_Pin;
}


static void CS_Init(AD7787_Ptr ptr)
{
	PIOA->PIO_PER = ptr->CS_Pin;
	PIOA->PIO_OER = ptr->CS_Pin;
	PIOA->PIO_SODR = ptr->CS_Pin;
}

/****************************************************************************/
AD7787_Ptr AD7787_Init(Spi *p_spi, uint32_t cs_pin, struct spi_device *device)
{
	AD7787_Ptr ptr = malloc(sizeof(struct AD7787_Data));
	if ( ptr ) {
		ptr->AD_Spi = p_spi;
		ptr->CS_Pin = cs_pin;
		ptr->dev = device;
		CS_Init(ptr);

		CS_On(ptr);	
		spi_transfer(ptr->AD_Spi, 0xFF);
		spi_transfer(ptr->AD_Spi, 0xFF);
		spi_transfer(ptr->AD_Spi, 0xFF);
		spi_transfer(ptr->AD_Spi, 0xFF);
		spi_transfer(ptr->AD_Spi, 0xFF);
		spi_transfer(ptr->AD_Spi, 0xFF);
		spi_transfer(ptr->AD_Spi, 0xFF);
		spi_transfer(ptr->AD_Spi, 0xFF);
		CS_Off(ptr);


	}
	return ptr;	
}

/****************************************************************************/
uint32_t AD7787_Run(AD7787_Ptr ptr, AD7787_Chanel ch)
{
	uint32_t value;
	
	CS_On(ptr);	
	spi_transfer(ptr->AD_Spi, (ch == AD_CH_1)? 0x10: 0x11);
	spi_transfer(ptr->AD_Spi, 0x82);
	
	spi_wait_miso_low(ptr->AD_Spi);

	spi_transfer(ptr->AD_Spi, (ch == AD_CH_1)? 0x38: 0x39);
	value = spi_transfer(ptr->AD_Spi, 0);
	value = spi_transfer(ptr->AD_Spi, 0) + (value *256);
	value = spi_transfer(ptr->AD_Spi, 0) + (value *256);
	CS_Off(ptr);
	
	return value;
}

