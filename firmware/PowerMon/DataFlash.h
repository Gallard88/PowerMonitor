/* ************************************************************************ */
/* ************************************************************************ */
#ifndef __DATAFLASH__
#define __DATAFLASH__

#ifdef __cplusplus
extern "C" {
#endif
/* 
 *  Data flash driver for AT45DBXX1 chips.
 *
 *
 *
 *
 */
//*****************************************************************************
#define FLASHBUFSIZE 528			// only used by monitor


typedef struct
{
  uint16_t FirstPage;
  uint16_t Size;
} DF_Token;

typedef enum {
  Buffer1,
  Buffer2
} DF_Buff;

void DataFlash_Init(uint32_t cs_pin);
void DFS_ReadPageDirect (const DF_Token *tk, uint16_t page, uint16_t offset, uint16_t size, uint8_t *dest);
void DFS_ReadFlashToBuffer (const DF_Token *tk, uint16_t page, DF_Buff whichbuf);
void DFS_WriteBufferToFlash(const DF_Token *tk, uint16_t page, DF_Buff whichbuf);
void DFS_BufferRead  (const DF_Token *tk, uint16_t page, uint16_t offset, uint16_t size, DF_Buff whichbuf, uint8_t *dest);
void DFS_BufferWrite (const DF_Token *tk, uint16_t page, uint16_t offset, uint16_t size, DF_Buff whichbuf, const uint8_t *src);



#define FLASH_DEVID_321		0x27	// values read from chip ID register
#define FLASH_DEVID_161		0x26	// values read from chip ID register
#define FLASH_DEVID_081		0x25

#ifdef __cplusplus
}
#endif
#endif
