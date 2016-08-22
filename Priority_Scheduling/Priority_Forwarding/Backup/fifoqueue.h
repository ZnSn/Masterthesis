/*
 * fifoqueue.h
 *
 *  Created on: May 30, 2016
 *      Author: stephan
 */

#ifndef FIFOQUEUE_H_
#define FIFOQUEUE_H_

#include "common.h"


typedef struct FifoNode FifoNode;

struct FifoNode
{
    Message* data;
    FifoNode* ptr;
};
 

int fifo_size();

bool fifo_enq(Message* payload);
void fifo_deq(Message** payload);
bool fifo_empty();

FifoNode* create_node();




#endif /* FIFOQUEUE_H_ */
