/*
	Serial Driver
	Serail.c
	T. Burns ( Thomas.burns@student.uts.edu.au )
	7 March 2011
	Last change:      TB 31/01/2012 2:22:34 PM
*/
//====================================
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "Serial.h"
//====================================
/*
Constant  Description
TIOCM_LE  DSR (data set ready/line enable)
TIOCM_DTR DTR (data terminal ready)
TIOCM_RTS RTS (request to send)
TIOCM_ST  Secondary TXD (transmit)
TIOCM_SR  Secondary RXD (receive)
TIOCM_CTS CTS (clear to send)
TIOCM_CAR DCD (data carrier detect)
TIOCM_CD  Synonym for TIOCM_CAR
TIOCM_RNG RNG (ring)
TIOCM_RI  Synonym for TIOCM_RNG
TIOCM_DSR DSR (data set ready)
*/
//====================================
int Serial_Openport(const char * port, int baudrate, int rec_chars, int block_time)
{
  struct termios options;
  int baudr;
  int port_id;

  switch(baudrate) {
  case      50 :
    baudr = B50;
    break;
  case      75 :
    baudr = B75;
    break;
  case     110 :
    baudr = B110;
    break;
  case     134 :
    baudr = B134;
    break;
  case     150 :
    baudr = B150;
    break;
  case     200 :
    baudr = B200;
    break;
  case     300 :
    baudr = B300;
    break;
  case     600 :
    baudr = B600;
    break;
  case    1200 :
    baudr = B1200;
    break;
  case    1800 :
    baudr = B1800;
    break;
  case    2400 :
    baudr = B2400;
    break;
  case    4800 :
    baudr = B4800;
    break;
  case    9600 :
    baudr = B9600;
    break;
  case   19200 :
    baudr = B19200;
    break;
  case   38400 :
    baudr = B38400;
    break;
  case   57600 :
    baudr = B57600;
    break;
  case  115200 :
    baudr = B115200;
    break;
  case  230400 :
    baudr = B230400;
    break;
  case  460800 :
    baudr = B460800;
    break;
  case  500000 :
    baudr = B500000;
    break;
  case  576000 :
    baudr = B576000;
    break;
  case  921600 :
    baudr = B921600;
    break;
  case 1000000 :
    baudr = B1000000;
    break;
  default      :
    baudr = B115200;
    break;
  }
  port_id = open(port, O_RDWR | O_NOCTTY);
  if(port_id < 0 )
    return (-1);

  fcntl(port_id, F_SETFL, 0);

  memset(&options, 0, sizeof(options));  /* clear the new struct */
  options.c_cflag = baudr | CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  options.c_cc[VMIN] = rec_chars;      // block untill n bytes are received
  options.c_cc[VTIME] = block_time;     // block untill a timer expires (n * 100 mSec.)

  if( tcsetattr(port_id, TCSANOW, &options) == -1) {
    close(port_id);
    return (-1);
  }
  return port_id;
}

//==============================================
int Serial_WriteData(int portId, unsigned char *data, int length)
{
  return (write(portId, data, length));
}

//==============================================
int Serial_Writebyte(int portId, unsigned char data)
{
  if (write(portId, &data, 1) < 0)
    return(-1);
  return(0);
}

//==============================================
int Serial_ReadData(int portId, unsigned char *buf, int length)
{
  return read(portId, buf, length);
}

//==============================================
int Serial_ClosePort(int portId)
{
  return close(portId);
}

//==============================================
//==============================================
//==============================================
//==============================================

