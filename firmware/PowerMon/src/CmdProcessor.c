
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <string.h>
#include <ctype.h>

#include "CmdProcessor.h"

/******************************************************************************/
const char *CP_SkipChars(const char *ptr)
{	// Skip to next blank
	while((*ptr != 0) && ( !isspace(*ptr)))
		++ptr;
	return ptr;
}

/******************************************************************************/
const char *CP_SkipSpace(const char *ptr)
{	// Skip to next blank
	while(*ptr && isspace(*ptr))
		++ptr;
	return ptr;
}

/******************************************************************************/
int CP_Lookup(char *line, const CmdTable *tbl, CmdResponse *resp)
{
	char * arg;
  
	// clear resp data.
	if ( resp->buffer ) {		
		resp->buffer[0] = 0;
	}
	while ( tbl->cmd != NULL ) { // scan a command table  

		if ( strncmp(tbl->cmd, line, strlen(tbl->cmd)) == 0 ) {  // skip to end of cmd   
			arg = (char*)CP_SkipChars(line);   // leave ptr at first char after command (might be nul)
			return (tbl->vector)(arg, resp);    // execute command
		}
		tbl++;
	}
	return -1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
