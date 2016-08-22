/*
 * receiver.c
 *
 *  Created on: Apr 14, 2016
 *      Author: stephan
 */

#include "receiver.h"
#include "common.h"
#include "config.h"
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ncs_simulation.h"
#include "queue.h"

int receive_pkt(__attribute__((unused)) void *args) {

    unsigned long int ret, index, sampleIndex = 0;
    unsigned long int i;
    uint32_t current_ip;
    int instance_index;

    struct rte_mbuf *recd_bufs[RX_RING_SIZE];
    struct ipv4_hdr *ipv4_header;
    unsigned char *payload_ptr;
    unsigned long int seqNo;
	
	double prio;
	int num_null = 0;
	double* y = (double*)malloc(sizeof(double) * 3);

	while(RCV_PKT && (sampleIndex < NUM_SAMPLES)) {

		ret = rte_eth_rx_burst(RX_PORT_NO, RX_QUEUE_ID, recd_bufs, RX_RING_SIZE);

		if(ret > 0) {

			for (index = 0; index < ret; index++) {

				payload_ptr = (rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr));
				ipv4_header = (struct ipv4_hdr *)(rte_pktmbuf_mtod(recd_bufs[index], unsigned char *) + sizeof(struct ether_hdr));
				current_ip = rte_be_to_cpu_32(ipv4_header->src_addr);
				instance_index = current_ip & 0xFF;

				if ((current_ip & 0xFFFFFF00) != IPv4(10,50,0,0)) {
					//printf("DPDK RECEIVER: Unknown Source address (%d.%d.%d.%d) - continue\n\n", (current_ip >> 24) & 0xFF, (current_ip >> 16) & 0xFF, (current_ip >> 8) & 0xFF, (current_ip) & 0xFF);
					rte_pktmbuf_free(recd_bufs[index]);
					continue;	
				}

				// Check if instance is already unstable
				if (simulation_instance[instance_index] == NULL) {
					printf("DPDK RECEIVER: Simulation %d unstable/terminated - continue\n\n", instance_index);
					continue;
				}

				memcpy(&prio, payload_ptr, sizeof(double));
				memcpy(&seqNo, payload_ptr + sizeof(double), sizeof(unsigned long int));
				memcpy(y, payload_ptr + sizeof(double) + sizeof(unsigned long int), sizeof(double) * 3);

				if (DEBUG) {
					printf("DPDK RECEIVER (%d): RECEIVING Packet nr. %d with prio %f and y:\n", instance_index, seqNo, prio);
					printf("  %.4e\n  %.4e\n  %.4e\n\n", y[0], y[1], y[2]);
				}

				// Enqueue packet in respective queue
				queue_enq(simulation_queues[instance_index], new_payload(seqNo, y));

				if (NUM_SAMPLES >= 0) // Endless loop if num samples negative
					sampleIndex++;

				rte_pktmbuf_free(recd_bufs[index]);
			}
		}
    }

	printf("DPDK RECEIVER: RETURNING ...\n\n");

	SND_PKT = 0;
	PKT_TRIGGER = 0;
	return 0;
}

