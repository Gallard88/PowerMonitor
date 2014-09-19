//	***************************************************************************
#ifndef __CMD_PARSE__
#define __CMD_PARSE__
//	***************************************************************************
typedef  struct CmdFunc_List* CmdFuncList_t;
typedef  int (*Cmd_Callback)(int fd, char *arg) ;

struct CmdFunc {
  const char *cmd;
  Cmd_Callback func;
};

char *CmdParse_SkipSpace(char *ptr);
char *CmdParse_SkipChars(char *ptr);

int CmdParse_ProcessString(const struct CmdFunc *table, char *string, int fd);

//	***************************************************************************
//	***************************************************************************
#endif