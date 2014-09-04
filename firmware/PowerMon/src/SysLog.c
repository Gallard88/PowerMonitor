/*
 * SysLog.c
 *
 * Created: 24-Aug-14 16:54:48
 *  Author: thomas
 */ 
#include <stdio.h>
#include <stdarg.h>
#include "Syslog.h"

/* ============================================= */
static LogLevel PrintLevel;
static SYS_RespHandler RespFn;
/* ============================================= */
void Syslog_Init(LogLevel logging, SYS_RespHandler fn)
{
  PrintLevel = logging;
  RespFn = fn;
}

/* ============================================= */
static const char *Get_WarningText(LogLevel level)
{
	switch ( level )
	{
		case lDebug:
			return "Debug";		// 	debug-level message
		case lInfo:
			return "Info";		// 	informational message
		case lNotice:
			return "Notice";	// 	normal, but significant, condition
		case lWarning:
			return "Warning";	// 	warning conditions
		case lError:
			return "Error";		// 	error conditions
		case lCritical:
			return "Critical";	// 	critical conditions
		case lAlert:
			return "Alert";		// 	action must be taken immediately
		case lEmerg:
			return "Emerg";		// 	system is unusable
		case lDiag:
			return "Diag";		// 	Diagnostic data, fimrware, start up information, etc.
		case lUpdate:
			return "Update";	// 	System setting has been updated.
		default :
			return "Unknown";
	}
}

/* ============================================= */
void Syslog(LogLevel level, const char *module, const char *fmt, ... )
{
	char buf [512];
	va_list args;

	if (( level < PrintLevel ) || ( RespFn == NULL)) {
		// we aren't going to print it, or we don't have anywhere TO print it.
		// so no point doing the rest of the processing.
		return ;	
	}

	(RespFn)(">> ");
	(RespFn)(Get_WarningText(level));
	(RespFn)(",");
	(RespFn)(module);
	(RespFn)(",");

	va_start(args,fmt);
	vsprintf(buf,fmt,args);
	va_end(args);

	(RespFn)(buf);
	(RespFn)("\r\n");
}

/* ============================================= */
/* ============================================= */
