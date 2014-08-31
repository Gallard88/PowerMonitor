/*
 * Run.h
 *
 * Created: 30-Aug-14 08:49:07
 *  Author: thomas
 */ 
#ifndef __RUN__
#define __RUN__
#include "PowerChanel.h"

extern PowerChanel_Ptr Pri_CH, Sec_CH;


void Run_Init(void);
void Run_Line(char *line);

#endif