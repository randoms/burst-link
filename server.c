/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

/**
 * server 主要进行请求的分发，接收到新的请求就把请求分发给route，
 */
#include "server.h"
/**
 * Globals
 */
Tox *my_tox;
uint8_t msg_task_flag = 0;
uint8_t msg_rec_flag = 0;
Queue *msg_task_queue; // 消息处队列
Queue *msg_rec_queue; //需要传递给node端的信息
Msg_listener_list *msg_listener_list = NULL;
uint8_t MODE = 0; // 0 req mode 1,server mode
uint32_t node_sockfd = 0; // node socket 

const uint8_t *target_ip; // 远端连接目标IP
uint8_t target_port; // 远端连接目标端口
    
void error(const char *msg)
{
    perror(msg);
    exit(1);
}


void friend_request(Tox *messenger, const uint8_t *public_key, const uint8_t *data, uint16_t length, void *userdata) {
    //接受請求
    write_local_message(node_sockfd,"friend req received");
    printf("req received\n");
    uint8_t str[TOX_CLIENT_ID_SIZE*2];
    hex_bin_to_string(public_key,TOX_CLIENT_ID_SIZE,str);
    printf("%s\n",str);
    accept_connect(my_tox,str,msg_listener_list);
}

void friend_message(Tox *m, int32_t friendnumber, const uint8_t *string, uint16_t length, void *userdata) {
    printf("MESSAGE:%s\n",string);
    write_local_message(node_sockfd,string);
    // get remote id
    uint8_t client_id_bin[TOX_CLIENT_ID_SIZE];
    uint8_t client_id_str[TOX_CLIENT_ID_SIZE*2];
    tox_get_client_id(m,friendnumber,client_id_bin);
    hex_bin_to_string(client_id_bin,TOX_CLIENT_ID_SIZE,client_id_str);
    // 添加消息觸發器
    trigger_msg_listener(msg_listener_list,string,client_id_str);
    //print_msg_listener_list(msg_listener_list);
    // 添加消息隊列消息
    // 服务端模式处理
    if(MODE == 1){
        if(strcmp(string,"HANDSHAKE") == 0){
            tox_send_message(m,friendnumber,"HANDSHAKE",strlen("HANDSHAKE"));
            return;
        }
        uint8_t *INIT_REQ_STR = "INIT_REQ:";
        int32_t i =0;
        uint8_t equal_flag = 1;
        for(i=0;i<strlen(INIT_REQ_STR);i++){
            if(string[i] != INIT_REQ_STR[i])equal_flag = 0;
        }
        if(equal_flag == 1){
            // send setting info to node
//             json_t *res = json_object();
//             int32_t err;
//             err = json_object_set(res,"TARGET_IP",json_string(target_ip));
//             mjson_error(err);
//             err = json_object_set(res,"PORT",json_integer(target_port));
//             mjson_error(err);
//             uint8_t response[2048];
//             response[0] = '\0';
//             strcat(response,"INIT_REQ");
//             strcat(response,json_dumps(res,JSON_INDENT(4)));
            //write_local_message(node_sockfd,response);
            write_local_message(node_sockfd,"test");
            
            // fake response
            tox_send_message(m,friendnumber,"INIT_REQ:OK",strlen("INIT_REQ:OK"));
            // tox_send_message(m,friendnumber,"INIT_REQ:OK",strlen("INIT_REQ:ERROR"));
        }
    }
    
    if(MODE == 0){
        if(strcmp(string,"INIT_REQ:OK") == 0){
            write_local_message(node_sockfd,"INIT_REQ:OK");
            write_local_message(node_sockfd,"402");
        }
    }
}

void write_file(Tox *messenger, int32_t friendnumber, uint8_t filenumber,const uint8_t *data, uint16_t length, void *userdata) {
}

void file_print_control(Tox *messenger, int friendnumber, uint8_t send_receive, uint8_t filenumber, uint8_t control_type, const uint8_t *data, uint16_t length, void *userdata) {
}

void file_request_accept(Tox *messenger, int friendnumber, uint8_t filenumber, uint64_t filesize,const uint8_t *filename, uint16_t filename_length, void *userdata) {
}

void on_connectionchange(Tox *m, int32_t friendnumber, uint8_t status, void *userdata){
    
}


static Tox *init_tox()
{
    Tox *m = tox_new(1);

    if (m == NULL) {
//         fprintf(stderr, "IPv6 didn't initialize, trying IPv4\n");
        m = tox_new(0);
    }

    if (m ==NULL)
//         fprintf(stderr, "Forcing IPv4 connection\n");

    /* Callbacks */
    tox_callback_connection_status(m, on_connectionchange, NULL);
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

int init_tox_connection(Tox *m)
{
    uint8_t *pub_key = hex_string_to_bin(BOOTSTRAP_KEY);
    int res = tox_bootstrap_from_address(m, BOOTSTRAP_ADDRESS, 0, htons(BOOTSTRAP_PORT), pub_key);
    if (!res) {
//         printf("Failed to convert into an IP address. Exiting...\n");
        exit(1);
    }
}



void *tox_works(){
    // do tox loop
    time_t timestamp0 = time(NULL);
    int on = 0;
    while (1) {
        if (on == 0) {
            if (tox_isconnected(my_tox)) {
//                 printf("connected to DHT\n");
                on = 1;
            } else {
                time_t timestamp1 = time(NULL);
                if (timestamp0 + 10 < timestamp1) {
                    timestamp0 = timestamp1;
                    init_tox_connection(my_tox);
                }
                
            }
        }
        tox_do(my_tox);
        if(msg_task_queue->size != 0 && msg_task_flag == 0){
            msg_task_flag = 1;
            MSGTask *mTask = front(msg_task_queue); // 开始处理
            route(mTask->msg,my_tox,mTask->sock);
            close(mTask->sock);
            Dequeue(msg_task_queue);
            msg_task_flag = 0;
        }
        else{
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
//     printf("Message Received:\n %s\n",buffer);
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

/**
 * send target ip and port to remote
 */
void init_req_mode(Tox *m,const uint8_t *address_str, const uint8_t *target_ip,const uint32_t target_port){
    uint8_t client_id_str[TOX_CLIENT_ID_SIZE*2];
    address_str_to_client_str(address_str,client_id_str);
    uint16_t friendnum = tox_get_friend_number(m,hex_string_to_bin(client_id_str));
    if(friendnum == -1){
        write_local_message(node_sockfd,"401");
        exit(1);
    }
    write_local_message(node_sockfd,"START INIT_REQ ...");
    if(tox_send_message(m,friendnum,"INIT_REQ:",strlen("INIT_REQ:")) < 0){
        write_local_message(node_sockfd,"401");
        exit(1);
    }
}

void on_local_message_received(uint32_t sockfd, uint8_t *msg){
    printf("LOCAL_MESSAGE:%s\n",msg);
}

void init_local_sock(local_port){
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    node_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (node_sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(local_port);
    if (connect(node_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
}

int main(int argc, char *argv[])
{
    // 添加系统事件监听
    signal(SIGINT, intHandler);
    my_tox = init_tox();
    init_tox_connection(my_tox);
    load_data(my_tox);
    
    // 处理参数
    const uint32_t local_port = atoi(argv[1]);
    const uint8_t *target_id;
    if(argc >3){
        target_id = argv[2];
        target_ip = argv[3];
        target_port = atoi(argv[4]);
    }else{
        target_id = NULL;
        uint8_t my_address_bin[TOX_FRIEND_ADDRESS_SIZE];
        uint8_t my_address_str[TOX_FRIEND_ADDRESS_SIZE*2];
        tox_get_address(my_tox,my_address_bin);
        hex_bin_to_string(my_address_bin,TOX_FRIEND_ADDRESS_SIZE,my_address_str);
        printf("MYID:%s\n",my_address_str);
    }
    // 虛擬參數
    
    /*
    const uint8_t *target_id = "341CCFBCC4D41C5B3AB89E31E7561C5D37E201D5DDBFA7AFC6B4EDD2D6A82F4B7D06A2ED3DE4";
    const uint8_t *target_ip = "";
    const uint32_t target_port = 0;
    */
    
    // 初始化消息隊列
    msg_task_flag = 0; // 1 when msg is not available
    msg_rec_flag = 0; // 1 when rec_queue is not available
    msg_task_queue = createQueue(MAX_MSG_CACHE); // 远程操作消息队列
    msg_rec_queue = createQueue(MAX_MSG_CACHE); // 本地操作消息队列
    
    // 開始tox線程
    pthread_t tox_thread;
    int iret1 = pthread_create( &tox_thread, NULL, tox_works,NULL);
    if(iret1){
//         printf("Create tox thread failed.\n");
        exit(EXIT_FAILURE);
    }
    
    // 初始化本地连接
    init_local_sock(local_port);
    
    // 開始準備連接
    if(target_id != NULL){
        // 進入請求者模式
        // 等待tox成功連接
        while(!tox_isconnected(my_tox)){
            usleep(20000);
        }
//         printf("connected\n");
        write_local_message(node_sockfd,"TOXCORE:ONLINE");
        init_connect(my_tox,target_id,&msg_listener_list);
        write_local_message(node_sockfd,"CONNECT:OK");
        init_req_mode(my_tox,target_id,target_ip,target_port);
    }else{
        // 進入服務者模式
        MODE = 1;
    }

    

    char buffer[2048];
    
    while(1){
        bzero(buffer,2048);
        int n = read(node_sockfd,buffer,2047);
        if(n>0)on_local_message_received(node_sockfd,buffer);
        if (n < 0) 
         error("ERROR reading from socket");
    }
    //開始監聽本地端口
//     int sockfd, newsockfd, portno,pid;
//     socklen_t clilen;
//     char buffer[256];
//     struct sockaddr_in serv_addr, cli_addr;
//     signal(SIGCHLD,SIG_IGN);
//     if (argc < 2) {
//         fprintf(stderr,"ERROR, no port provided\n");
//         exit(1);
//     }
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) 
//        error("ERROR opening socket");
//     bzero((char *) &serv_addr, sizeof(serv_addr));
//     portno = atoi(argv[1]);
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_addr.s_addr = INADDR_ANY;
//     serv_addr.sin_port = htons(portno);
//     if (bind(sockfd, (struct sockaddr *) &serv_addr,
//             sizeof(serv_addr)) < 0) 
//             error("ERROR on binding");
//     listen(sockfd,5);
//     clilen = sizeof(cli_addr);
//     while(1){
//         newsockfd = accept(sockfd, 
//               (struct sockaddr *) &cli_addr, 
//               &clilen);
//         if (newsockfd < 0) 
//             error("ERROR on accept");
//         
//         pthread_t *sock_thread = (pthread_t *)malloc(sizeof(pthread_t));
//         int iret2 = pthread_create( sock_thread, NULL, process_sock,(void*)(&newsockfd));
//         if(iret2){
//             printf("Create Socket thread failed.\n");
//         }
//         // this may led to memory leak
//     }
//     close(sockfd);
    return 0; 
}
