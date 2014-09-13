/* 	Serial Driver

	Last change:      TB 31/01/2012 2:22:32 PM
*/
#ifndef __SERIAL__
#define __SERIAL__
//=========================
int Serial_Openport(const char * port, int baudrate, int rec_chars, int block_time);
int Serial_WriteData(int portId, unsigned char *data, int length);
int Serial_Writebyte(int portId, unsigned char data);
int Serial_ReadData(int portId, unsigned char *buf, int length);
int Serial_ClosePort(int portId);
//==============================================
//==============================================
#endif

