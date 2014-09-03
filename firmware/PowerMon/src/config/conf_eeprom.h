
/*
 * conf_eeprom.h
 *
 * Created: 02-Sep-14 19:48:04
 *  Author: thomas
 */ 
#ifndef __CONF_EEPROM__
#define __CONF_EEPROM__

#ifdef __cplusplus
extern "C" {
#endif


#define EE_CH_1_VOLT		0x0002	//	uint32_t
#define EE_CH_2_VOLT		0x0006	//	uint32_t
#define EE_CH_1_VOLT_OFFSET	0x000A	//	uint32_t
#define EE_CH_2_VOLT_OFFSET	0x000E	//	uint32_t

#define EE_NEXT				0x0002


#ifdef __cplusplus
}
#endif
#endif
