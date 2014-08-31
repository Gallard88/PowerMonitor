//*****************************************************************************
//*****************************************************************************
#ifndef __EEPROM__
#define __EEPROM__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "DataFlash.h"

void EE_Init(const DF_Token *tk, uint8_t cache_pages);

uint8_t EE_Cache_Check (void);
void EE_Cache_Flush (void);

// -----------------------------------------------------
uint8_t  EE_Read1 (uint32_t addr);
uint16_t EE_Read2 (uint32_t addr);
uint32_t EE_Read4 (uint32_t addr);
void     EE_Read  (uint32_t addr, uint8_t *buf, uint16_t size);

uint8_t  EE_Read1_Default (uint32_t addr, uint8_t  def);
uint16_t EE_Read2_Default (uint32_t addr, uint16_t def);
uint32_t EE_Read4_Default (uint32_t addr, uint32_t def);

uint16_t EE_ReadString (uint32_t addr, char *dest, uint16_t size);

// -----------------------------------------------------
void EE_Write1 (uint32_t addr, uint8_t data);
void EE_Write2 (uint32_t addr, uint16_t data);
void EE_Write4 (uint32_t addr, uint32_t data);
void EE_Write  (uint32_t addr, const uint8_t *buf, uint16_t size);

void EE_WriteString (uint32_t addr, const char *buf, uint8_t size);

#ifdef __cplusplus
}
#endif
#endif

//*****************************************************************************
//*****************************************************************************
