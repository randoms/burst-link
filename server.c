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
uint8_t msg_task_flag = 0; // 1 not available, 0 available
uint8_t msg_rec_flag = 0;
Queue *msg_task_queue; // 消息处队列
Queue *msg_rec_queue; //需要传递给node端的信息
Msg_listener_list *msg_listener_list = NULL;
uint8_t MODE = 0; // 0 req mode 1,server mode

const uint8_t *target_ip; // 远端连接目标IP
uint32_t target_port; // 远端连接目标端口
const uint8_t *target_id; // 远程目标ADDRESS

// FRIEND NUMBER
uint32_t FRIEND_NUM = 0;

// flags
int32_t init_over_flag = 0;
int32_t init_req_flag = 0;

// local sockets
local_socks_list *msocks_list = NULL;
uint32_t local_socksfd = 0;

//debug
int previous = -1;
    
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// typedef struct accept_req_params{
//     Tox *m;
//     const uint8_t *id;
//     Msg_listener_list *msg_listener_list;
//     uint32_t sockfd;
//     
// }accept_req_params;

// void *accept_req_work(void *mparms){
//     accept_req_params *parms = (accept_req_params *)mparms;
//     accept_connect(parms->m,parms->id,parms->msg_listener_list,parms->sockfd);
// }

void friend_request(Tox *messenger, const uint8_t *public_key, const uint8_t *data, uint16_t length, void *userdata) {
    //接受請求
//     printf("req received\n");
//     uint8_t *str = (uint8_t *)malloc(sizeof(uint8_t)*(TOX_CLIENT_ID_SIZE*2+1));
//     hex_bin_to_string(public_key,TOX_CLIENT_ID_SIZE,str);
//     // start a new thread to init new connection
//     
//     pthread_t *accept_req_thread;
//     accept_req_thread = (pthread_t *)malloc(sizeof(pthread_t));
//     accept_req_params *req_parms = (accept_req_params *)malloc(sizeof(accept_req_params));
//     req_parms->m = my_tox;
//     req_parms->id = str;
//     req_parms->msg_listener_list = msg_listener_list;
//     req_parms->sockfd = local_socksfd;
//     
//     int iret1 = pthread_create( accept_req_thread, NULL,accept_req_work,req_parms);
//     if(iret1){
//         exit(EXIT_FAILURE);
//     }else{
// //         write_local_message(local_socksfd,"PROCESSING FRIEND REQ");
//     }
}

void friend_message(Tox *m, int32_t friendnumber, const uint8_t *bin, uint16_t length, void *userdata) {
    
    uint8_t client_id_bin[TOX_CLIENT_ID_SIZE];
    uint8_t client_id_str[TOX_CLIENT_ID_SIZE*2+1];
    tox_get_client_id(m,friendnumber,client_id_bin);
    hex_bin_to_string(client_id_bin,TOX_CLIENT_ID_SIZE,client_id_str);
    // 添加消息觸發器
    trigger_msg_listener(msg_listener_list,bin,client_id_str);
    //trigger_msg_listener_debug(msg_listener_list,bin,client_id_str,local_socksfd);
    // 添加消息隊列消息
    // 服务端模式处理
    if(MODE == 1){
        
    }
    
    // 请求者模式
    if(MODE == 0){
        
        if(strcmp(bin,"HANDSHAKE") == 0){
            return;
        }
        
        // parse message
        uint8_t string[16384];
        bin_to_msg(string,bin);
        json_error_t err;
        json_t *msg_json = json_loads(string,0,&err);
        if(msg_json == NULL)
            return;
        json_t *uuid_json = json_object_get(msg_json,"uuid");
        if(uuid_json == NULL)
            return;
        json_t *cmd_json = json_object_get(msg_json,"cmd");
        if(cmd_json == NULL){
            on_remote_data_received(string,client_id_bin);
            return;
        }
        printf("CMD:%s\n",string);
        const uint8_t *cmd_str = json_string_value(cmd_json);
        if(strcmp(cmd_str,"CLOSE_SOCK") == 0){
            const uint8_t *uuid_str = json_string_value(uuid_json);
            printf("CLOSE SOCKET:%s\n",uuid_str);
            int32_t sockfd = get_local_socks(msocks_list,uuid_str);
            printf("%d\n",sockfd);
            shutdown(sockfd,2);
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
            send_data_remote();
        }else{
            usleep(40000);
        }
    }
}



void intHandler(int dummy) {
    store_data(my_tox);
    printf("EXITING...\n");
    exit(EXIT_SUCCESS);
}

/**
 * send target ip and port to remote
 * return 0 timeout
 * return 1 success
 * return -1 error
 */
int init_req_mode(Tox *m,const uint8_t *address_str, const uint8_t *target_ip,const uint32_t target_port){
    uint8_t client_id_str[TOX_CLIENT_ID_SIZE*2+1];
    address_str_to_client_str(address_str,client_id_str);
    uint16_t friendnum = tox_get_friend_number(m,hex_string_to_bin(client_id_str));
    if(friendnum == -1){
//         write_local_message(local_socksfd,"401");
        exit(1);
    }
//     write_local_message(local_socksfd,"START INIT_REQ ...");
    json_t *res = json_object();
    int32_t err;
    err = json_object_set(res,"TARGET_IP",json_string(target_ip));
    mjson_error(err);
    err = json_object_set(res,"PORT",json_integer((int)target_port));
    mjson_error(err);
    uint8_t response[1024];
    response[0] = '\0';
    strcat(response,"INIT_REQ:");
    strcat(response,json_dumps(res,JSON_INDENT(4)));
    int init_res = tox_send_message(m,friendnum,response,strlen(response));
    
    if(init_res < 0){
//         write_local_message(local_socksfd,"401");
        init_req_flag = -1;
        return -1;
    }
    
    // wait init req over
    uint8_t i = 0;
    while(init_req_flag == 0 && i<100){
        usleep(200000);
        i++;
    }
    if(i>100){
        // time out
        return 0;
    }
    return init_req_flag;
}


uint32_t init_local_sock(uint32_t local_port){
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
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
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    return sockfd;
}

uint32_t init_local_sock_serv(uint32_t local_port){
    int sockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(local_port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            error("ERROR on binding");
    listen(sockfd,5);
    return sockfd;
}


/**
 * add data to local message queue
 * this func may block if current message queue is not available
 * data format
 * {
 *  remote_id:
 *  uuid:
 *  cmd: can be null
 *  data: raw data <SOCK_BUF_SIZE or less>
 * }
 */
void write_data_local(uint8_t *uuid, uint8_t *data, uint32_t length){
    
}

/**
 * add data to remote message queue
 * this func may block if current message queue is not available
 * data format
 * {
 *  target_id: target to send
 *  uuid: sock uuid
 *  data: raw data <SOCK_BUF_SIZE or less>
 *  cmd: can be null when no cmd is to send
 * }
 * 
 */
void write_data_remote(const uint8_t *uuid, const uint8_t* cmd, const uint8_t *data, const uint32_t length){
    json_t *uuid_json = json_pack("s",uuid);
    json_t *cmd_json = json_pack("s",cmd);
    json_t *length_json = json_pack("i",length);
    json_t *msg_json = json_object();
    json_object_set(msg_json,"uuid",uuid_json);
    json_object_set(msg_json,"length",length_json);
    json_t *data_json;
    if(cmd != NULL){
        json_object_set(msg_json,"cmd",cmd_json);
        data_json = json_pack("s",data);
    }else{
        data_json = json_array();
        buf_to_json_array(data_json,data,length);
    }
    json_object_set(msg_json,"data",data_json);
    
    uint8_t *msg = json_dumps(msg_json,JSON_INDENT(4));
    // add msg to message queue
    MSGTask *newTask = (MSGTask *)malloc(sizeof(MSGTask));
    uint8_t *target_addr_bin = hex_string_to_bin(target_id);
    newTask->target_addr_bin = target_addr_bin;
    newTask->msg = msg;
    while(msg_task_flag == 1 || (msg_task_queue->size) >= MAX_MSG_CACHE){
        usleep(1000);
    };
    // enter queue
    msg_task_flag = 1;
    Enqueue(msg_task_queue,newTask);
    msg_task_flag = 0;
    // free space
    free(newTask);
}

/**
 * read message from local message queue, and send to local server
 * this will only send one message in the message queue
 * and write_data_local will block at the same time
 */
void send_data_local(){
    
}


void debug_msg_bin(uint8_t *msg_bin){
    if(msg_bin[UUID_LENGTH] != 0)return;
    uint8_t hight_byte = msg_bin[UUID_LENGTH+CMD_LENGTH];
    uint8_t low_byte = msg_bin[UUID_LENGTH+CMD_LENGTH + 1];
    uint32_t length = hight_byte*256 +low_byte;
    uint8_t data[length];
    int i=0;
    for(i=0;i<length;i++){
        data[i] = msg_bin[i + UUID_LENGTH + CMD_LENGTH + MESSAGE_LENGTH_BYTE];
    }
    debug_data(data,length);
}

/**
 * read message from remote message queue, and send to remote
 * this will only send one message in the message queue
 * and write_data_remote will be block at the same time
 */
void send_data_remote(){
    if(msg_task_queue->size != 0 && msg_task_flag == 0){
        msg_task_flag = 1;
        MSGTask *mTask = front(msg_task_queue); // 开始处理
        int friend_num = tox_get_friend_number(my_tox,mTask->target_addr_bin);
        uint8_t bin[MY_MESSAGE_LENGTH];
        msg_to_bin(bin,mTask->msg);
        //tox_send_message(my_tox,friend_num,mTask->msg,strlen(mTask->msg));
        int res = -1;
        int retry_count = 0;
        //debug_msg_bin(bin);
        while(res <=0 && retry_count <5){
            res = tox_send_message(my_tox,friend_num,bin,MY_MESSAGE_LENGTH);
            printf("send message failed\n");
            retry_count += 1;
        }
        if(retry_count >=5){
            printf("send message failed *********************\n");
        }else{
            printf("DATA SEND\n");
            Dequeue(msg_task_queue);
            free(mTask->target_addr_bin);
            free(mTask->msg);
        }
        msg_task_flag = 0;
    }
}

void debug_data(const uint8_t *data,uint32_t length){
    uint32_t i =0;
    if(previous != -1){
        if(data[0] != previous +1 && data[0] != 0){
            printf("%d,%d\n",previous,data[0]);
            printf("DATA ERROR 1\n");
            exit(1);
        }
    }
    for(i=0;i<length-1;i++){
        if(data[i+1] != data[i]+1 && data[i+1] !=0){
            printf("%d,%d\n",data[i],data[i+1]);
            printf("DATA ERROR\n");
            exit(1);
        }
    }
    previous = data[length-1];
}

/**
 * this func will be called immediately after local data received
 * wrap data in proper format
 * {
 *  uuid
 *  data
 * }
 */
void on_local_data_received(uint8_t *data, uint32_t length,uint32_t sockfd){
    const uint8_t *uuid = get_local_socks_uuid(msocks_list,sockfd);
    write_data_remote(uuid,NULL,data,length);
}

/**
 * this func will be called immediately after remote data received
 * NOTE: remote data is not remote message. it's remote message with fixed format
 * {
 *  uuid
 *  data
 *  cmd
 * }
 */
void on_remote_data_received(const uint8_t *data, const uint8_t *client_id_bin){
    
    json_error_t err;
    json_t *data_json = json_loads(data,0,&err);
    // get sockfd from uuid
    const uint8_t *uuid = json_string_value(json_object_get(data_json,"uuid"));
    const uint32_t length = json_integer_value(json_object_get(data_json,"length"));
    uint8_t data_bin[2048];
    json_array_to_bin(data_bin,json_object_get(data_json,"data"));
    uint32_t sockfd = get_local_socks(msocks_list,uuid);
    
    // send data to target socket
    if(sockfd != 0){
        // record found
        printf("WRITING DATA:%d\n",length);
        write(sockfd,data_bin,length);
    }else{
        // socket might be closed
        printf("INVALID SOCKET, CLOSE IT\n");
        close_remote_socket(uuid,client_id_bin);
        // socket might be create first time
    }
    json_decref(data_json);
}


/**
 * close remote socket
 */

void close_remote_socket(const uint8_t *uuid, const uint8_t *client_id_bin){
    write_data_remote(uuid,"CLOSE_SOCK","",strlen(""));
}

void create_remote_socket(const uint8_t *uuid, const uint8_t *client_id_bin,const uint8_t *target_ip, uint32_t target_port){
    json_t *target_port_json = json_pack("i",target_port);
    json_t *target_ip_json = json_pack("s",target_ip);
    json_t *data = json_object();
    json_object_set(data,"target_ip",target_ip_json);
    json_object_set(data,"target_port",target_port_json);
    uint8_t *data_str = json_dumps(data,JSON_INDENT(4));
    write_data_remote(uuid,"CREATE_SOCK",data_str,strlen(data_str));
    
}

void *on_local_sock_connect(void *msockfd){
    uint32_t sockfd = *((uint32_t *)msockfd);
    uint8_t *target_addr_bin = hex_string_to_bin(target_id);
    add_local_socks(msocks_list,sockfd,target_addr_bin,target_ip,target_port);
    printf("CONNECTED\n");
    // create remote socket
    const uint8_t *uuid_temp = get_local_socks_uuid(msocks_list,sockfd);
    uint8_t uuid[36];
    strcpy(uuid,uuid_temp);
    create_remote_socket(uuid,target_addr_bin,target_ip,target_port);
    uint8_t buf[SOCK_BUF_SIZE];
    int length = 1;
    while(length > 0){
        bzero(buf,SOCK_BUF_SIZE);
        length = read(sockfd,buf,SOCK_BUF_SIZE-1);
        if(length > 0){
            on_local_data_received(buf,length,sockfd);
        }
            
    }
    // read data error
    // close remote and local sock
    close(sockfd);
    close_remote_socket(uuid,target_addr_bin);
    //free(target_addr_bin);
    //close_local_socks(msocks_list,sockfd);
    
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
    
    // 初始化消息隊列
    msg_task_flag = 0; // 1 when msg is not available
    msg_rec_flag = 0; // 1 when rec_queue is not available
    msg_task_queue = createQueue(MAX_MSG_CACHE); // 远程操作消息队列
    msg_rec_queue = createQueue(MAX_MSG_CACHE); // 本地操作消息队列
    
    // 開始tox線程
    pthread_t tox_thread;
    int iret1 = pthread_create( &tox_thread, NULL, tox_works,NULL);
    if(iret1){
        exit(EXIT_FAILURE);
    }
    
    // 初始化本地连接
    local_socksfd = init_local_sock_serv(local_port);
    
    // 等待tox成功連接
    while(!tox_isconnected(my_tox)){
        usleep(20000);
    }
    printf("TOXCORE:ONLINE\n");
    printf("SERVER:LISTEN ON %d\n",local_port);
    
    // 開始準備連接
    if(target_id != NULL){
        // 進入請求者模式
        int res = init_connect(my_tox,target_id,&msg_listener_list);
        if(res == 402){
            printf("CONNECT:OK\n");
        }
        else{
            printf("CONNECT:ERROR\n");
        }
    }else{
        // 進入服務者模式
        MODE = 1;
    }

    

    // client mode
    // create local tcp server
    msocks_list = create_local_socks_list();
    
    char buffer[1024];
    
    while(1){
        struct sockaddr_in cli_addr;
        uint32_t clilen = sizeof(cli_addr);
        uint32_t newsockfd = accept(local_socksfd, 
                    (struct sockaddr *) &cli_addr, 
                    &clilen);
        printf("NEWFD:%d\n",newsockfd);
        if (newsockfd < 0){
            printf("LOCAL_SOCK:ERROR\n");
            continue;
        }
        
        // start a new thread
        pthread_t new_sock_thread;
        pthread_create( &new_sock_thread, NULL, on_local_sock_connect,&newsockfd);
    }
    
    return 0; 
}
