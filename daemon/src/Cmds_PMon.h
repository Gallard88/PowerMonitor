#ifndef __PMON_CMDS__
#define __PMON_CMDS__

// *****************
// *****************
#include "CmdParse.h"

extern const struct CmdFunc Cmd_Table[];

// *****************
void PMon_CreateSharedMemory(void);
void PMon_ClearSharedMemory(void);

// *****************
void PMon_SetConnected(void);
void PMon_SetDisconnected(void);

int PMon_Poll(int fd);

int PMon_SetStartup(int fd);
int PMon_Send_Restart(int fd);

// *****************
// *****************
#endif
