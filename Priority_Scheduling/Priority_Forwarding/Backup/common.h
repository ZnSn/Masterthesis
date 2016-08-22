/*
 * common.h
 *
 *  Created on: May 30, 2016
 *      Author: stephan
 */

#ifndef _NCS_SIM_DPDK_COMMON_H_
#define _NCS_SIM_DPDK_COMMON_H_

#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_ip.h>
#include <rte_udp.h>

#include <sched.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

#include "priorityqueue.h"

#define NUM_MBUFS 				8000
#define MBUF_CACHE_SIZE 		256
#define TX_RING_SIZE 			512
#define TX_BURST_SIZE 			1
#define RX_RING_SIZE 			512

#define BASE_PERIOD_NS 			pow(10,5)
#define PKT_CREATION_TIME_NS 	(1 * pow(10, 3))
#define SLOT_OFFSET_NS 			(0 * pow(10, 3))

#define Y_SIZE					sizeof(double) * 3
#define PRIO_SIZE 				sizeof(double)
#define SEQ_SIZE				sizeof(unsigned long int)
#define PAYLOAD_SIZE 			Y_SIZE + PRIO_SIZE + SEQ_SIZE

typedef enum { false, true } bool;
typedef struct rte_mempool* mempoolptr;

typedef struct _Payload{
	double priority;
	int seq_nr;
	double* y;
} Payload;

Payload *new_payload(double, int, double*);
void delete_payload(Payload*);

typedef struct _Package {

	struct ether_hdr* eth_header;
    struct ipv4_hdr* ipv4_header;
    struct udp_hdr* udp_header;
    Payload* payload;

    /*// C++ Operator Overload
    bool operator<(const Package& rhs) const
    {
        return payload->priority < rhs.payload->priority;
    }
    */

} Package;

Package* new_package(struct ether_hdr*, struct ipv4_hdr*, struct udp_hdr*, char*);
void delete_package(Package*);


typedef struct _Message {
	struct rte_mbuf* message;
	int num_packets;
} Message;

Message* new_message(struct rte_mbuf* msg, int num_packets);
void delete_message(Message* msg);

PriorityQueue* pqueue;

// Configuration parameters
extern const int TX_QUEUES;
extern const int TX_QUEUE_ID;
extern const int RX_QUEUES;
extern const int RX_QUEUE_ID;

// Operational parameters
extern volatile int RCV_PKT;
extern volatile int SND_PKT;

// IP Address define
extern uint32_t IP_ADDR;

extern struct ether_addr src_address;
extern struct ether_addr dst_address;
extern struct rte_eth_conf port_conf;

void setup_sender(mempoolptr);
void setup_receiver(mempoolptr);

#endif /* _NCS_SIM_DPDK_COMMON_H_ */

