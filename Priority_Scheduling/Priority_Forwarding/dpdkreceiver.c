/*
 * dpdkreceiver.c
 *
 *  Created on: May 30, 2016
 *      Author: stephan
 */

#include "dpdkreceiver.h"
#include "common.h"
#include "config.h"
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

int receive_pkt_ts(__attribute__((unused)) void *args) {

	struct ether_hdr *eth_header;
	struct ipv4_hdr *ipv4_header;
	struct udp_hdr *udp_header;
	unsigned char *payload_ptr;

	unsigned long int ret, index, sampleIndex = 0;
	unsigned long int i;

	printf("STARTING TIME-SENSITIVE RECEIVER\n");
	
	int num_recvd = 0;
	uint32_t ip;
	
	struct rte_mbuf *recd_bufs[1];

	while(RCV_PKT) {
    
		ret = rte_eth_rx_burst(RX_PORT_NO_TS, RX_QUEUE_ID_TS, recd_bufs, 1);

		if(ret > 0) {

			int index = 0; // hard coded -> one packet

			eth_header = rte_pktmbuf_mtod(recd_bufs[index], struct ether_hdr *);
			ipv4_header = (struct ipv4_hdr *)(rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr));
			udp_header = (struct udp_hdr *)(rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr));
			payload_ptr = (rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr));
		
			Message* package = new_message(recd_bufs[0], 1);

			double prio;

			memcpy(&prio, payload_ptr, PRIO_SIZE);
			//memcpy(&seq, payload_ptr + PRIO_SIZE, SEQ_SIZE);
			//memcpy(y, payload_ptr + PRIO_SIZE + SEQ_SIZE, Y_SIZE);

			pthread_mutex_lock(&ts_lock);
			priority_enqueue(pqueue, package, prio);
			pthread_mutex_unlock(&ts_lock);

			num_recvd++;

			// Set the senders tokens here since now all packets are in the queue
			if (num_recvd == NUM_INSTANCES) {
				BUF_TOKENS = 2;
				num_recvd = 0;
			}

			printf("\n");

			rte_pktmbuf_free(recd_bufs[index]);
		}
    }

    printf("DPDK TS RECEIVER: RETURNING ...\n");
    SND_PKT = 0;
    return 0;
}



int receive_pkt_be(__attribute__((unused)) void *args) {

	struct ether_hdr *eth_header;
    	struct ipv4_hdr *ipv4_header;
    	struct udp_hdr *udp_header;
    	unsigned char *payload_ptr;

    	unsigned long int ret, index, sampleIndex = 0;
    	unsigned long int i;

	printf("STARTING BEST_EFFORT RECEIVER\n");
	
	uint32_t ip;

    	while(RCV_PKT) {
    
    		struct rte_mbuf *recd_bufs[1];

        	ret = rte_eth_rx_burst(RX_PORT_NO_BE, RX_QUEUE_ID_BE, recd_bufs, 1);

        	if(ret > 0) {

			int index = 0; // hard coded -> one packet

			//printf("\nReceived best-effort packet!!\n\n");

			eth_header = rte_pktmbuf_mtod(recd_bufs[index], struct ether_hdr *);
        		ipv4_header = (struct ipv4_hdr *)(rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr));
       	 		udp_header = (struct udp_hdr *)(rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr));
        		payload_ptr = (rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr));

            		//if (VLAN_PKT) {
            		//    payload_ptr += sizeof(struct vlan_hdr);
            		//}
            
             
			//printf("\n----------------\ndst-port: %d src-port %d\n----------------\n\n", rte_be_to_cpu_16(udp_header->dst_port), rte_be_to_cpu_16(udp_header->src_port));

			Message* package = new_message(recd_bufs[0], 1);


			//double prio;
			//unsigned long int seq;
			//double* y = (double*)malloc(sizeof(Y_SIZE));

			//memcpy(&prio, payload_ptr, PRIO_SIZE);
			//memcpy(&seq, payload_ptr + PRIO_SIZE, SEQ_SIZE);
			//memcpy(y, payload_ptr + PRIO_SIZE + SEQ_SIZE, Y_SIZE);
			
			//printf("Prio: %f \n", prio);
		

			//ip = rte_be_to_cpu_32(ipv4_header->dst_addr);

          		//unsigned char bytes[4];
			//bytes[0] = ip & 0xFF;
			//bytes[1] = (ip >> 8) & 0xFF;
			//bytes[2] = (ip >> 16) & 0xFF;
			//bytes[3] = (ip >> 24) & 0xFF;	
			
			
			//printf("Best-Effort Destination IP Address: %d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]); 

			/* TODO:
			Check header for type of service (port number for testing)

			If Yes:
				Deep Packet Inspection for payload = Priority
				Put in correct place in priority queue

			If No:
				Put packet in Best Effort FIFO Queue
			*/

			//if (ip == (int)IP_ADDR) {
				pthread_mutex_lock(&be_lock);
				fifo_enq(package);
				pthread_mutex_unlock(&be_lock);
			//}

			//else {
			//	priority_enqueue(pqueue, package, prio);
			//}

            		rte_pktmbuf_free(recd_bufs[index]);
        

		}
    }

    printf("DPDK BE RECEIVER: RETURNING ...\n");
    SND_PKT = 0;
    return 0;
}
