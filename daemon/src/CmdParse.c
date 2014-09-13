//	***************************************************************************
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "CmdParse.h"
//	***************************************************************************
//	***************************************************************************
char *CmdParse_SkipSpace(char *ptr)
{
  // Skip to next non-blank
  while(isspace(*ptr))
    ++ptr;
  return ptr;
}

//	***************************************************************************
char *CmdParse_SkipChars(char *ptr)
{
  // Skip to next blank
//	while(isalnum(*ptr))
  while(*ptr && !isspace(*ptr))
    ++ptr;
  return ptr;
}


//	***************************************************************************
//	***************************************************************************
int CmdParse_ProcessString(const struct CmdFunc *table, char *string, int fd)
{
  struct CmdFunc *element;
  char *cmd;
  char *arg;
  char *end;
  int rv = -1;

  end = strchr(string, '\n');
  if ( end != NULL ) {
    *end = 0; // terminate this string.
    end++;
    cmd = CmdParse_SkipSpace(string);	// remove any preceding white space.

    arg = strchr(string, ':');
    if ( arg ) {
      *arg++ = 0;
      if ( *arg == 0 )
        arg = NULL;
    }
    if ( strlen(cmd) > 0 ) {
      element = (struct CmdFunc *)table;
      while ( element->cmd ) {
        // scan list
        if ( strncmp(element->cmd, cmd, strlen(element->cmd)) == 0 ) {
          rv = (*element->func)(fd, arg);
          break;
        }
        element++;
      }
    }
    memmove(string, end, strlen(end)+1);
  }
  return rv;
}

//	***************************************************************************
//	***************************************************************************
