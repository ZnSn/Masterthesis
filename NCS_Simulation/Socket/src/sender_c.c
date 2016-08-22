/*
Opens a UDP socket to send packets to the destination as mentioned in commandline arguments.
Sends the packets and prints the data rate at regular intervals.

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

#include "ncs_simulation.h"
#include "common.h"


/* Globals */
static char buf[PAYLOAD_SIZE];

Simulation* simulation_instance;

unsigned long int seqNo = 1;
double* y;

struct timeval timeval;
unsigned long int sentTime = 0;

/* Signal Handler */
void send_packet_handler(int sig)
{

	if (DEBUG) {
				printf("-----------------------------------------------\n");
				printf("################### run %d ###################\n", current_k);
				printf("-----------------------------------------------\n\n");
            }

	
	simulation_calculate_y(simulation_instance, &y);

	if (DEBUG) {
		printf("SOCKET SENDER: SENDING Packet nr. %d with y:\n", seqNo);
		printf("  %.4e\n  %.4e\n  %.4e\n\n", y[0], y[1], y[2]);
	}
	
	gettimeofday(&timeval, NULL);
    sentTime = timeval.tv_sec * pow(10, 6) + timeval.tv_usec;


	memcpy(buf, (void*)&seqNo, sizeof(unsigned long int));
	memcpy(buf + sizeof(unsigned long int), y, sizeof(double) * 3); // y is 3 doubles!
	memcpy(buf + sizeof(unsigned long int) + 3*sizeof(double), (void*)&sentTime, sizeof(unsigned long int));	
		

	seqNo++;

    write(socketNo, buf, PAYLOAD_SIZE);

    current_k++;
	TRIGGER_RECEIVER = 1;

}

/* Send Function */
void* socket_send(void* void_ptr)
{

    y = (double*)malloc(3* sizeof(double));

    // Local variables
    struct hostent *dst;
    struct sockaddr_in dst_addr;
    struct itimerval interval;

    // Open a socket
    socketNo = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketNo < 0) {
        fprintf(stderr, "Error opening socket");
        exit(0);
    }  
    
    printf("SENDER SOCKET NR: %d\n", socketNo);

    // Get the destionation  
    dst = gethostbyname(ipAddr);
    if (dst == NULL) {
        fprintf(stderr, "Error getting the host.\n");
        exit(0);
    }

    // Bind the socket for the destionation
    memset((char *) &dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    memcpy((char *)&dst_addr.sin_addr.s_addr, (char *)dst->h_addr, dst->h_length);
    dst_addr.sin_port = htons(portNo);
    
    if (connect(socketNo, (struct sockaddr*)&dst_addr, sizeof(dst_addr)) < 0) {
        fprintf(stderr, "Error binding socket");
        exit(0);
    }

    signal(SIGALRM, send_packet_handler);

    interval.it_interval.tv_sec = SEND_PERIOD_SEC;
    interval.it_interval.tv_usec = SEND_PERIOD_USEC;
    interval.it_value.tv_sec = SEND_PERIOD_SEC;
    interval.it_value.tv_usec = SEND_PERIOD_USEC;
    setitimer(ITIMER_REAL, &interval, NULL);

	if (!simulation_instance) {
		fprintf(stderr, "Simulation instance is NULL - Returning");
		sendPkt = 0;
		recvPkt = 0;
	}
	
    while(sendPkt) {
            // Do Nothing
    }    
    
    recvPkt = 0;
    sendPkt = 0;

    // Return 0
    return 1;
}
