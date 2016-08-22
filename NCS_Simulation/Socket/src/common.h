
#ifndef _NCS_SIM_SOCKET_COMMON_H_
#define _NCS_SIM_SOCKET_COMMON_H_

#include <inttypes.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "sys/types.h" 
#include <signal.h>
#include "ncs_simulation.h"

#define PAYLOAD_SIZE sizeof(double) * 3 + 2* sizeof(unsigned long int)
#define NUM_SAMPLES 	(int)(60/(SEND_PERIOD_SEC + SEND_PERIOD_USEC * pow(10, -6))) //5000



#define SEND_PERIOD_USEC 10007.50
#define SEND_PERIOD_SEC 0

#define DEBUG 1

extern Simulation* simulation_instance;

typedef struct _Payload{
	int seq_nr;
	double* y;
	unsigned long int sentTime;
}Payload;

Payload *new_payload(int, double*, unsigned long int);
void delete_Payload(Payload*);

extern volatile int PKT_TRIGGER;
extern volatile int TRIGGER_RECEIVER;

extern unsigned long int current_k;

extern volatile sig_atomic_t recvPkt;
extern volatile sig_atomic_t sendPkt;

extern volatile int socketNo;

volatile int portNo;
volatile char* ipAddr;

void signal_handler(int sig);

#endif
