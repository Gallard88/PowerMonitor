
//*****************************************************************************
#include "eeprom.h"

#include <stdio.h>
#include <string.h>

//*****************************************************************************
//*****************************************************************************
#define EE_PAGE_SIZE  512
#define EE_CACHE_PAGE_OFFSET 9

//*****************************************************************************
typedef struct
{
	uint16_t page;
	unsigned dirty:1;
#define PAGE_COUNT_MAX	((1<<15)-1)
	unsigned count:15;
	uint8_t buf[EE_PAGE_SIZE];
} EE_CachePage;

//*****************************************************************************
static const DF_Token *EE_Token;
static EE_CachePage *ExtCache;
static uint16_t NumPages;
static uint16_t FlushDelay;
#define FLUSH_TIME		50	// ~1s

//*****************************************************************************
static void Age_Cache(void)
{
	uint16_t i;
	for ( i = 0; i < NumPages; i++ ) {
		if ( ExtCache[i].count < PAGE_COUNT_MAX )
			ExtCache[i].count++;	// stop over flow
	}
}

//*****************************************************************************
/*************************************
 *
 * ExtCache_Fill()
 * -> page, check to see if this page is currently in the cache
 * Several conditions exist.
 * If pages exists in cache:
 *   Clear count
 *   return ptr to page
 * Page not in cache:
 * Find oldest page.
 * If not dirty,
 *   Read in new page
 *   Clear count
 *   return ptr to page
 * else:
 *   Write old page to SDF BUF2
 *   Read new page into Cache
 *   Write BUF2 to Flash
 *   Clear count
 *   return ptr to page
 */
static EE_CachePage *ExtCache_Fill (uint16_t page)
{
	EE_CachePage *ptr, *oldest;
	uint16_t i, old_page;

	Age_Cache();
	// age each page with in the cache,
	// that way we can find the oldest :D

	oldest = &ExtCache[0];	// preset pointer
	for ( i = 0; i < NumPages; i++ ) {
		ptr = &ExtCache[i];
		if ( ptr->page == page ) {
      // found a match
			ptr->count = 0;
			return ptr;
		}

		// mark the oldest page
		if ( ptr->count > oldest->count ) {
			oldest = ptr;
    }
	}
	// if here then page not found
	if ( oldest->dirty == 0 )	{
    DFS_ReadPageDirect (EE_Token, page, 0, EE_PAGE_SIZE, oldest->buf);
	} else {	
  	/*
		 * We write to buf2
		 * read in new page,
		 * THEN write buf to flash
		 * this way we avoid the chip busy delay.
		 */
		old_page = oldest->page;
    DFS_BufferWrite (EE_Token, old_page, 0, EE_PAGE_SIZE, Buffer2, oldest->buf);
    DFS_ReadPageDirect (EE_Token, page, 0, EE_PAGE_SIZE, oldest->buf);
    DFS_WriteBufferToFlash(EE_Token, oldest->page, Buffer2);
		FlushDelay = FLUSH_TIME;	// so the system knows that the df is busy.
	}
	oldest->page = page;
	oldest->count = 0;
	oldest->dirty = 0;
	return oldest;
}

// -----------------------------------------------------
// -----------------------------------------------------
void EE_Init(const DF_Token *tk, uint8_t cache_pages)
{
  EE_Token = tk;
  NumPages = cache_pages;
  ExtCache = malloc(sizeof(EE_CachePage) * NumPages );
  memset(ExtCache, 0xFF, sizeof(EE_CachePage) * NumPages);
}

// -----------------------------------------------------
uint8_t EE_Cache_Check (void)
{
	EE_CachePage *ptr;
	uint16_t i;

	for ( i = 0; i < NumPages; i++ ) {// finds the first dirty page.
		ptr = &ExtCache[i];
		if ( ptr->dirty ) {
      DFS_BufferWrite (EE_Token, ptr->page, 0, EE_PAGE_SIZE, Buffer2, ptr->buf);
      DFS_WriteBufferToFlash(EE_Token, ptr->page, Buffer2);
			ptr->dirty = 0;
			return 1;
		}
	}
	return 0;
}

// -----------------------------------------------------
void EE_Cache_Flush (void) 
{
  while ( EE_Cache_Check() != 0 )
    ;
}

// -----------------------------------------------------
// -----------------------------------------------------
uint8_t  EE_Read1 (uint32_t addr) 
{
  EE_CachePage *ptr;
  
  ptr = ExtCache_Fill ( addr >> EE_CACHE_PAGE_OFFSET);
  return ptr->buf[addr & (EE_PAGE_SIZE-1)];
}
  
// -----------------------------------------------------
uint16_t EE_Read2 (uint32_t addr)
{
	return (EE_Read1(addr) | ((uint16_t)EE_Read1(addr+1) * 256) );
}

// -----------------------------------------------------
uint32_t EE_Read4 (uint32_t addr)
{
	return (EE_Read2(addr) | ((uint32_t)EE_Read2(addr+2) * 0x10000) );
}

// -----------------------------------------------------
void     EE_Read  (uint32_t addr, uint8_t *buf, uint16_t size)
{
  while ( size -- ) {
    *buf++ = EE_Read1(addr++);
  }
}

// -----------------------------------------------------
uint8_t  EE_Read1_Default (uint32_t addr, uint8_t  def)
{
  uint8_t d;
  d = EE_Read1(addr);
  return ( d == 0xFF)? def: d;
}

// -----------------------------------------------------
uint16_t EE_Read2_Default (uint32_t addr, uint16_t def)
{
  uint16_t d;
  d = EE_Read2(addr);
  return ( d == 0xFFFF)? def: d;
}

// -----------------------------------------------------
uint32_t EE_Read4_Default (uint32_t addr, uint32_t def)
{
  uint32_t d;
  d = EE_Read4(addr);
  return ( d == 0xFFFFFFFF)? def: d;
}

// -----------------------------------------------------
uint16_t EE_ReadString (uint32_t addr, char *dest, uint16_t size)
{
	uint16_t len;
	char c;

  for ( len = 0; len < (size-1); len++ ) {
     c = EE_Read1(addr ++);
     if ( c == 0xFF ) 
       c = 0;  // end of string.
     if ( dest != NULL ) {
       *dest++ = c;
     }
     if ( c == 0 ) {
       break;
     }     
  }
  *dest = 0;  // terminate the string.
  return len;
}

// -----------------------------------------------------
// -----------------------------------------------------
void EE_Write1 (uint32_t addr, uint8_t data)
{
 EE_CachePage *ptr;
  uint8_t *flash_data;

  ptr = ExtCache_Fill ( addr >> EE_CACHE_PAGE_OFFSET);
  flash_data = &ptr->buf[addr & (EE_PAGE_SIZE-1)];
  if ( *flash_data != data  )
  {
    ptr->dirty = 1;
    *flash_data = data;
  }
}
// -----------------------------------------------------
void EE_Write2 (uint32_t addr, uint16_t data)
{
  EE_Write1(addr,   data);
  EE_Write1(addr+1, data / 256);
}

// -----------------------------------------------------
void EE_Write4 (uint32_t addr, uint32_t data)
{
  EE_Write2(addr,   data);
  EE_Write2(addr+2, data > 16);
}

// -----------------------------------------------------
void EE_Write  (uint32_t addr, const uint8_t *buf, uint16_t size)
{
  while ( size -- ) {
    EE_Write1(addr++, *buf++);
  }
}

// -----------------------------------------------------
void EE_WriteString (uint32_t addr, const char *buf, uint8_t size)
{
  if ( (strlen(buf)+1) < size )
    size = strlen(buf)+1;	// if possible save the terminating null
  EE_Write(addr,(uint8_t *)buf,size);
}

// -----------------------------------------------------
// -----------------------------------------------------
