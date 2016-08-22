#include "common.h"
#include <stdio.h>

volatile int PKT_TRIGGER = 1;
volatile int TRIGGER_RECEIVER = 0;

unsigned long int current_k = 0;
Simulation* simulation_instance;

volatile int recvPkt = 1;
volatile int sendPkt = 1;
volatile int socketNo;

Payload* new_payload(int seq_nr, double* y, unsigned long int sent)
{
	Payload* payload = (Payload*)malloc(sizeof(Payload));
	payload->seq_nr = seq_nr;
	payload->y = (double*)malloc(sizeof(double) * 3);
	payload->sentTime = sent;
	memcpy(payload->y, y, sizeof(double) * 3); // copy values of y to payload
	return payload;
}

void delete_payload(Payload* payload) {
	free(payload->y);
	free(payload);
}

void signal_handler(int sig) {

    if (sig == SIGINT) {
    	printf("\nSIG INT\n");
		sendPkt = 0;
        recvPkt = 0;
    }

}
