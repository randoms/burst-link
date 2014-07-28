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
                // release mem
                free(Q->elements[Q->front].msg);
                free(Q->elements[Q->front].target_addr_bin);
                
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
                return NULL;
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
                
                // 复制值，开辟新内存
                Q->elements[Q->rear].target_addr_bin = (uint8_t *)malloc(sizeof(uint8_t)*TOX_FRIEND_ADDRESS_SIZE);
                bufcopy(Q->elements[Q->rear].target_addr_bin,element->target_addr_bin,TOX_FRIEND_ADDRESS_SIZE);
                Q->elements[Q->rear].msg = (uint8_t *)malloc(sizeof(uint8_t)*MY_MESSAGE_LENGTH);
                bufcopy(Q->elements[Q->rear].msg,element->msg,MY_MESSAGE_LENGTH);
        }
        return;
}
// int main()
// {
//     Queue *Q = createQueue(5);
//     while(1){
//         MSGTask *task1, *task2;
//         task1 = (MSGTask *)malloc(sizeof(MSGTask));
//         task1->target_addr_bin = (uint8_t *)malloc(sizeof(uint8_t)*TOX_FRIEND_ADDRESS_SIZE);
//         bzero(task1->target_addr_bin,TOX_FRIEND_ADDRESS_SIZE);
//         task1->msg = (uint8_t *)malloc(sizeof(uint8_t)*TOX_FRIEND_ADDRESS_SIZE);
//         bzero(task1->msg,TOX_FRIEND_ADDRESS_SIZE);
//         task2 = (MSGTask *)malloc(sizeof(MSGTask));
//         task2->target_addr_bin = (uint8_t *)malloc(sizeof(uint8_t)*TOX_FRIEND_ADDRESS_SIZE);
//         bzero(task2->target_addr_bin,TOX_FRIEND_ADDRESS_SIZE);
//         task2->msg = (uint8_t *)malloc(sizeof(uint8_t)*TOX_FRIEND_ADDRESS_SIZE);
//         bzero(task2->msg,TOX_FRIEND_ADDRESS_SIZE);
//         Enqueue(Q,task1);
//         Enqueue(Q,task2);
//         MSGTask *task3 = front(Q);
//         
//         Enqueue(Q,task2);
//         Dequeue(Q);
//         task3 = front(Q);
//         
//         Dequeue(Q);
//         Dequeue(Q);
//         free(task1->msg);
//         free(task1);
//         free(task2->msg);
//         free(task2);
//         usleep(1000);
//     }
//         
// }
