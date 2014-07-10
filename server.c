/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

/**
 * server 主要进行请求的分发，接收到新的请求就把请求分发给route，
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#include "toxcore/tox.h"
#include "utils/utils.c"
#include "queue.h"
#include "route.h"


#define BOOTSTRAP_ADDRESS "42.96.195.88"
#define BOOTSTRAP_PORT 33445
#define BOOTSTRAP_KEY "7F613A23C9EA5AC200264EB727429F39931A86C39B67FC14D9ECA4EBE0D37F25"
#define MAX_MSG_CACHE 128
/*
#define BOOTSTRAP_ADDRESS "109.169.46.133"
#define BOOTSTRAP_PORT 33445
#define BOOTSTRAP_KEY "7F31BFC93B8E4016A902144D0B110C3EA97CB7D43F1C4D21BCAE998A7C838821"
*/
#define MY_NAME "BurstLink"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *SigCatcher(int n)
{
  wait3(NULL,WNOHANG,NULL);
}

void friend_request(Tox *messenger, const uint8_t *public_key, const uint8_t *data, uint16_t length, void *userdata) {
}

void friend_message(Tox *messenger, int32_t friendnumber, const uint8_t *string, uint16_t length, void *userdata) {
    printf("MESSAGE:%s\n",string);
}

void write_file(Tox *messenger, int32_t friendnumber, uint8_t filenumber,const uint8_t *data, uint16_t length, void *userdata) {
}

void file_print_control(Tox *messenger, int friendnumber, uint8_t send_receive, uint8_t filenumber, uint8_t control_type, const uint8_t *data, uint16_t length, void *userdata) {
}

void file_request_accept(Tox *messenger, int friendnumber, uint8_t filenumber, uint64_t filesize,const uint8_t *filename, uint16_t filename_length, void *userdata) {
}

static Tox *init_tox(int ipv4)
{
    /* Init core */
    int ipv6 = !ipv4;
    Tox *m = tox_new(ipv6);

    /*
    * TOX_ENABLE_IPV6_DEFAULT is always 1.
    * Checking it is redundant, this *should* be doing ipv4 fallback
    */
    if (ipv6 && m == NULL) {
        fprintf(stderr, "IPv6 didn't initialize, trying IPv4\n");
        m = tox_new(0);
    }

    if (ipv4)
        fprintf(stderr, "Forcing IPv4 connection\n");

    if (m == NULL)
        return NULL;

    /* Callbacks */
    //tox_callback_connection_status(m, on_connectionchange, NULL);
    //tox_callback_typing_change(m, on_typing_change, NULL);
    tox_callback_friend_request(m, friend_request, NULL);
    tox_callback_friend_message(m, friend_message, NULL);
    //tox_callback_name_change(m, on_nickchange, NULL);
    //tox_callback_user_status(m, on_statuschange, NULL);
    //tox_callback_status_message(m, on_statusmessagechange, NULL);
    //tox_callback_friend_action(m, on_action, NULL);
    //tox_callback_group_invite(m, on_groupinvite, NULL);
    //tox_callback_group_message(m, on_groupmessage, NULL);
    //tox_callback_group_action(m, on_groupaction, NULL);
    //tox_callback_group_namelist_change(m, on_group_namelistchange, NULL);
    tox_callback_file_send_request(m, file_request_accept, NULL);
    tox_callback_file_control(m, file_print_control, NULL);
    tox_callback_file_data(m, write_file, NULL);

    tox_set_name(m, MY_NAME, strlen(MY_NAME)); // Sets the username
    return m;
}

int init_connection(Tox *m)
{
    uint8_t *pub_key = hex_string_to_bin(BOOTSTRAP_KEY);
    int res = tox_bootstrap_from_address(m, BOOTSTRAP_ADDRESS, 0, htons(BOOTSTRAP_PORT), pub_key);
    if (!res) {
        printf("Failed to convert into an IP address. Exiting...\n");
        exit(1);
    }
}


/**
 * Globals
 */
Tox *my_tox;
uint8_t msg_task_flag;
uint8_t msg_rec_flag;
Queue *msg_task_queue; // 消息处队列
Queue *msg_rec_queue;

void *tox_works(){
    // do tox loop
    time_t timestamp0 = time(NULL);
    int on = 0;
    while (1) {
        if (on == 0) {
            if (tox_isconnected(my_tox)) {
                printf("connected to DHT\n");
                on = 1;
            } else {
                time_t timestamp1 = time(NULL);
                if (timestamp0 + 10 < timestamp1) {
                    timestamp0 = timestamp1;
                    init_connection(my_tox);
                }
                
            }
        }
        tox_do(my_tox);
        if(msg_task_queue->size != 0 && msg_task_flag == 0){
            printf("processing\n");
            msg_task_flag = 1;
            MSGTask *mTask = front(msg_task_queue); // 开始处理
            route(mTask->msg,my_tox,mTask->sock);
            close(mTask->sock);
            Dequeue(msg_task_queue);
            msg_task_flag = 0;
        }else{
            usleep(40000);
        }
    }
}

void *process_sock(void *ptr){
    int sock = *((int *)ptr);
    int n;
    uint8_t buffer[2048];
    bzero(buffer,2048);
    n = read(sock,buffer,2048);
    if (n < 0) error("ERROR reading from socket");
    printf("Message Received:\n %s\n",buffer);
    MSGTask *newTask = (MSGTask *)malloc(sizeof(MSGTask));
    newTask->sock = sock;
    newTask->msg = buffer;
    do{
        while(msg_task_queue->size >= MAX_MSG_CACHE); // wait for cache space
    }while(msg_task_flag == 1);
    // enter queue
    msg_task_flag = 1;
    Enqueue(msg_task_queue,newTask);
    msg_task_flag = 0;
    // free space
    free(newTask);
}


void intHandler(int dummy) {
    store_data(my_tox);
    printf("EXITING...\n");
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
    // 添加系统事件监听
    signal(SIGINT, intHandler);
    my_tox = init_tox(0);
    init_connection(my_tox);
    load_data(my_tox);
    msg_task_flag = 0; // 1 when msg is not available
    msg_rec_flag = 0; // 1 when rec_queue is not available
    msg_task_queue = createQueue(MAX_MSG_CACHE); // 远程操作消息队列
    msg_rec_queue = createQueue(MAX_MSG_CACHE); // 本地操作消息队列
    pthread_t tox_thread;
    int iret1 = pthread_create( &tox_thread, NULL, tox_works,NULL);
    if(iret1){
        printf("Create tox thread failed.\n");
        exit(EXIT_FAILURE);
    }
    int sockfd, newsockfd, portno,pid;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    signal(SIGCHLD,SIG_IGN);
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while(1){
        newsockfd = accept(sockfd, 
              (struct sockaddr *) &cli_addr, 
              &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        
        pthread_t *sock_thread = (pthread_t *)malloc(sizeof(pthread_t));
        int iret2 = pthread_create( sock_thread, NULL, process_sock,(void*)(&newsockfd));
        if(iret2){
            printf("Create Socket thread failed.\n");
        }
        // this may led to memory leak
    }
    close(sockfd);
    return 0; 
}
