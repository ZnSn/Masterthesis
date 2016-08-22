/*
 * C Program to Implement a Queue using an Array
 */

#include "common.h"
#include "config.h"
 
#include "queue.h"


QueueNode* create_node(Payload* payload) {
	QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
	new_node->ptr = NULL;
	new_node->payload = payload;

	return new_node;
}

Queue* create_queue() {
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	queue->queue_count = 0;
	queue->front = NULL;
	queue->rear = NULL;
	
	return queue;
}

bool queue_enq(Queue* queue, Payload* payload) {

	QueueNode* tmp = create_node(payload);

	if (queue->rear == NULL) { // No queue created yet
		queue->rear = tmp;
		queue->front = queue->rear;
	}
	else {
		queue->rear->ptr = tmp;
		queue->rear = tmp;
	}

	queue->queue_count++;
	return true;
}

void queue_deq(Queue* queue, Payload** payload) {

	if (queue->front == NULL) // No element in queue 
		return;

	QueueNode* tmp = queue->front;

	*payload = queue->front->payload;
	tmp = tmp->ptr; // tmp is next
	free(queue->front);
	queue->front = tmp;

	if (queue->front == NULL) // empty queue
		queue->rear = NULL;

	queue->queue_count--;
}

bool queue_empty(Queue* queue) {
	return ((queue->queue_count) == 0);
}

int queue_size(Queue* queue) {
	return queue->queue_count;
}

