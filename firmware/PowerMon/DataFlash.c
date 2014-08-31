/****************************************************************************/
/****************************************************************************/
// ===========================================================================
/****************************************************************************/

#include <stdint.h>
#include "DataFlash.h"

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

/****************************************************************************/
static void CS_On(int pin)
static void CS_Off(int pin)
static void CS_Init(int pin)

/****************************************************************************/
#define MASK(bit) (1<<(bit))	// bit number to bit mask
static DataFlashMap *DFMap;
static uint32_t CS_Pin;
static uint16_t NumChips;
/****************************************************************************/
void DataFlash_Init(uint32_t cs_pin)
{
  CS_Pin = cs_pin;
  CS_Init(CS_Pin);
}

//*****************************************************************************
static int IsFlashReady (uint32_t pin)
{	// use page to determine which flash chip to test
	int status;

  CS_On(pin);				// enable DataFlash
	Write_SPI(STATUS_REGISTER);	// request status register
	Write_SPI(0xFF);			// write dummy value to start register shift
	status = SPI_Read();		// read status byte
	CS_Off(pin);				// disable DataFlash
	return (status & MASK(7));	// test the READY bit
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

	while ( !IsFlashReady(CS_Pin) )
		;
  page += tk->FirstPage;

	CS_On(CS_Pin);			// delay 250ns minimum till SCK (1.8T)
	Write_SPI(MAIN_MEMORY_PAGE_READ);		// command byte
	Write_SPI(page >> 6);
	Write_SPI((page << 2) | (offset >> 8));
	Write_SPI((uint8_t)offset);
	Write_SPI(0);				// don't care bytes
	Write_SPI(0);
	Write_SPI(0);
	Write_SPI(0);
	// ready to start reading
	for ( i=offset; i<(offset+size); i++ )
	{
		SPI_Write(0);			// dummy write to generate clock pulses
		*dest++ = SPI_Read();	// get and store the byte
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
  
	while ( !IsFlashReady(CS_Pin) )
		;

  page += tk->FirstPage;
	CS_On(CS_Pin);			// delay 250ns minimum till SCK (1.8T)
	Write_SPI(whichbuf==FLASHBUF1?MM_PAGE_TO_B1_XFER:MM_PAGE_TO_B2_XFER);	// Main Memory Page Read to Buffer 1 or 2
	Write_SPI(page>>6);
	Write_SPI((page<<2)+0);
	Write_SPI(0);
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

	while ( !IsFlashReady(CS_Pin) )
		;

  page += tk->FirstPage;
	CS_On(CS_Pin);			// delay 250ns minimum till SCK (1.8T)
	Write_SPI(whichbuf==FLASHBUF1?B1PROGWITHERASE:B2PROGWITHERASE);	// Buffer to Main Memory Page write
	Write_SPI(page>>6);
	Write_SPI((page<<2)+0);
	Write_SPI(0);
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

	while ( !IsFlashReady(CS_Pin) )
		;
  page += tk->FirstPage;
	CS_On(CS_Pin);			// delay 250ns minimum till SCK (1.8T)
	Write_SPI(whichbuf==FLASHBUF1?BUFFER_1_READ:BUFFER_2_READ);		// Read Buffer 1 or 2
	Write_SPI(0);
	Write_SPI(offset>>8);
	Write_SPI((uint8_t)offset);
	Write_SPI(0);
	// ready to start reading
	for ( i=offset; i<(offset+size); i++ )
	{
		SPI_Write(0);			// dummy write to generate clock pulses
		*dest++ = SPI_Read();	// get and store the byte
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

	while ( !IsFlashReady(CS_Pin) )
		;
    
  page += tk->FirstPage;
	CS_On(CS_Pin);			// delay 250ns minimum till SCK (1.8T)
	Write_SPI(whichbuf==FLASHBUF1?BUFFER_1_WRITE:BUFFER_2_WRITE);	// Write to Buffer 1 or 2
	Write_SPI(0);
	Write_SPI(offset>>8);
	Write_SPI((uint8_t)offset);
	// ready to start writing
	for ( i=offset; i<(offset+size); i++ )
	{
		SPI_Write(*src++);		// write the byte
	}
	// all done
	CS_Off();
}

//*****************************************************************************
//*****************************************************************************

