/*
 * sender.c
 *
 *  Created on: Apr 14, 2016
 *      Author: stephan
 */

#include "sender.h"
#include "common.h"
#include "config.h"
#include "ncs_simulation.h"

int send_pkt(__attribute__((unused)) void *args) {


    struct ether_hdr *eth_header;
    struct ipv4_hdr *ipv4_header;
    struct udp_hdr *udp_header;
    unsigned char *payload_ptr;
    struct rte_mbuf *send_bufs[1000];
    unsigned long int sentTime, seqNo = 1;
    int ret, index;
    struct timeval timeval;
    struct itimerspec its;
    timer_t timerid;
    struct sigevent sev;

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) < 0) {
        fprintf(stderr, "DPDK SENDER: Error creating timer");
        exit(0);
    }

    gettimeofday(&timeval, NULL);
    its.it_value.tv_sec = timeval.tv_sec + 1;
    its.it_value.tv_nsec = BASE_PERIOD_NS + SLOT_OFFSET_NS - PKT_CREATION_TIME_NS;
    its.it_interval.tv_sec = SEND_PERIOD_SEC;
    its.it_interval.tv_nsec = SEND_PERIOD_NSEC;
    if (timer_settime(timerid, TIMER_ABSTIME, &its, NULL) < 0) {

        printf("DPDK SENDER: Error setting timer - %s.\n", strerror(errno));
    }

    PKT_TRIGGER = 1;

    double* y = (double*)malloc(3* sizeof(double));
    double* prio = (double*)malloc(sizeof(double));
   	unsigned long int runIndex[NUM_INSTANCES] = {0};
	unsigned long int number_losses[NUM_INSTANCES] = {0};
	double* j_abs = (double*)malloc(sizeof(double));
	double j[NUM_INSTANCES] = {0} ;

    // While loop for running the sending process
    while(SND_PKT) {

    	// Loop for waiting for the trigger set by the interrupt
        while(PKT_TRIGGER) {

        }
        
        if (DEBUG) {
			printf("-----------------------------------------------\n");
			printf("################### run %d ###################\n", current_k);
			printf("-----------------------------------------------\n\n");
		}
			
		// Iterate over all instances, remove packets from queue and do calculation
		for (index = 0; index < NUM_INSTANCES; index++) {
			
			if (simulation_instance[index] == NULL) {
				printf("DPDK RECEIVER: Simulation %d unstable/terminated - continue\n\n", index);
				continue;
			}
			
// ################################	//
// DEQUEUE AND CALCULATE U			//
// ################################	//
		
			int gamma = 1; // Preset gamma	
			double* to_send;
			int packet_seq_no;
			Payload* p;
			bool del_p = false;
			Queue* current_queue = simulation_queues[index];			
			
			if (!queue_empty(current_queue)) {
			
				// Take current package from queue
				queue_deq(current_queue, &p);
		
				// Get sequence number and y
				packet_seq_no = p->seq_nr;
				to_send = p->y;
		
				// Remove old packets from queue
				while (!queue_empty(current_queue) && (packet_seq_no < current_k)) {
					if (DEBUG)
						printf("DPDK RECEIVER (%d): Removing packet %ld (Setpoint: %ld, Queue_size: %d)\n\n", index, packet_seq_no, current_k, queue_size(current_queue));
					delete_payload(p); // delete old payload
					queue_deq(current_queue, &p);

					packet_seq_no = p->seq_nr;
					to_send = p->y;
				}
						
				// Check for correct current packet
				if (queue_empty(current_queue) && (packet_seq_no < current_k)) {
					gamma = 0;
					if (DEBUG)
						printf("DPDK RECEIVER (%d): Setting gamma zero (Packet Nr.: %d, Setpoint: %d)\n\n", index, packet_seq_no, current_k);
				
					number_losses[index]++;
				}
				
				if (DEBUG)
					printf("\n");
					
				del_p = true;
			}
			else {
				gamma = 0;
				double arr[3];
				to_send = arr;
				
				if (DEBUG)
					printf("DPDK RECEIVER (%d): Queue Empty\n\n", index);
			
				number_losses[index]++;
			}
			
			// Calculate u
			bool res = simulation_calculate_u(simulation_instance[index], gamma, to_send);
			if (!res) {
				// Error greater 1e6! Stopping simulation
				printf("DPDK RECEIVER: Stopping instance nr. %d - Error calculating u\n\n", index);
        				simulation_delete(simulation_instance[index]);
        				simulation_instance[index] = NULL;
      	 				if (del_p == true) delete_payload(p);
        				continue;
			}
			
// ################################	//
// CALCULATE V, Y AND SEND OUT		//
// ################################	//
			
			if (simulation_instance[index] == NULL) continue;

            send_bufs[0] = rte_pktmbuf_alloc(args);
            if(send_bufs[0] == NULL) {
                printf("DPDK SENDER (%d): Error assigning mbuf pkt\n", index);
    			continue;
            }
            
            simulation_calculate_v(simulation_instance[index], &prio);

            // Set the packets headers
            eth_header = rte_pktmbuf_mtod(send_bufs[0], struct ether_hdr *);
            ipv4_header = (struct ipv4_hdr *)(rte_pktmbuf_mtod(send_bufs[0], unsigned char *) + sizeof(struct ether_hdr));
            udp_header = (struct udp_hdr *)(rte_pktmbuf_mtod(send_bufs[0], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr));
            payload_ptr = (rte_pktmbuf_mtod(send_bufs[0], unsigned char *) + sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr));

			//printf("ipv4: %d, ether: %d, udp: %d, payload: %d\n", sizeof(struct ipv4_hdr), sizeof(struct ether_hdr), sizeof(struct udp_hdr), sizeof(double) + sizeof(unsigned long int) + sizeof(double) * 3); exit(0);


            ether_addr_copy(&src_address, &eth_header->s_addr);
            ether_addr_copy(&dst_address, &eth_header->d_addr);
            eth_header->ether_type = rte_cpu_to_be_16(ETHER_TYPE_IPv4);

            ipv4_header->version_ihl = 0x45; // Version=4, IHL=5
            ipv4_header->type_of_service = DSCP << 2; // 46 is DSCP Expedited Forwarding
            ipv4_header->packet_id = 0;
            ipv4_header->fragment_offset = 0;
            ipv4_header->time_to_live = 15;
            ipv4_header->next_proto_id = 17;
            ipv4_header->src_addr = rte_cpu_to_be_32(IPv4(10,50,0,index)); // Use src address for instance identification
            ipv4_header->dst_addr = rte_cpu_to_be_32(IPv4(10,50,1,1));

            udp_header->src_port = rte_cpu_to_be_16(9000);
            udp_header->dst_port = rte_cpu_to_be_16(9000);
            udp_header->dgram_len = rte_cpu_to_be_16(sizeof(struct udp_hdr)+ PAYLOAD_SIZE);
            udp_header->dgram_cksum = 0;
            udp_header->dgram_cksum = rte_ipv4_phdr_cksum(ipv4_header, send_bufs[0]->ol_flags);

            rte_pktmbuf_append(send_bufs[0], ETHER_HDR_LEN + sizeof(struct vlan_hdr) + sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr) + PAYLOAD_SIZE);
            ipv4_header->total_length = rte_cpu_to_be_16(sizeof(struct ipv4_hdr) + sizeof(struct udp_hdr) + PAYLOAD_SIZE);
            ipv4_header->hdr_checksum = 0;
            send_bufs[0]->ol_flags = 0;
            send_bufs[0]->ol_flags |= PKT_TX_IP_CKSUM | PKT_TX_UDP_CKSUM | PKT_TX_IPV4;
            send_bufs[0]->l2_len = sizeof(struct ether_hdr);
            send_bufs[0]->l3_len = sizeof(struct ipv4_hdr);


            // send sequence number plus y
            simulation_calculate_y(simulation_instance[index], &y);

			if (DEBUG) {
				printf("DPDK SENDER (%d): SENDING Packet nr. %d with priority %f and y:\n", index, seqNo, *prio);
				printf("  %.4e\n  %.4e\n  %.4e\n\n", y[0], y[1], y[2]);
			}
			
			memcpy((char*)(payload_ptr), (void*)prio, sizeof(double));
			memcpy((char*)(payload_ptr + sizeof(double)), (void*)&seqNo, sizeof(unsigned long int));
			memcpy((char*)(payload_ptr + sizeof(double) + sizeof(unsigned long int)), y, sizeof(double) * 3); // y is 3 doubles!
			
			// Send the packet 
			ret = rte_eth_tx_burst(TX_PORT_NO, TX_QUEUE_ID, send_bufs, 1);
			if (unlikely(ret < 1)) {
				rte_pktmbuf_free(send_bufs[0]);
			}
			
// ################################	//
// CALCULATE X						//
// ################################	//

			// Calculate x
			res = simulation_calculate_x(simulation_instance[index], &j_abs);
			if (!res) {
				// Error greater 1e6! Stopping simulation
				printf("DPDK RECEIVER: Stopping instance nr. %d - Error calculating x\n\n", index);
				simulation_delete(simulation_instance[index]);
				simulation_instance[index] = NULL;
				if (del_p == true) delete_payload(p);
				continue;
			}

			if (del_p == true) delete_payload(p);
				
			j[index] += *j_abs;			
			
			runIndex[index]++;
        }
        
        seqNo++;

		// Increase k and set loop variable
		current_k++;
        PKT_TRIGGER = 1;
		
		// Check if all instances are terminated
		int num_term = 0;
		for (index = 0; index < NUM_INSTANCES; index++) {
			if (simulation_instance[index] == NULL)
				num_term++;
		}
		if (num_term == NUM_INSTANCES)  {
			printf("ALL INSTANCES TERMINATED!\n\n");
			break;
		}
		

    }

	printf("DPDK SENDER: RETURNING ...\n");
	RCV_PKT = 0;
	PKT_TRIGGER = 0;
	
	printf("STATISTICS:\n--------------------------------------\n\n");
	for (index=0; index < NUM_INSTANCES; index++) {
		printf("NCS #%d\n", index);
		printf("\tNumber of Losses: %d\n", number_losses[index]-1);
		printf("\tNumber of Iterations: %d\n", runIndex[index]);
		printf("\tPerformance J: %f\n\n", (j[index]/runIndex[index])/(SEND_PERIOD_SEC + SEND_PERIOD_USEC * pow(10, -6)));
	}


	return 0;
}
