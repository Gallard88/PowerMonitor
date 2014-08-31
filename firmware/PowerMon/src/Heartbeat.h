/*
 * Heartbeat.h
 *
 * Created: 09-Jul-14 21:45:22
 *  Author: thomas
 */ 


#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

void Heartbeat_Tick(void);
bool Hearbeat_Run(void);
bool Is_10ms(void);
bool Is_100ms(void);
bool Is_1s(void);


#endif /* HEARTBEAT_H_ */