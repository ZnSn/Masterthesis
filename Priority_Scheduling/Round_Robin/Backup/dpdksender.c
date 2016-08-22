/*
 * dpdksender.c
 *
 *  Created on: May 30, 2016
 *      Author: stephan
 */

#include "dpdksender.h"
#include "common.h"
#include "config.h"

#include "priorityqueue.h"

int send_pkt(__attribute__((unused)) void *args) {


    struct ether_hdr *eth_header;
    struct ipv4_hdr *ipv4_header;
    struct udp_hdr *udp_header;
    unsigned char *payload_ptr;
    struct rte_mbuf *send_bufs[1000];
    unsigned long int sentTime, seqNo = 1;
    int ret, index;




	printf("STARTING SENDER\n");

    // While loop for running the sending process
    while(SND_PKT) {

    	for (index = 0; index < FIFO_WEIGHT + PRIO_WEIGHT; index++) {

    		send_bufs[index] = rte_pktmbuf_alloc(args);
			if(send_bufs[index] == NULL) {
				printf("DPDK SENDER: Error assigning mbuf pkt\n");
				SND_PKT = 0;
				RCV_PKT = 0;
				break;
			}
						
			Package* pkg;
			if (index < FIFO_WEIGHT) { // Handling FIFO queue packet
				if (fifo_empty()) {
					rte_pktmbuf_free(send_bufs[index]);
					continue;
				}
				fifo_deq(&pkg);
				printf("DPDK SENDER: Sending FIFO Packet\n");
			}
			else { // Handling priority queue packet
				if (priority_empty(pqueue)) {
					rte_pktmbuf_free(send_bufs[index]);
					continue;
				}
				pkg = priority_dequeue(pqueue);
				printf("DPDK SENDER: Sending Priority Packet\n");
			}


    		eth_header = pkg->eth_header;
    		ipv4_header = pkg->ipv4_header;
    		udp_header = pkg->udp_header;
    		
    		ipv4_header->dst_addr = rte_cpu_to_be_32(IPv4(10,50,1,1));
    		

    		payload_ptr = (rte_pktmbuf_mtod(send_bufs[index], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr));

    		Payload* payload = pkg->payload;
    		double prio = payload->priority;
    		unsigned long int seq = payload->seq_nr;
    		double* y = payload->y;


    		rte_pktmbuf_append(send_bufs[index], ETHER_HDR_LEN + sizeof(struct vlan_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr) + PAYLOAD_SIZE);
			send_bufs[index]->ol_flags = 0;
			send_bufs[index]->ol_flags |= PKT_TX_IP_CKSUM | PKT_TX_UDP_CKSUM | PKT_TX_IPV4;
			send_bufs[index]->l2_len = sizeof(struct ether_hdr);
			send_bufs[index]->l3_len = sizeof(struct ipv4_hdr);


			// If VLAN is defined, add respective fields to the packet
			if (VLAN_PKT)
			{
				send_bufs[index]->l2_len += sizeof(struct vlan_hdr);
				send_bufs[index]->vlan_tci = VLAN_PRIO << 13 | 0x00 | VLAN_TAG;
				rte_vlan_insert(&send_bufs[index]);
			}
		
			memcpy(payload_ptr, &prio, PRIO_SIZE);
			memcpy(payload_ptr + PRIO_SIZE, &seq, SEQ_SIZE);
			memcpy(payload_ptr + PRIO_SIZE + SEQ_SIZE, y, Y_SIZE);
			
			unsigned char bytes[4];
			bytes[0] = ipv4_header->dst_addr & 0xFF;
			bytes[1] = (ipv4_header->dst_addr >> 8) & 0xFF;
			bytes[2] = (ipv4_header->dst_addr >> 16) & 0xFF;
			bytes[3] = (ipv4_header->dst_addr >> 24) & 0xFF;	
			
			printf("Forwarding to IP Address: %d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]); 


			ret = rte_eth_tx_burst(TX_PORT_NO, TX_QUEUE_ID, send_bufs, 1); // TX_BURST_SIZE instead of 1 for BURST
			if (unlikely(ret < 1)) {
				for (index = ret; index < 1; index++)
					rte_pktmbuf_free(send_bufs[index]);
			}
    	}
    }

	printf("DPDK SENDER: RETURNING ...\n");
	RCV_PKT = 0;

	return 0;
}
