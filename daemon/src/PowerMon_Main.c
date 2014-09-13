
// *****************
#define _XOPEN_SOURCE

// *****************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>

#include "PowerMonitor.h"
#include "Serial.h"
#include "Cmds_PMon.h"

// *****************
// Options
//#define __DAEMONISE__    1

// *****************
static int Serial_fd;

#define SYSTEM_DELAY      10000   // us
static const struct timeval system_time = {0,SYSTEM_DELAY};
const char PortName[] = "/dev/ttyUSB0";

// *****************
static void System_Shutdown(void);
static void Check_Serial(int rv);

// *****************
void Connect_To_Port(void)
{
  Serial_fd = Serial_Openport(PortName, 115200, 0,0 );
  if ( Serial_fd < 0 )
    return;
  printf("Port OPened\n");
  syslog(LOG_NOTICE, "Com Port %s connected", PortName);
  PMon_SetConnected();

  // send start up commands.
	PMon_SetStartup(Serial_fd);

	// force unit to restart
  PMon_Send_Restart(Serial_fd);
}

// *****************
void Run_CtrlC(int sig)
{
  exit(0);
}

// *****************
void Setup_SignalHandler(void)
{
  struct sigaction sig;

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &Run_CtrlC;
  sigaction (SIGINT , &sig, NULL);
  sigaction (SIGTERM , &sig, NULL);
  sigaction (SIGABRT , &sig, NULL);
}

// *****************
// *****************
int main(int argc, char *argv[])
{
  int loop = 1;
  int rv;
  fd_set readfds;
  struct timeval select_time;

  if ( argc != 2 ) {
    printf("Usage: PowerMonitor port\n");
    return -1;
  }

  openlog("PowerMonitor", LOG_PID , LOG_USER );
  syslog(LOG_NOTICE, "PowerMonitor Startup");

  // register shutdown function.
  atexit(System_Shutdown);

  PMon_CreateSharedMemory();
  Setup_SignalHandler();

#ifdef __DAEMONISE__
  rv = daemon( 0, 0 );
  if ( rv < 0 ) {
    syslog(LOG_EMERG, "Daemonise failed" );
    exit(-1);
  }
#else
  printf("System starting - Debug mode\n");
#endif

  while ( loop > 0 ) {
    Connect_To_Port();
    while ( Serial_fd >= 0 ) {

      rv = PMon_Poll( Serial_fd );
      Check_Serial(rv);

      FD_ZERO(&readfds);
      FD_SET(Serial_fd, &readfds);
      select_time = system_time;

      rv = select(Serial_fd+1, &readfds, NULL, NULL, &select_time);	// wait indefinately
      if ( rv < 0 )
        continue;

      if ( FD_ISSET(Serial_fd, &readfds) ) {
        // run receiver
        char readBuf[4096];
        int length = strlen(readBuf);

        rv = read(Serial_fd, readBuf + length, sizeof(readBuf) - length);
        Check_Serial(rv);

        if ( rv > 0 ) {
          readBuf[length + rv] = 0;

          while ( rv >= 0 ) {
            rv = CmdParse_ProcessString(Cmd_Table, readBuf, Serial_fd);
          }
        }
      }
    }
    sleep(60);
  }
  return 0;
}

// *****************
void Check_Serial(int rv)
{
  if ( rv < 0 ) {
    PMon_ClearSharedMemory();
    Serial_fd = Serial_ClosePort(Serial_fd);
    syslog(LOG_EMERG, "Serial coms lost, %d", errno);
  }
}

// *****************
static void System_Shutdown(void)
{
  PMon_ClearSharedMemory();
  syslog(LOG_NOTICE, "System shutting down");
  closelog();
}

// *****************
// *****************

