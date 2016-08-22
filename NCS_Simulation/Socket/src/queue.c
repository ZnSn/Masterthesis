/*
 * C Program to Implement a Queue using an Array
 */

#include "queue.h"
#include <stdio.h>

int queue_count = 0;
QueueNode* front = NULL;
QueueNode* rear = NULL;


QueueNode* create_node(Payload* payload) {
	QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
	new_node->ptr = NULL;
	new_node->payload = payload;

	return new_node;
}

bool queue_enq(Payload* payload) {

	QueueNode* tmp = create_node(payload);

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

void queue_deq(Payload** payload) {

	if (front == NULL) // No element in queue 
		return;

	QueueNode* tmp = front;

	*payload = front->payload;
	tmp = tmp->ptr; // tmp is next
	free(front);
	front = tmp;

	if (front == NULL) // empty queue
		rear = NULL;

	queue_count--;
		
	return;
}

bool queue_empty() {
	return queue_count == 0;
}

int queue_size() {
	return queue_count;
}

