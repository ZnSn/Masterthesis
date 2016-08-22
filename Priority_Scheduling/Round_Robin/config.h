/*
 * config.h
 *
 *  Created on: May 30, 2016
 *      Author: stephan
 */

#include <math.h>

#ifndef _DPDK_SIM_CONFIG_H_
#define _DPDK_SIM_CONFIG_H_

// En-/Disable terminal messages
#define DEBUG 1

// Period for sending packets
#define SEND_PERIOD_SEC 	0
#define SEND_PERIOD_USEC 	10000
#define SEND_PERIOD_NSEC 	(SEND_PERIOD_USEC * pow(10, 3))

// Number of packets to send
#define NUM_SAMPLES 		100
#define NUM_INSTANCES		6
#define NUM_TOKENS			2

// VLAN/DSCP onfigurations
#define VLAN_PKT   			0
#define VLAN_TAG   			10
#define VLAN_PRIO  			7
#define DSCP       			0

// DPDK port configuration
#define TX_PORT_NO 		1
#define RX_PORT_NO_TS		0 		// TS = Time-Sensitive
#define RX_PORT_NO_BE		2		// BE = Best-Effort
#define TX_CORE_NR		4
#define RX_CORE_NR_TS		2
#define RX_CORE_NR_BE		3

#define PRIO_WEIGHT 		5
#define FIFO_WEIGHT		1

// EXPECTED IP ADDRESS CONFIGURATION
#define IP_0	1
#define IP_1 	1
#define IP_2	60
#define IP_3	10

#endif /* _DPDK_SIM_CONFIG_H_ */
