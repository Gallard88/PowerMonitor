#ifndef __CMD_PROC__
#define __CMD_PROC__

#ifdef __cplusplus
extern "C" {
#endif
//*****************************************************************************
typedef struct 
{
	char *buffer;
	int length;
} CmdResponse;

typedef int (*CmdHandler)(const char *argument, CmdResponse *resp);

typedef const struct 
{
	const char * const cmd;		// full command word
	CmdHandler vector;
} CmdTable;

int CP_Lookup(char *line, const CmdTable *tbl, CmdResponse *resp);


const char *CP_SkipChars(const char *ptr);
const char *CP_SkipSpace(const char *ptr);

//*****************************************************************************
#ifdef __cplusplus
}
#endif
#endif
