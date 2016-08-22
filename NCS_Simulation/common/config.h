/*
 * config.h

 *
 *  Created on: Apr 28, 2016
 *      Author: Stephan Zinkler

 * This file defines all configurabale parameters used in the dpdk application
 
 */

#include <math.h>

#ifndef _DPDK_SIM_CONFIG_H_
#define _DPDK_SIM_CONFIG_H_

// En-/Disable terminal messages
#define DEBUG 1

// Number of Simulation Instances
#define NUM_INSTANCES	1

// Period for sending packets
#define SEND_PERIOD_SEC 	0
#define SEND_PERIOD_USEC 	250000
#define SEND_PERIOD_NSEC 	(SEND_PERIOD_USEC * pow(10, 3))

// Number of packets to send
#define NUM_SAMPLES 		-1

// VLAN/DSCP onfigurations
#define VLAN_PKT   			0
#define VLAN_TAG   			10
#define VLAN_PRIO  			7
#define DSCP       			0

// DPDK port configuration
#define TX_PORT_NO 			0
#define RX_PORT_NO			1
#define TX_CORE_NR			3
#define RX_CORE_NR			2


#endif /* _DPDK_SIM_CONFIG_H_ */
