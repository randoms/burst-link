#include "queue.h"

/* crateQueue function takes argument the maximum number of elements the Queue can hold, creates
   a Queue according to it and returns a pointer to the Queue. */
Queue * createQueue(int maxElements)
{
        /* Create a Queue */
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        /* Initialise its properties */
        Q->elements = (MSGTask *)malloc(sizeof(MSGTask)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = 0;
        Q->rear = -1;
        /* Return the pointer */
        return Q;
}
void Dequeue(Queue *Q)
{
        /* If Queue size is zero then it is empty. So we cannot pop */
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                return;
        }
        /* Removing an element is equivalent to incrementing index of front by one */
        else
        {
                Q->size--;
                Q->front++;
                /* As we fill elements in circular fashion */
                if(Q->front==Q->capacity)
                {
                        Q->front=0;
                }
        }
        return;
}
MSGTask *front(Queue *Q)
{
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                exit(0);
        }
        /* Return the element which is at the front*/
        return &(Q->elements[Q->front]);
}
void Enqueue(Queue *Q,MSGTask *element)
{
        /* If the Queue is full, we cannot push an element into it as there is no space for it.*/
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
                Q->size++;
                Q->rear = Q->rear + 1;
                /* As we fill the queue in circular fashion */
                if(Q->rear == Q->capacity)
                {
                        Q->rear = 0;
                }
                /* Insert the element in its rear side */ 
                Q->elements[Q->rear].target_addr_bin = element->target_addr_bin;
                Q->elements[Q->rear].msg = element->msg;
        }
        return;
}
// int main()
// {
//         MSGTask *task1, *task2;
//         task1 = (MSGTask *)malloc(sizeof(MSGTask));
//         task1->sock = 1;
//         task1->msg = "dasdasdas";
//         task2 = (MSGTask *)malloc(sizeof(MSGTask));
//         task2->sock = 2;
//         task2->msg = "dsadasdas";
//         Queue *Q = createQueue(5);
//         Enqueue(Q,task1);
//         Enqueue(Q,task2);
//         MSGTask *task3 = front(Q);
//         printf("Front element is %d\n",task3->sock);
//         printf("%s\n",task3->msg);
//         Enqueue(Q,task2);
//         Dequeue(Q);
//         task3 = front(Q);
//         printf("Front element is %d\n",task3->sock);
//         printf("%s\n",task3->msg);
// }
