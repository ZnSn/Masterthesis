/*
 * common.cpp
 *
 *  Created on: May 30, 2016
 *      Author: stephan
 */

#include "common.h"
#include "config.h"

#include "dpdksender.h"
#include "dpdkreceiver.h"

// ########################################
// DPDK QUEUE PARAMETERS
// ########################################

const int TX_QUEUES = 1;
const int TX_QUEUE_ID = 0;
const int RX_QUEUES = 1;
const int RX_QUEUE_ID_TS = 0;
const int RX_QUEUE_ID_BE = 0;

// ########################################
// DPDK OPERATIONAL PARAMETERS
// ########################################

volatile int RCV_PKT = 1;
volatile int SND_PKT = 1;
volatile int BUF_TOKENS = 0;

// ########################################
// EXPECTED IP ADDRESS
// ########################################

uint32_t IP_ADDR =  (IP_0 | (IP_1 << 8) | (IP_2 << 16) | (IP_3 << 24));



Message* new_message(struct rte_mbuf* msg, int num_packets){
	Message* message = (Message*)malloc(sizeof(Message));
	message->buf = msg;
	message->num_packets = num_packets;
	return message;
}

void delete_message(Message* msg) {
	free(msg);
}


// ########################################
// DPDK STRUCTS
// ########################################

struct ether_addr src_address = { .addr_bytes = {00,11,22,33,44,55} };
struct ether_addr dst_address = { .addr_bytes = {66,77,88,99,0xAA,0xBB} };
struct rte_eth_conf port_conf = {
    .rxmode = {
        .mq_mode        = ETH_MQ_RX_RSS,
        .max_rx_pkt_len = ETHER_MAX_LEN,
        .split_hdr_size = 0,
        .header_split   = 0, /**< Header Split disabled */
        .hw_ip_checksum = 1, /**< IP checksum offload enabled */
        .hw_vlan_filter = 0, /**< VLAN filtering disabled */
        .jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
        .hw_strip_crc   = 0, /**< CRC stripped by hardware */
    },
    .rx_adv_conf = {
        .rss_conf = {
            .rss_key = NULL,
            .rss_hf = ETH_RSS_PROTO_MASK,
        },
    },
    .txmode = {
        .mq_mode = ETH_MQ_TX_NONE,
    }
};


// ########################################
// Setting Up Sender and Receiver
// ########################################


void setup_best_effort_receiver(mempoolptr mbuf_pool) {

	int ret;

    // Configure port for receiving
    ret = rte_eth_dev_configure(RX_PORT_NO_BE, RX_QUEUES, 0, &port_conf);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Port could not be configured.\n");

    // Setup a receive queue for port
    ret = rte_eth_rx_queue_setup(RX_PORT_NO_BE, RX_QUEUE_ID_BE, RX_RING_SIZE, rte_eth_dev_socket_id(RX_PORT_NO_BE), NULL, mbuf_pool);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not set-up the receive queue for port.\n");

	// Start the port
    ret = rte_eth_dev_start(RX_PORT_NO_BE);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not start port\n");

	// Enable RX in promiscuous mode for the Ethernet device.
    rte_eth_promiscuous_enable(RX_PORT_NO_BE);

	// Start running function receive_pkt on core RX_CORE_NR
    ret = rte_eal_remote_launch(receive_pkt_be, mbuf_pool, RX_CORE_NR_BE);

}

void setup_time_sensitive_receiver(mempoolptr mbuf_pool) {

	int ret;

    // Configure port for receiving
    ret = rte_eth_dev_configure(RX_PORT_NO_TS, RX_QUEUES, 0, &port_conf);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Port could not be configured.\n");

    // Setup a receive queue for port
    ret = rte_eth_rx_queue_setup(RX_PORT_NO_TS, RX_QUEUE_ID_TS, RX_RING_SIZE, rte_eth_dev_socket_id(RX_PORT_NO_TS), NULL, mbuf_pool);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not set-up the receive queue for port.\n");

	// Start the port
    ret = rte_eth_dev_start(RX_PORT_NO_TS);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not start port\n");

	// Enable RX in promiscuous mode for the Ethernet device.
    rte_eth_promiscuous_enable(RX_PORT_NO_TS);

	// Start running function receive_pkt on core RX_CORE_NR
    ret = rte_eal_remote_launch(receive_pkt_ts, mbuf_pool, RX_CORE_NR_TS);
}

void setup_sender(mempoolptr mbuf_pool) {

	int ret;

    // Configure port for sending
    ret = rte_eth_dev_configure(TX_PORT_NO, 0, TX_QUEUES, &port_conf);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Port could not be configured\n");

    // Setup a transmit queue for tx port
    struct rte_eth_dev_info dev_info;
    rte_eth_dev_info_get(TX_PORT_NO, &dev_info);
    struct rte_eth_txconf *txconf  = &dev_info.default_txconf;
    txconf->txq_flags = 0;
    ret = rte_eth_tx_queue_setup(TX_PORT_NO, TX_QUEUE_ID, TX_RING_SIZE, rte_eth_dev_socket_id(TX_PORT_NO), txconf);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not set-up the transmit queue for port\n");

    // Start the port
    ret = rte_eth_dev_start(TX_PORT_NO);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not start port\n");

	// Enable RX in promiscuous mode for the Ethernet device.
    rte_eth_promiscuous_enable(TX_PORT_NO);

	// Start running function send_pkt on core TX_CORE_NR
    ret = rte_eal_remote_launch(send_pkt, mbuf_pool, TX_CORE_NR);

	printf("SETUP SENDER FINISHED\n");
}

void setup_receiver(mempoolptr mbuf_pool) {

	setup_best_effort_receiver(mbuf_pool);
	setup_time_sensitive_receiver(mbuf_pool);
	
	return;
	
	/*
	int ret;

    // Configure port for receiving
    ret = rte_eth_dev_configure(RX_PORT_NO, RX_QUEUES, 0, &port_conf);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Port could not be configured.\n");

    // Setup a receive queue for port
    ret = rte_eth_rx_queue_setup(RX_PORT_NO, RX_QUEUE_ID, RX_RING_SIZE, rte_eth_dev_socket_id(RX_PORT_NO), NULL, mbuf_pool);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not set-up the receive queue for port.\n");

	// Start the port
    ret = rte_eth_dev_start(RX_PORT_NO);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not start port\n");

	// Enable RX in promiscuous mode for the Ethernet device.
    rte_eth_promiscuous_enable(RX_PORT_NO);

	// Start running function receive_pkt on core RX_CORE_NR
    ret = rte_eal_remote_launch(receive_pkt, mbuf_pool, RX_CORE_NR);
    
    */


}
