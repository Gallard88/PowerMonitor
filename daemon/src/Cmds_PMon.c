
// *****************
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
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

// *****************
#include "ShdMem.h"
#include "PowerMonitor.h"
#include "Cmds_PMon.h"

// *****************
static PMon_t PM_ptr;

// *****************
void PMon_CreateSharedMemory( void )
{
  key_t key;
  int shmid;
  int fd;

  // Make sure the file exists.
  fd = open(PMON_KEY_FILE, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
  /* Only wanted to make sure that the file exists. */
  close(fd);

  // Generate memory key. */
  key = ftok(PMON_KEY_FILE, PMON_MEM_KEY);
  if (key  == -1) {
    syslog(LOG_EMERG, "ftok() failed");
    perror("ftok() failed");
    exit(1);
  }

  // connect to (and possibly create) the segment:
  if ((shmid = shmget(key, PMON_SHM_SIZE, 0777 | IPC_CREAT)) == -1) {
    syslog(LOG_EMERG, "shmget()");
    perror("shmget() - failed");
    exit(1);
  }

  // attach to the segment to get a pointer to it:
  PM_ptr = shmat(shmid, (void *)0, 0);
  if ((char *)PM_ptr == (char *)(-1)) {
    syslog(LOG_EMERG, "shmat()");
    perror("shmat() - failed");
    exit(1);
  }

  memset(PM_ptr, 0, sizeof(struct PMon_Shared_Mem));
  pthread_mutex_init(&PM_ptr->access, NULL);
}

// *****************
void PMon_ClearSharedMemory(void)
{
  PM_ptr->data_ready = 0;
  PM_ptr->port_connected = 0;
  PM_ptr->voltage[0] = 0;
  PM_ptr->voltage[1] = 0;
  PM_ptr->current[0] = 0;
  PM_ptr->current[1] = 0;

  // detach from the segment:
  if (shmdt(PM_ptr) == -1)
    syslog(LOG_EMERG, "shmdt()");
}

// *****************
void PMon_SetConnected(void)
{
  PM_ptr->port_connected = 1;
}

// *****************
void PMon_SetDisconnected(void)
{
  PM_ptr->port_connected = 0;
}

// *****************
static const char *Poll_Msgs[2] = {
	"volt pri\r\n",
	"volt sec\r\n"
};

// *****************
int PMon_Poll(int fd)
{
  static int last, state;
  int current = time(NULL);
  if (( current - last) > 8 ) {
    state++;
    if (state == 1 )
      state = 0;
    return write (fd, Poll_Msgs[state], strlen(Poll_Msgs[state]));
  }
  return 0;
}

// *****************
int PMon_SetStartup(int fd)
{
  char cmd[100];

  sprintf(cmd, "ver\r\n");
  return write (fd, cmd, strlen(cmd));
}

// *****************
static const char Restart_Cmd[] = "restart\r\n";
// *****************
int PMon_Send_Restart(int fd)
{
  return write(fd, Restart_Cmd, strlen(Restart_Cmd));
}

// *****************
static int Receive_Current(int fd, char *buf)
{
  char *ptr;

  if ( buf ) {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PM_ptr->access );

    PM_ptr->current[0] = (float)atoi(ptr) / 1000.0;

    pthread_mutex_unlock( &PM_ptr->access );
  }
  return 1;
}

// *****************
static int Receive_Voltage(int fd, char *buf)
{
  char *ptr;
  float volt;
  int ch;

  if ( buf ) {
//    ptr = CmdParse_SkipSpace(buf);
    ptr = buf;
    printf("Volt: %s\n", buf);
    if ( strncmp(ptr,"pri", 3) == 0) {
      ch = 0;
      printf("Pri\r\n");
    } else if ( strncmp(ptr,"sec", 3) == 0) {
      ch = 1;
      printf("Sec\r\n");
    } else {
      printf("???\r\n");
      return 0;
    }

    volt = (float)atoi(ptr) / 1000.0;
    printf("V %d, %f\r\n", ch, volt);

    pthread_mutex_lock( &PM_ptr->access );
    PM_ptr->voltage[ch] = volt;
    pthread_mutex_unlock( &PM_ptr->access );
  }
  return 1;
}

// *****************
const struct CmdFunc Cmd_Table[] = {
  { "volt",	&Receive_Voltage },
  { "current",	&Receive_Current },
  { NULL,	NULL }
};

// *****************
// *****************

