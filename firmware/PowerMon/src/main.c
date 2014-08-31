/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <string.h>
#include "UartBuffer.h"
#include "CmdProcessor.h"
#include "SysLog.h"
#include "DataFlash.h"
#include "Heartbeat.h"
#include "spi_master.h"
#include "PowerChanel.h"
#include "Run.h"
#include "eeprom.h"


#define BOARD_ID_USART             ID_USART0
#define BOARD_USART                USART0
#define BOARD_USART_BAUDRATE       115200
#define USART_IRQn                 USART0_IRQn
#define ALL_INTERRUPT_MASK  0xffffffff

struct spi_device SPI_DEVICE_EXAMPLE = {
	/** Board specific select id */
	.id = SPI_DEVICE_EXAMPLE_ID
};

PowerChanel_Ptr Pri_CH, Sec_CH;
const DF_Token EE_Token = { 0, 4};

/* ============================================= */
static int Cmd_Version(const char *argument, CmdResponse *resp)
{
	strcpy(resp->buffer,"Ver: Power Monitor V0.1 "__DATE__" "__TIME__"\r\n");
	return 0;
}

/* ============================================= */
static int Cmd_Current(const char *argument, CmdResponse *resp)
{
	uint8_t ch;
	
	argument = CP_SkipSpace(argument);

	if ( strncmp("sec", argument, 3) == 0 )
		ch = 2;
	else if ( strncmp("pri", argument, 3) == 0)
		ch = 1;
	else  {
		return -1;
	}
	
	sprintf(resp->buffer,"Current %s: %ld mA\r\n", (ch == 2)?"Sec":"Pri", PowerChan_GetCurrent((ch==2)?Sec_CH:Pri_CH));
	return 0;
}

/* ============================================= */
static int Cmd_Voltage(const char *argument, CmdResponse *resp)
{
	uint8_t ch;
	
	argument = CP_SkipSpace(argument);

	if ( strncmp("sec", argument, 3) == 0 )
	ch = 2;
	else if ( strncmp("pri", argument, 3) == 0)
	ch = 1;
	else  {
		return -1;
	}
	
	sprintf(resp->buffer,"Voltage %s: %lu mV\r\n", (ch == 2)?"Sec":"Pri", PowerChan_GetVolts((ch==2)?Sec_CH:Pri_CH));
	return 0;
}

/* ============================================= */
static int Cmd_VoltageRaw(const char *argument, CmdResponse *resp)
{
	uint8_t ch;
	
	argument = CP_SkipSpace(argument);

	if ( strncmp("sec", argument, 3) == 0 )
	ch = 2;
	else if ( strncmp("pri", argument, 3) == 0)
	ch = 1;
	else  {
		return -1;
	}
	
	sprintf(resp->buffer,"Voltage %s: %08lX\r\n", (ch == 2)?"Sec":"Pri", PowerChan_GetVoltsRaw((ch==2)?Sec_CH:Pri_CH));
	return 0;
}

/* ============================================= */
static int Cmd_CurrentRaw(const char *argument, CmdResponse *resp)
{
	uint8_t ch;
	
	argument = CP_SkipSpace(argument);

	if ( strncmp("sec", argument, 3) == 0 )
	ch = 2;
	else if ( strncmp("pri", argument, 3) == 0)
	ch = 1;
	else  {
		return -1;
	}
	
	sprintf(resp->buffer,"Current %s: %08lX\r\n", (ch == 2)?"Sec":"Pri", PowerChan_GetCurrent((ch==2)?Sec_CH:Pri_CH));
	return 0;
}

/* ============================================= */
static int Cmd_Restart(const char *argument, CmdResponse *resp)
{
	UartBuffer_PutString("System Restarting\r\n");
	while(1);
	return 0;
}

/* ============================================= */
/* ============================================= */
static const CmdTable SystemCommands[] = {
	{ "restart",	Cmd_Restart		},
	{ "current",	Cmd_Current		},
	{ "volt",		Cmd_Voltage		},
	{ "raw",		Cmd_VoltageRaw	},
	{ "craw",		Cmd_CurrentRaw	},
	{ "ver",		Cmd_Version		},
	{ NULL,			NULL			}
};

/* ============================================= */
void USART0_Handler(void)
{
	UartBuffer_Interrupt();
}

void SysTick_Handler(void)
{
	Heartbeat_Tick();
}

/* ============================================= */
#define RX_SIZE	256
static char MsgBuf[RX_SIZE];
static char RespBuf[RX_SIZE];
static CmdResponse Resp;
static uint16_t RxSize;

/* ============================================= */
static void Run_Config(void)
{
	while ( UartBuffer_RxReady() ) {
		uint8_t c = UartBuffer_GetChar();
		UartBuffer_PutChar(c);
		if ( c == '\r' || c == '\n' ) {
			if ( RxSize ) {
				UartBuffer_PutChar('\n');			
				CP_Lookup(MsgBuf, SystemCommands, &Resp);
				UartBuffer_PutString(Resp.buffer);			
				Run_Line(MsgBuf);
				RxSize = 0;
			}
		}
		else
		if ( RxSize < RX_SIZE ) {
			MsgBuf[RxSize++] = c;
			MsgBuf[RxSize] = 0;
		}
	}
}

/* ============================================= */
int main (void)
{
	int state = 0;
	
	sysclk_init();
	SysTick_Config(SystemCoreClock / 1000);
	board_init();
	
	delay_init(SystemCoreClock );
	wdt_restart( WDT);
	delay_s(1);
	wdt_restart( WDT);
	delay_s(1);
	wdt_restart( WDT);
	delay_s(1);
	wdt_restart( WDT);
	delay_s(1);
	wdt_restart( WDT);
	delay_s(1);
	
	Resp.buffer = RespBuf;
	Resp.length = sizeof(RespBuf);	
	
	UartBuffer_Init();
	
	spi_master_init(SPI);
	spi_master_setup_device(SPI, &SPI_DEVICE_EXAMPLE,SPI_MODE_0, 1000000, 0);
	spi_enable(SPI);

	
	DataFlash_Init(SPI, PIO_PA17);
	Pri_CH = PowerChan_Init(SPI, PIO_PA16, &SPI_DEVICE_EXAMPLE);
	Sec_CH = PowerChan_Init(SPI, PIO_PA15, &SPI_DEVICE_EXAMPLE);

	DFS_ReadChipID();
	EE_Init( &EE_Token, 4);

	UartBuffer_PutString("\r\nSystem Startup\r\n");
	while ( 1 )
	{
		Hearbeat_Run();
		UartBuffer_Interrupt(); // for some reason the interrupt isn't firing
								// keep this here until it does.
		Run_Config();
		
		if ( Is_1s() ) 	{
			wdt_restart( WDT);
			PowerChan_Run(Pri_CH);
			PowerChan_Run(Sec_CH);
			if ( state ) {
				PIOA->PIO_SODR = PIO_PA8;
				PIOA->PIO_CODR = PIO_PA9;
				PIOA->PIO_SODR = PIO_PA10;
				state = 0;
			} else {
				PIOA->PIO_CODR = PIO_PA8;
				PIOA->PIO_SODR = PIO_PA9;
				PIOA->PIO_CODR = PIO_PA10;
				state = 1;
			}
		}
	}
	return 0;
}
