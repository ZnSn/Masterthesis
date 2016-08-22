#include "sender_c.h"
#include "receiver_c.h"
#include "common.h"
#include <signal.h>
#include <stdio.h>
#include <pthread.h>

int main(int argc, char* argv[]) {


	// Parse commandline arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s DstIP DstPort\n", argv[0]);
        exit(0);
    } else {
       portNo = atoi(argv[2]);
       ipAddr = argv[1];
    }
 
	signal(SIGINT, signal_handler);

	// Create simulation instance 
	double ts = SEND_PERIOD_SEC + SEND_PERIOD_USEC * pow(10, -6);
	double arrival_rate = 1.0;
	simulation_instance = simulation_new(ts, arrival_rate);

	
	// Create threads
	pthread_t sender_thread;
	pthread_t receiver_thread;

	if(pthread_create(&sender_thread, NULL, socket_send, NULL)) {
		fprintf(stderr, "Error creating sender thread\n");
		exit(0);
	}

	if(pthread_create(&receiver_thread, NULL, socket_receive, NULL)) {
		fprintf(stderr, "Error creating receiver thread\n");
		exit(0);
	}
	
	while(sendPkt || recvPkt) {
	
	}
	
	sendPkt = 0;
	recvPkt = 0;
	
	// Wait for threads to finish
	//pthread_join(sender_thread, NULL);
	//pthread_join(receiver_thread, NULL);
	
	return 1;

}
