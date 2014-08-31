/*
 * UartBuffer.h
 *
 * Created: 24-Aug-14 16:06:54
 *  Author: thomas
 */ 

#ifndef __UART_BUFFER__
#define __UART_BUFFER__

#include <stdint.h>
#include <asf.h>
#include "usart.h"

void UartBuffer_Init(void);

void UartBuffer_Interrupt(void);

uint8_t UartBuffer_TxFull(void);
void UartBuffer_PutChar(uint8_t c);
void UartBuffer_PutString(const char *s);
void UartBuffer_PutBuffer(const uint8_t *buf, uint16_t len);


uint8_t UartBuffer_RxReady(void);
uint8_t UartBuffer_GetChar(void);

#endif
