/*
 * fifoqueue.c
 *
 *  Created on: May 30, 2016
 *      Author: stephan
 */

#include "fifoqueue.h"

int queue_count = 0;
FifoNode* front = NULL;
FifoNode* rear = NULL;

FifoNode* create_node(Message* package) {
	FifoNode* new_node = (FifoNode*)malloc(sizeof(FifoNode));
	new_node->ptr = NULL;
	new_node->data = package;

	return new_node;
}

bool fifo_enq(Message* package) {

	FifoNode* tmp = create_node(package);

	if (rear == NULL) { // No queue created yet
		rear = tmp;
		front = rear;
	}
	else {
		rear->ptr = tmp;
		rear = tmp;
	}

	queue_count++;
	return true;
}

void fifo_deq(Message** package) {

	if (front == NULL) // No element in queue 
		return;

	FifoNode* tmp = front;

	*package = front->data;
	tmp = tmp->ptr; // tmp is next
	free(front);
	front = tmp;

	if (front == NULL) // empty queue
		rear = NULL;

	queue_count--;
		
	return;
}

bool fifo_empty() {
	return queue_count == 0;
}

int fifo_size() {
	return queue_count;
}

