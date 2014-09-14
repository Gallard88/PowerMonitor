/*
 * UartBuffer.c
 *
 * Created: 24-Aug-14 16:05:57
 *  Author: thomas
 */ 
#include <asf.h>
#include "UartBuffer.h"

#define BUFFER_SIZE	256

uint8_t TxBuffer[BUFFER_SIZE];	
uint8_t RxBuffer[BUFFER_SIZE];
volatile uint8_t TxIn, TxOut;
volatile uint8_t RxIn, RxOut;	

/* ============================================= */
const sam_usart_opt_t usart_console_settings = {
	115200,
	US_MR_CHRL_8_BIT,
	US_MR_PAR_NO,
	US_MR_NBSTOP_1_BIT,
	US_MR_CHMODE_NORMAL,
	/* This field is only used in IrDA mode. */
	0
};
#define BOARD_ID_USART             USART0
#define ALL_INTERRUPT_MASK  0xffffffff
/* ============================================= */
void UartBuffer_Init(void)
{
	sysclk_enable_peripheral_clock(ID_USART0);
	usart_init_rs232(USART0, &usart_console_settings, sysclk_get_cpu_hz());
	usart_disable_interrupt(USART0, ALL_INTERRUPT_MASK);
	usart_enable_tx(USART0);
	usart_enable_rx(USART0);
	usart_enable_interrupt(USART0, US_IER_RXRDY);
	NVIC_EnableIRQ(USART0_IRQn);

	TxIn = TxOut = 0;
	RxIn = RxOut = 0;
}

/* ============================================= */
void UartBuffer_Interrupt(void)
{
	static uint32_t ul_status;
	uint32_t c;

	/* Read USART Status. */
	ul_status = usart_get_status(USART0);

	if ( ul_status & US_CSR_TXBUFE ) {
		if ( TxOut != TxIn ) {
			usart_putchar(USART0, TxBuffer[TxOut]);
			TxOut++;
		}
	} 
	if ( usart_is_rx_ready(USART0)) {
		usart_getchar(USART0, &c);
		RxBuffer[RxIn] = c;
		RxIn++;		
	}
}

/* ============================================= */
uint8_t UartBuffer_TxFull(void)
{
	return ((TxIn+1)== TxOut )? 1: 0;
}

void UartBuffer_PutChar(uint8_t c)
{
	cpu_irq_disable();
	uint16_t out_next = (TxOut+1);
	if ( out_next != TxIn ) {
		TxBuffer[TxIn] = c;
		TxIn++;
	}
	UartBuffer_Interrupt();
	cpu_irq_enable();
}

void UartBuffer_PutString(const char *s)
{
	while ( *s ) {
		UartBuffer_PutChar(*s++);
	}
	
}

void UartBuffer_PutBuffer(const uint8_t *buf, uint16_t len)
{
	uint16_t i;
	
	for ( i = 0; i < len; i ++) {
		UartBuffer_PutChar(buf[i]);
	}
}

/* ============================================= */
uint8_t UartBuffer_RxReady(void)
{
	return ( RxIn != RxOut )? 1: 0;
}

uint8_t UartBuffer_GetChar(void)
{
	uint8_t c = 0;
	cpu_irq_disable();
	if ( UartBuffer_RxReady() ) {
		c = RxBuffer[RxOut];
		RxOut++;
	}
	cpu_irq_enable();
	return c;		
}
