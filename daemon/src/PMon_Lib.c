// *****************
#define _XOPEN_SOURCE

// *****************
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

// *****************
#include "ShdMem.h"
#include "PowerMonitor.h"

// *****************
PMon_t PMon_Connect(void)
{
  key_t key;
  int shmid;
  int fd;
  PMon_t ptr;

  // Make sure the file exists.
  fd = open(PMON_KEY_FILE, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
  // Only wanted to make sure that the file exists.
  close(fd);

  // Generate memory key.
  key = ftok(PMON_KEY_FILE, PMON_MEM_KEY);
  if (key  == -1) {
    perror("ftok");
    return NULL;
  }

  // connect to (and possibly create) the segment:
  if ((shmid = shmget(key, PMON_SHM_SIZE, O_RDWR)) == -1) {
    perror("shmget");
    return NULL;
  }

  // attach to the segment to get a pointer to it:
  ptr = shmat(shmid, (void *)0, 0);
  if ((char *)ptr == (char *)(-1)) {
    perror("shmat");
    return NULL;
  }

  return ptr;
}

// *****************
int PMon_isConnected(PMon_t ptr)
{
  if ( ptr == NULL ) {
    return 0;
  }
  return ptr->port_connected;
}

// *****************
float PMon_GetVoltage(PMon_t ptr, int ch)
{
  float value;

  if (( ch >= PMON_NUM_CHANELS ) || ( ptr == NULL )) {
    return -1;
  }
  pthread_mutex_lock( &ptr->access );
  value = ptr->voltage[ch];
  pthread_mutex_unlock( &ptr->access );

  return value;
}

// *****************
int PMon_GetVoltages(PMon_t ptr, float *v1, float *v2)
{
  if ( ptr == NULL ) {
    return -1;
  }
  pthread_mutex_lock( &ptr->access );
  if ( v1 != NULL ) {
    *v1 = ptr->voltage[0];
  }
  if ( v2 != NULL ) {
    *v2 = ptr->voltage[1];
  }
  pthread_mutex_unlock( &ptr->access );

  return 0;
}

// *****************
float PMon_GetCurrent(PMon_t ptr, int ch)
{
  float value;
  if (( ch >= PMON_NUM_CHANELS ) || ( ptr == NULL )) {
    return -1;
  }

  pthread_mutex_lock( &ptr->access );
  value = ptr->current[ch];
  pthread_mutex_unlock( &ptr->access );

  return value;
}

