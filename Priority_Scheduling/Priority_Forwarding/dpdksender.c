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

#include <unistd.h>

int send_pkt(__attribute__((unused)) void *args) {


	struct ether_hdr *eth_header;
	struct ipv4_hdr *ipv4_header;
	struct udp_hdr *udp_header;
	unsigned char *payload_ptr;
	unsigned long int sentTime, seqNo = 1;
	int ret, index = 0;

	double prio;

	printf("STARTING SENDER\n");

	// While loop for running the sending process
	while(SND_PKT) {

		for (index = 0; index < NUM_INSTANCES; index++) {
		
			Message* pkg;

			if (BUF_TOKENS > 0) {
				// Enough Tokens -> Send priority packet
				pthread_mutex_lock(&ts_lock);
				if (priority_empty(pqueue)) {
					printf("DPDK SENDER: Tokens (%d) there but no pkt in queue\n", BUF_TOKENS);
					pthread_mutex_unlock(&ts_lock);
					continue;
				}
				pkg = priority_dequeue(pqueue);
				pthread_mutex_unlock(&ts_lock);
				printf("DPDK SENDER: Sending Priority Packet\n");
				BUF_TOKENS--;

				if (BUF_TOKENS == 0) priority_reset(pqueue);
			}
			else {
				// Not enough tokens -> send best-effort packet
				pthread_mutex_lock(&be_lock);	
				if (fifo_empty()) {
					pthread_mutex_unlock(&be_lock);
					continue;
				}
				fifo_deq(&pkg);
				pthread_mutex_unlock(&be_lock);
				//printf("DPDK SENDER: Sending FIFO Packet\n");

				//rte_pktmbuf_free(pkg->buf);
				//delete_message(pkg);
				//continue;
			}


			struct rte_mbuf *send_bufs[1]; // 1 since only one packet per burst!
			send_bufs[0] = pkg->buf;

			eth_header = rte_pktmbuf_mtod(send_bufs[0], struct ether_hdr *);
			ipv4_header = (struct ipv4_hdr *)(rte_pktmbuf_mtod(send_bufs[0], unsigned char *) + sizeof(struct ether_hdr));
			udp_header = (struct udp_hdr *)(rte_pktmbuf_mtod(send_bufs[0], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr));
			payload_ptr = (rte_pktmbuf_mtod(send_bufs[0], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr));


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
