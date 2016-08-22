#include "common.h"

#ifndef _QUEUE_H__
#define _QUEUE_H__

typedef struct QueueNode QueueNode;

struct QueueNode
{
    Payload* payload;
    QueueNode* ptr;
};
 

int queue_size();

bool queue_enq(Payload* payload);
void queue_deq(Payload** payload);
bool queue_empty();

QueueNode* create_node();

#endif
