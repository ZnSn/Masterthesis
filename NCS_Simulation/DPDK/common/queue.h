
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "common.h"

QueueNode* create_node();
Queue* create_queue(); 

bool queue_enq(Queue* queue, Payload* payload);
void queue_deq(Queue* queue, Payload** payload);
bool queue_empty(Queue* queue);
int queue_size(Queue* queue);

#endif /* __QUEUE_H__ */
