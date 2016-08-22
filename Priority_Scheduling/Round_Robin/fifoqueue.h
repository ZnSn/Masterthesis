/*
 * fifoqueue.h
 *
 *  Created on: August 05, 2016
 *      Author: stephan
 */

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include "common.h"

#define BUFFER_SIZE 	100


int fifo_size();
bool fifo_enq(Message* payload);
bool fifo_deq(Message** payload);
bool fifo_empty();


#endif /* _RINGBUFFER_H_ */
