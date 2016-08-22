/*
Opens a UDP socket on the port specified in the parameters of the commandline.
Receives the packets and prints the data rate at regular intervals

Author - Naresh Nayak
Date - 5.07.2015
*/

/* Include files */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>

#include "common.h"

void* socket_receive(void* void_ptr) {

    // Local variables
    int sampleIndex = 0, socketNo = 0, i, runIndex=0;
    struct sockaddr_in self_addr;
    char buf[PAYLOAD_SIZE];

    unsigned long int seqNo;
	unsigned long int sentTime;
	unsigned long int arrivalTime;
	unsigned long int number_losses = 0;
	double* y = (double*)malloc(sizeof(double) * 3);
	double* j = (double*)malloc(sizeof(double));
	double j_sum = 0;

	unsigned long int delay = 0;
	unsigned long int delay_sum = 0;
	unsigned long int delay_loss_sum = 0;
	unsigned long int min_delay = 999999999;
	unsigned long int max_delay = 0;

	struct timeval timeval;

    /* Open and bind a socket to listen on the aformentioned port No. */
    socketNo = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketNo < 1) {
        fprintf(stderr, "Error in opening the socket.\n");
        exit(0);
    }

    memset((char*) &self_addr, 0, sizeof(struct sockaddr_in));
    self_addr.sin_family = AF_INET;
    self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    self_addr.sin_port = htons(portNo);

    if (bind(socketNo, (struct sockaddr *)&self_addr, sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "Error binding socket - %s\n", strerror(errno));
        exit(0);
    }

	if (!simulation_instance) {
		fprintf(stderr, "Simulation instance is NULL - Returning");
		sendPkt = 0;
		recvPkt = 0;
	}
	
	// Set recv function timeout
	//struct timeval tv;
	//tv.tv_sec = RECV_PERIOD_SEC; 
	//tv.tv_usec = RECV_PERIOD_USEC; 
	//setsockopt(socketNo, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	
	if(fcntl(socketNo, F_SETFL, fcntl(socketNo, F_GETFL) | O_NONBLOCK) < 0) printf("NON BLOCKING FAILED\n");
	
	printf("STARTING RECEIVER\n%d, %d\n", socketNo, portNo);

    while(recvPkt && (sampleIndex < NUM_SAMPLES)){

        //try to receive some data        
        int ret = recv(socketNo, buf, PAYLOAD_SIZE, 0);
        
        if (ret > 0) {
        	// Received something
        
			gettimeofday(&timeval, NULL);
			arrivalTime = timeval.tv_sec * pow(10, 6) + timeval.tv_usec;

        	memcpy(&seqNo, buf, sizeof(unsigned long int));
        	memcpy(y, buf+sizeof(unsigned long int), sizeof(double) * 3);
			memcpy(&sentTime, buf+sizeof(unsigned long int) + sizeof(double) * 3, sizeof(unsigned long int));

			if (DEBUG) {
		    	printf("SOCKET RECEIVER: RECEIVING Packet nr. %d with y:\n", seqNo);
		    	printf("  %.4e\n  %.4e\n  %.4e\n\n", y[0], y[1], y[2]);    
        	}
        	
			delay = arrivalTime-sentTime;
			
        	queue_enq(new_payload(seqNo, y, delay));	
            delay_sum += delay;

			if (delay > max_delay) max_delay = delay;
			else if (delay < min_delay) min_delay = delay;

			if (NUM_SAMPLES >= 0) // Endless loop if num samples negative
                	sampleIndex++;
        }
        
        if (TRIGGER_RECEIVER) {
        
        	if (DEBUG)
				printf("DPDK RECEIVER: TRIGGERED\n\n");
				
			TRIGGER_RECEIVER = 0;
				
			int gamma = 1;
			double* to_send;
			int packet_seq_no;
			Payload* p;


			if (!queue_empty()) {
			
				// Take current package from queue
				
				queue_deq(&p);
		
				packet_seq_no = p->seq_nr;
				to_send = p->y;

				if (DEBUG) {			
					printf("SOCKET RECEIVER: DEQUEUEING Packet nr. %d with y:\n", p->seq_nr);
					printf("  %.4e\n  %.4e\n  %.4e\n\n", p->y[0], p->y[1], p->y[2]);   
		    	}
								
				// Remove old packets from queue
				while (!queue_empty() && (packet_seq_no < current_k-1)) {
					if (DEBUG)
						printf("----------------------------------------\nDPDK RECEIVER: Removing old packet %d (%ld). In queue:  %d\nPacket Delay: %d\n\n", packet_seq_no, current_k-1, queue_size(), p->sentTime);
					delete_payload(p); // delete old payload
					queue_deq(&p);
					packet_seq_no = p->seq_nr;
					to_send = p->y;
				}
		
				// Check for current packet
				if (queue_empty() && (packet_seq_no < current_k-1)) {
					gamma = 0;
					if (DEBUG)
						printf("DPDK RECEIVER: Setting gamma zero (%d, %d). Packet Delay: %d\n\n", packet_seq_no, current_k-1, p->sentTime);
					number_losses++;
					
				}

				if (DEBUG)
					printf("\n");
					
				int res = simulation_calculate_u(simulation_instance, gamma, to_send);
			
				if (!res) {
					// Error greater 1e6! Stopping simulation
					sendPkt = 0;
					recvPkt = 0;
				}

				res = simulation_calculate_x(simulation_instance,&j);
				if (!res) {
					// Error greater 1e6! Stopping simulation
					sendPkt = 0;
					recvPkt = 0;
				}

				delete_payload(p);
			}
			else {
				gamma = 0;
				double arr[3];
				to_send = arr;

				if (DEBUG)
					printf("\n");
					
				int res = simulation_calculate_u(simulation_instance, gamma, to_send);
				if (!res) {
					// Error greater 1e6! Stopping simulation
					sendPkt = 0;
					recvPkt = 0;
				}

				res = simulation_calculate_x(simulation_instance,&j);
				
				if (!res) {
					// Error greater 1e6! Stopping simulation
					sendPkt = 0;
					recvPkt = 0;
				}
			}
		j_sum += *j;
		runIndex++;
        
        }

	}

	printf("Delay: Avg. %d, Min. %d, Max. %d\n", delay_sum/sampleIndex-1, min_delay, max_delay);
	printf("Number of Losses: %d\n", number_losses);
    printf("Performance J: %f\n", (j_sum/runIndex)/(SEND_PERIOD_SEC + SEND_PERIOD_USEC * pow(10, -6)));

            
    recvPkt = 0;
    sendPkt = 0;
}



