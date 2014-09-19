// *****************
#ifndef __POWER_MONITOR__
#define __POWER_MONITOR__
// *****************
#ifdef __cplusplus
extern "C" {
#endif

// *****************
#include <unistd.h>
#include <sys/types.h>

typedef struct PMon_Shared_Mem *PMon_t;
// *****************
PMon_t PMon_Connect(void);

#define PMON_NUM_CHANELS   2
// *****************
int PMon_isConnected(PMon_t ptr);

// *****************
float PMon_GetVoltage(PMon_t ptr, int ch);

// *****************
float PMon_GetCurrent(PMon_t ptr, int ch);

// *****************
// *****************
#ifdef __cplusplus
}
#endif
#endif

