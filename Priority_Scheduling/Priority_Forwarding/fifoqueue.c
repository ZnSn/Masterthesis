/*
 * fifoqueue.c
 *
 *  Created on: August 05, 2016
 *      Author: stephan
 */

#include "fifoqueue.h"

int queue_count = 0;
unsigned long int overflow = 0;
unsigned long int underflow = 0;

int buf_read = 0;
int buf_write = 0;
Message* buffer[BUFFER_SIZE];


bool fifo_enq(Message* package) {

	if ( buf_read == (buf_write + 1) % BUFFER_SIZE ) {
		overflow++;
		return false;
	}
	
	buffer[buf_write] = package;
	buf_write = (buf_write + 1) % BUFFER_SIZE;

	queue_count++;
	return true;
}

bool fifo_deq(Message** package) {

	if (buf_read == buf_write) {
		// Empty Queue
		underflow++;
		return false;
	}
	
	*package = buffer[buf_read];
	buf_read = (buf_read + 1) % BUFFER_SIZE;
		
	queue_count--;
	return true;
}

bool fifo_empty() {
	return queue_count == 0;
}

int fifo_size() {
	return queue_count;
}

