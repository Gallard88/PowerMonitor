/*
 * SysLog.h
 *
 * Created: 24-Aug-14 16:57:52
 *  Author: thomas
 */ 

#ifndef __SYSLOG__
#define __SYSLOG__
/* ============================================= */
typedef enum
{
	lDebug = 0,		//	Debug level message.
	lInfo = 1,		//	Informational message.
	lNotice = 2,	//	Normal, but significant, condition.
	lWarning = 3,	//	Warning conditions.
	lError = 4,		//	Error conditions.
	lCritical = 5,	//	Critical conditions.
	lAlert = 6,		//	Action must be taken immediately.
	lEmerg = 7,		//	System is unusable.
	lDiag = 0xFE,	//	Diagnostic data, fimrware, ip, start up information, etc.
	lUpdate = 0xFF	//	System setting has been updated.
} LogLevel;

/* ============================================= */
typedef void (*SYS_RespHandler)(const char *buffer);

void Syslog_Init(LogLevel logging, SYS_RespHandler fn);
void Syslog(LogLevel level, const char *module, const char *fmt, ... );

/* ============================================= */
/* ============================================= */
#endif