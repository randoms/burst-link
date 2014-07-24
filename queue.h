#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "toxcore/tox.h"

typedef struct MSGTask {
   uint8_t *target_addr_bin;
   uint8_t *msg;
} MSGTask;


/*Queue has five properties. capacity stands for the maximum number of elements Queue can hold.
  Size stands for the current size of the Queue and elements is the array of elements. front is the
 index of first element (the index at which we remove the element) and rear is the index of last element
 (the index at which we insert the element) */
typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        MSGTask *elements;
}Queue;

/* crateQueue function takes argument the maximum number of elements the Queue can hold, creates
   a Queue according to it and returns a pointer to the Queue. */
Queue * createQueue(int maxElements);

void Dequeue(Queue *Q);
MSGTask *front(Queue *Q);
void Enqueue(Queue *Q,MSGTask *element);
#endif