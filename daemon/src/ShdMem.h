/*


*/
// *****************
#ifndef __SHARD_MEMORY_H__
#define __SHARD_MEMORY_H__
// *****************
#ifdef __cplusplus
extern "C" {
#endif
// *****************
#include <pthread.h>
//#include <sys/types.h>
//#include <unistd.h>

  struct PMon_Shared_Mem {
    pthread_mutex_t access;
    float voltage[2];
    float current[2];
    int port_connected;
  };

// *****************
#define PMON_SHM_SIZE		sizeof(struct PMon_Shared_Mem)
#define PMON_KEY_FILE	"/tmp/PowerMon.shmem"
#define PMON_MEM_KEY 'P'

// *****************
#ifdef __cplusplus
}
#endif
#endif

