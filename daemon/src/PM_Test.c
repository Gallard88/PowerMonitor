

#include <stdio.h>
#include "PowerMonitor.h"

int main (int argc, char **argv)
{
  int i;

  PMon_t ptr = PMon_Connect();

  if ( ptr == NULL ) {
    printf("Failed to connect\n");
    return -1;
  }

  printf("Hardware: %s\n", PMon_isConnected(ptr)? "Connected": "Disconnected" );

  for ( i = 0; i < PMON_NUM_CHANELS; i ++ ) {
    printf("Ch %d: %2.3fV, %2.3fA\n", i +1, PMon_GetVoltage(ptr, i), PMon_GetCurrent(ptr, i));
  }

  return 0;
}

