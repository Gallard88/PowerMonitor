/****************************************************************************/
/****************************************************************************/
// ===========================================================================
/****************************************************************************/

#include <asf.h>
#include "DataFlash.h"
#include "spi_master.h"
#include "conf_spi_master.h"

#include "SysLog.h"

/****************************************************************************/
// defines for all opcodes
#define BUFFER_1_WRITE 0x84			// buffer 1 write
#define BUFFER_2_WRITE 0x87			// buffer 2 write

#define BUFFER_1_READ 0xD4			// buffer 1 read
#define BUFFER_2_READ 0xD6			// buffer 2 read

// buffer 1 to main memory page program with built-in erase
#define B1PROGWITHERASE 0x83
// buffer 2 to main memory page program with built-in erase
#define B2PROGWITHERASE 0x86

// buffer 1 to main memory page program without built-in erase
#define B1PROGNOERASE 0x88
// buffer 2 to main memory page program without built-in erase
#define B2PROGNOERASE 0x89

// main memory page program through buffer 1
#define MM_PAGE_PROG_THROUGH_B1 0x82
// main memory page program through buffer 2
#define MM_PAGE_PROG_THROUGH_B2 0x85
 
// auto page rewrite through buffer 1
#define AUTO_PAGE_REWRITE_THROUGH_B1 0x58
// auto page rewrite through buffer 2
#define AUTO_PAGE_REWRITE_THROUGH_B2 0x59
 
// main memory page compare to buffer 1
#define MM_PAGE_TO_B1_COMP 0x60
// main memory page compare to buffer 2
#define MM_PAGE_TO_B2_COMP 0x61
 
// main memory page to buffer 1 transfer
#define MM_PAGE_TO_B1_XFER 0x53
// main memory page to buffer 2 transfer
#define MM_PAGE_TO_B2_XFER 0x55

// DataFlash status register for reading density, compare status, 
// and ready/busy status
#define STATUS_REGISTER 0xD7	// was 0x57

#define MAIN_MEMORY_PAGE_READ 0xD2	// main memory page read
#define PAGE_ERASE 0x81				// erase a page
#define BLOCK_ERASE 0x50			// erase a block of pages

#define DEVICE_ID_READ		0x9F

/****************************************************************************/
/****************************************************************************/
#define MAX_PAGE_SIZE 528

struct spi_device df_spi = {
	/** Board specific select id */
	.id = SPI_DEVICE_EXAMPLE_ID
};

/****************************************************************************/
#define MASK(bit) (1<<(bit))	// bit number to bit mask
static uint32_t CS_Pin;
//static uint16_t NumChips;
static Spi *DF_Spi;
/****************************************************************************/
static void CS_On(void)	
{
	spi_select_device(DF_Spi, &df_spi);
	PIOA->PIO_CODR = CS_Pin;
}

static void CS_Off(void)
{
	spi_deselect_device(DF_Spi, &df_spi);
	PIOA->PIO_SODR = CS_Pin;
}


static void CS_Init(void)
{
	PIOA->PIO_PER = CS_Pin;
	PIOA->PIO_OER = CS_Pin;
	PIOA->PIO_SODR = CS_Pin;
	CS_Off();
}

/****************************************************************************/
void DataFlash_Init(Spi *p_spi, uint32_t cs_pin)
{
  CS_Pin = cs_pin;
  DF_Spi = p_spi;  
  CS_Init();
}

//*****************************************************************************
static int IsFlashReady (void)
{	// use page to determine which flash chip to test
	int status;

    CS_On();				// enable DataFlash
	spi_transfer(DF_Spi, STATUS_REGISTER);	// request status register
	status = spi_transfer(DF_Spi, 0xFF);			// write dummy value to start register shift
	CS_Off();				// disable DataFlash
	return (status & MASK(7));	// test the READY bit
}

//*****************************************************************************
void DFS_ReadChipID(void)
{
	uint8_t data1, data2;
	while ( !IsFlashReady() )
		;

	CS_On();
	spi_transfer(DF_Spi, DEVICE_ID_READ);
	data1 = spi_transfer(DF_Spi, 0);
	data2 = spi_transfer(DF_Spi, 0);
	CS_Off();
}

//*****************************************************************************
static int TokenCheck(const DF_Token *tk, uint16_t page)
{
  return ( page >= tk->Size ) ? -1 : 0;
}

// ****************************************************************************
void DFS_ReadPageDirect (const DF_Token *tk, uint16_t page, uint16_t offset, uint16_t size, uint8_t *dest)
{	// Main Memory Page Read (does not use either buffer)
	// 24 address bits, 4 don't care bytes
	// address = 13 bits page, 10bits byte
	// 0ppp pppp pppp ppbb bbbb bbbb
	uint16_t i;

  if ( TokenCheck(tk, page) < 0 ) {
    return;
  }

	while ( !IsFlashReady() )
		;
  page += tk->FirstPage;

	CS_On();			// delay 250ns minimum till SCK (1.8T)
	spi_transfer(DF_Spi,MAIN_MEMORY_PAGE_READ);		// command byte
	spi_transfer(DF_Spi,page >> 6);
	spi_transfer(DF_Spi,(page << 2) | (offset >> 8));
	spi_transfer(DF_Spi,(uint8_t)offset);
	spi_transfer(DF_Spi,0);				// don't care bytes
	spi_transfer(DF_Spi,0);
	spi_transfer(DF_Spi,0);
	spi_transfer(DF_Spi,0);
	// ready to start reading
	for ( i=offset; i<(offset+size); i++ )
	{
		*dest++ = spi_transfer(DF_Spi,0);			// dummy write to generate clock pulses
	}
	// all done
	CS_Off();
}

// ****************************************************************************
void DFS_ReadFlashToBuffer (const DF_Token *tk, uint16_t page, DF_Buff whichbuf)
{	// do a Main Memory Page Read to Buffer
	// 24 address bits, 32 don't care bits
	// address = 1bits 0, 13bits page, 10bits 0
	// 0ppp pppp pppp pp00 0000 0000

  if ( TokenCheck(tk, page) < 0 ) {
    return;
  }
  
	while ( !IsFlashReady() )
		;

  page += tk->FirstPage;
	CS_On();			// delay 250ns minimum till SCK (1.8T)
	spi_transfer(DF_Spi,whichbuf==Buffer1?MM_PAGE_TO_B1_XFER:MM_PAGE_TO_B2_XFER);	// Main Memory Page Read to Buffer 1 or 2
	spi_transfer(DF_Spi,page>>6);
	spi_transfer(DF_Spi,(page<<2)+0);
	spi_transfer(DF_Spi,0);
	CS_Off();					// data transfered in 250us
}

// ****************************************************************************
void DFS_WriteBufferToFlash(const DF_Token *tk, uint16_t page, DF_Buff whichbuf)
{	// do a Buffer to Main Memory Page Write with auto-erase
	// 24 address bits, 32 don't care bits
	// address = 2bits 0, 13bits page, 10bits 0
	// 0ppp pppp pppp pp00 0000 0000

  if ( TokenCheck(tk, page) < 0 ) {
    return;
  }

	while ( !IsFlashReady() )
		;

  page += tk->FirstPage;
	CS_On();			// delay 250ns minimum till SCK (1.8T)
	spi_transfer(DF_Spi,whichbuf==Buffer1?B1PROGWITHERASE:B2PROGWITHERASE);	// Buffer to Main Memory Page write
	spi_transfer(DF_Spi,page>>6);
	spi_transfer(DF_Spi,(page<<2)+0);
	spi_transfer(DF_Spi,0);
	CS_Off();					// data written in 20ms (erase+write)
}

// ****************************************************************************
void DFS_BufferRead  (const DF_Token *tk, uint16_t page, uint16_t offset, uint16_t size, DF_Buff whichbuf, uint8_t *dest)
{	// start a Read from Buffer
	// 24 address bits, 8 don't care bits
	// address = 14 bits 0, 10bits byte
	// 0000 0000 0000 00bb bbbb bbbb
	uint16_t i;

  if ( TokenCheck(tk, page) < 0 ) {
    return;
  }

	while ( !IsFlashReady() )
		;
  page += tk->FirstPage;
	CS_On();			// delay 250ns minimum till SCK (1.8T)
	spi_transfer(DF_Spi,whichbuf==Buffer1?BUFFER_1_READ:BUFFER_2_READ);		// Read Buffer 1 or 2
	spi_transfer(DF_Spi,0);
	spi_transfer(DF_Spi,offset>>8);
	spi_transfer(DF_Spi,(uint8_t)offset);
	spi_transfer(DF_Spi,0);
	// ready to start reading
	for ( i=offset; i<(offset+size); i++ )
	{
		*dest++ = spi_transfer(DF_Spi,0);			// dummy write to generate clock pulses
	}
	// all done
	CS_Off();
}

// ****************************************************************************
void DFS_BufferWrite (const DF_Token *tk, uint16_t page, uint16_t offset, uint16_t size, DF_Buff whichbuf, const uint8_t *src)
{	// do a Write to Buffer
	// 24 address bits, 8 don't care bits
	// address = 14 bits 0, 10 bits byte
	// 0000 0000 0000 00bb bbbb bbbb
	uint16_t i;

  if ( TokenCheck(tk, page) < 0 ) {
    return;
  }

	while ( !IsFlashReady() )
		;
    
  page += tk->FirstPage;
	CS_On();			// delay 250ns minimum till SCK (1.8T)
	spi_transfer(DF_Spi,whichbuf==Buffer1?BUFFER_1_WRITE:BUFFER_2_WRITE);	// Write to Buffer 1 or 2
	spi_transfer(DF_Spi,0);
	spi_transfer(DF_Spi,offset>>8);
	spi_transfer(DF_Spi,(uint8_t)offset);
	// ready to start writing
	for ( i=offset; i<(offset+size); i++ )
	{
		spi_transfer(DF_Spi,*src++);		// write the byte
	}
	// all done
	CS_Off();
}

//*****************************************************************************
//*****************************************************************************

