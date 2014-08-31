/*
 * SysLog.c
 *
 * Created: 24-Aug-14 16:54:48
 *  Author: thomas
 */ 

#include "UartBuffer.h"
#include "Syslog.h"

#include <stdio.h>

/* ============================================= */
/* ============================================= */

void Syslog(const char *file, const char *msg)
{
	UartBuffer_PutChar('[');
	UartBuffer_PutString(file);
	UartBuffer_PutString("], ");
	UartBuffer_PutString(msg);
	UartBuffer_PutString("\r\n");
}


/* ============================================= */
/* ============================================= */
