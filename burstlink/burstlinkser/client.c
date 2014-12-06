/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

/**
 * server 主要进行请求的分发，接收到新的请求就把请求分发给route，
 */
#include "client.h"

#ifdef _WIN32
#pragma comment(lib,"libtox.dll.a")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"jansson.lib")
#endif

/**
 * Globals
 */
Tox *my_tox;
Queue *msg_task_queue; // 消息处队列
Msg_listener_list *msg_listener_list = NULL;
uint8_t MODE = 0; // 0 req mode 1,server mode
uint8_t close_count;
// 队列等待条件锁
extern pthread_mutex_t msg_task_lock;
pthread_cond_t msg_task_cond = PTHREAD_COND_INITIALIZER;

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

//debug data
int previous = -1;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

typedef struct accept_req_params{
    Tox *m;
    const uint8_t *id;
    Msg_listener_list *msg_listener_list;
    
}accept_req_params;

void *accept_req_work(void *mparms){
    accept_req_params *parms = (accept_req_params *)mparms;
    accept_connect(parms->m,parms->id,parms->msg_listener_list);
	return NULL;
}

void friend_request(Tox *messenger, const uint8_t *public_key, const uint8_t *data, uint16_t length, void *userdata) {
    //接受請求
    uint8_t *str = (uint8_t *)malloc(sizeof(uint8_t)*(TOX_CLIENT_ID_SIZE*2));
    hex_bin_to_string(public_key,TOX_CLIENT_ID_SIZE,str);
    // start a new thread to init new connection
    pthread_t accept_req_thread;
    accept_req_params *req_parms = (accept_req_params *)malloc(sizeof(accept_req_params));
    req_parms->m = my_tox;
    req_parms->id = str;
    req_parms->msg_listener_list = msg_listener_list;
    
    // DATA CAN NOT BE RELEASED
    
    int iret1 = pthread_create( &accept_req_thread, NULL,accept_req_work,req_parms);
    
    if(iret1){
        exit(EXIT_FAILURE);
    }else{
        printf("TOXCORE:RECEIVED NEW REQ\n");
    }
}

void friend_message(Tox *m, int32_t friendnumber, const uint8_t *bin, uint16_t length, void *userdata) {
    // get remote id
    uint8_t client_id_bin[TOX_CLIENT_ID_SIZE+1];
    uint8_t client_id_str[TOX_CLIENT_ID_SIZE*2+1];
    tox_get_client_id(m,friendnumber,client_id_bin);
    hex_bin_to_string(client_id_bin,TOX_CLIENT_ID_SIZE,client_id_str);
    // 添加消息觸發器
    trigger_msg_listener(msg_listener_list,bin,client_id_str);
    // 添加消息隊列消息
    // 服务端模式处理
    if(MODE == 1){
        if(strcmp(bin,"HANDSHAKE") == 0){
            on_hand_shake_reveived(client_id_bin);
            return;
        }
        
        // parse message
        uint8_t *uuid = (uint8_t *)malloc(sizeof(uint8_t)*UUID_LENGTH+1);
        memset(uuid,'\0',UUID_LENGTH+1);
        uint8_t *cmd = (uint8_t *)malloc(sizeof(uint8_t)*128);
        uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t)*MY_MESSAGE_LENGTH);
        uint32_t *length = (uint32_t *)malloc(sizeof(uint32_t));
        unpack_msg_bin(bin, uuid, cmd, data, length);
        
        
        if(strcmp(cmd,"UNKNOWN_CMD") == 0){
            
        }else if(strcmp(cmd,"RAW_DATA") == 0){
            on_remote_data_received(uuid, data, *length, client_id_bin);
        }else{
            printf("CMD:%s\n",cmd);
            if(strcmp(cmd,"CLOSE_SOCK") == 0){
                int32_t sockfd = get_local_socks(msocks_list,uuid);
                close_local_socks(msocks_list,sockfd);
                printf("CLOSE OK\n");
            }else if(strcmp(cmd, "CREATE_SOCK") == 0){
                printf("CREATE SOCKET\n");
                json_error_t err;
                json_t *data_json = json_loads(data,0,&err);
                json_t *target_ip_json = json_object_get(data_json,"target_ip");
                json_t *target_port_json = json_object_get(data_json,"target_port");
                const uint8_t *ip_str = json_string_value(target_ip_json);
                int32_t port_int = (int32_t)json_integer_value(target_port_json);
                on_remote_create_sock_received(client_id_bin,
                                            uuid,
                                            ip_str,
                                            port_int);
                // free resource
                json_decref(target_ip_json);
                json_decref(target_port_json);
                json_decref(data_json);
            }
        }
        
        // free data
        free(uuid);
        free(cmd);
        free(data);
        free(length);
    }
    
    // 请求者模式
    if(MODE == 0){
        
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

/**
 * Server functions
 */

void on_hand_shake_reveived(const uint8_t *target_addr_bin){
    printf("HANDSHAKE RECEIVED\n");
    int32_t friend_num = tox_get_friend_number(my_tox,target_addr_bin);
    printf("FRIENDNUM:%d\n",friend_num);
	uint8_t addr_str[TOX_FRIEND_ADDRESS_SIZE * 2+1];
	hex_bin_to_string(target_addr_bin, TOX_CLIENT_ID_SIZE, addr_str);
	printf("ID:%s\n", addr_str);
    //return HANDSHAKE
    tox_send_message(my_tox,friend_num,"HANDSHAKE",strlen("HANDSHAKE"));
}


static Tox *init_tox(){
    Tox *m = tox_new(1);

    if (m == NULL) {
        m = tox_new(0);
    }
    /* Callbacks */
    tox_callback_connection_status(m, on_connectionchange, NULL);
    tox_callback_friend_request(m, friend_request, NULL);
    tox_callback_friend_message(m, friend_message, NULL);
    tox_callback_file_send_request(m, file_request_accept, NULL);
    tox_callback_file_control(m, file_print_control, NULL);
    tox_callback_file_data(m, write_file, NULL);

    tox_set_name(m, MY_NAME, strlen(MY_NAME)); // Sets the username
    return m;
}

int init_tox_connection(Tox *m)
{
    uint8_t pub_key[TOX_FRIEND_ADDRESS_SIZE+1];
    hex_string_to_bin(pub_key,BOOTSTRAP_KEY);
    int res = tox_bootstrap_from_address(m, BOOTSTRAP_ADDRESS, 0, htons(BOOTSTRAP_PORT), pub_key);
    if (!res) {
        exit(1);
    }
	return 0;
}



void *tox_works(void *a){
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
        if(msg_task_queue->size != 0){
            send_data_remote();
        }else{
#ifdef _WIN32
			Sleep(4);
#else
			usleep(4000);
#endif
            
        }
    }
}



void intHandler(int dummy) {
    store_data(my_tox);
#ifdef _WIN32
	WSACleanup();
#endif
    printf("EXITING...\n");
    exit(EXIT_SUCCESS);
}

#ifdef _WIN32
uint32_t init_local_sock_serv(uint32_t local_port){
	uint32_t iServerSock;
	struct sockaddr_in ServerAddr;
	WSADATA WSAData;

	if (WSAStartup(MAKEWORD(1, 1), &WSAData)){
		printf("initializationing error!\n");
		return 0;
	}

	if ((iServerSock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("create socket failed\n");
		return 0;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(local_port);//监视的端口号
	ServerAddr.sin_addr.s_addr = INADDR_ANY;//本地IP
	memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));

	if (bind(iServerSock, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) == -1){
		printf("bind failed!\n");
		return 0;
	}

	if (listen(iServerSock, 5) == -1){
		printf("listen failed!\n");
		return 0;
	}
	return iServerSock;
}

int init_local_sock(const uint8_t *ip, int local_port){
	int iClientSock;
	struct sockaddr_in ServerAddr;

	WSADATA WSAData;


	if (WSAStartup(MAKEWORD(1, 1), &WSAData)){
		printf("initializationing error!\n");
		return 0;
	}

	if ((iClientSock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("create socket failed!\n");
		return 0;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(local_port);
	ServerAddr.sin_addr.s_addr = inet_addr(ip);
	memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));

	if (connect(iClientSock, (struct sockaddr *) & ServerAddr, sizeof(struct sockaddr)) == -1){
		printf("connect failed\n");
		return 0;
	}

	return iClientSock;
}

#else

uint32_t init_local_sock(const uint8_t *hostname, const uint32_t local_port){
	struct sockaddr_in serv_addr;
	struct hostent *server;

	int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		return 0;
	server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		return 0;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(local_port);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		return 0;
	return sockfd;
}

uint32_t init_local_sock_serv(uint32_t local_port){
	int sockfd;
	struct sockaddr_in serv_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(local_port);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd, 5);
	return sockfd;
}

#endif

int32_t readCSock(uint32_t sockfd, uint8_t *buf, uint32_t length){
#ifdef _WIN32
	return recv(sockfd, buf, length, 0);
#else
	return read(sockfd, buf, length);
#endif
}

int32_t writeCSock(uint32_t sockfd, const uint8_t *buf, uint32_t length){
#ifdef _WIN32
	return send(sockfd, buf, length, 0);
#else
	return write(sockfd, buf, length);
#endif
}

int32_t closeCSock(uint32_t sockfd){
#ifdef _WIN32
	shutdown(sockfd, 2);
	int res = closesocket(sockfd);
	return res;
#else
	return shutdown(sockfd, 2);
#endif
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
void write_data_remote(const uint8_t *uuid, const uint8_t *target_addr_bin, const uint8_t* cmd, const uint8_t *data, const uint32_t length){
    // check data vaild
    if(target_addr_bin[0] == '\0'){
        printf("error msg.\n");
        exit(1);
    }
    
	// pack msg to bin
	uint8_t *msg_bin = (uint8_t *)malloc(sizeof(uint8_t)*MY_MESSAGE_LENGTH);
	pack_msg_bin(msg_bin, uuid, cmd, data, length);
	// add msg to message queue
	MSGTask *newTask = (MSGTask *)malloc(sizeof(MSGTask));
	uint8_t *mtarget_addr_bin = (uint8_t *)malloc(sizeof(uint8_t)*TOX_FRIEND_ADDRESS_SIZE);
	memcpy(mtarget_addr_bin, target_addr_bin, TOX_FRIEND_ADDRESS_SIZE);
	newTask->target_addr_bin = mtarget_addr_bin;
	newTask->msg = msg_bin;
    pthread_mutex_lock(&msg_task_lock);
	while ((msg_task_queue->size) >= MAX_MSG_CACHE){
        pthread_cond_wait(&msg_task_cond, &msg_task_lock);
	};
	// enter queue
	Enqueue(msg_task_queue, newTask);
    if((msg_task_queue->size) < MAX_MSG_CACHE)
        pthread_cond_broadcast(&msg_task_cond);
    pthread_mutex_unlock(&msg_task_lock);
	// free space
	free(msg_bin);
	free(mtarget_addr_bin);
	free(newTask);
}






/**
 * read message from remote message queue, and send to remote
 * this will only send one message in the message queue
 * and write_data_remote will be block at the same time
 */
void send_data_remote(){
    MSGTask *mTask = front(msg_task_queue); // 开始处理
    int friend_num = tox_get_friend_number(my_tox,mTask->target_addr_bin);
    int res = -1;
    int retry_count = 0;
    while(res <=0 && retry_count <5){
        res = tox_send_message(my_tox,friend_num,mTask->msg,MY_MESSAGE_LENGTH);
        retry_count += 1;
    }
    if(retry_count < 5){
        Dequeue(msg_task_queue);
        pthread_cond_broadcast(&msg_task_cond);
    }else{
        // check target online status
        if(tox_get_friend_connection_status(my_tox,friend_num) != 1){
            // target offline
            Dequeue(msg_task_queue);
            pthread_cond_broadcast(&msg_task_cond);
            // unpack msg to get uuid
            uint8_t *uuid = (uint8_t *)malloc(sizeof(uint8_t)*UUID_LENGTH+1);
            memset(uuid,'\0',UUID_LENGTH+1);
            uint8_t *cmd = (uint8_t *)malloc(sizeof(uint8_t)*128);
            uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t)*MY_MESSAGE_LENGTH);
            uint32_t *length = (uint32_t *)malloc(sizeof(uint32_t));
            unpack_msg_bin(mTask->msg, uuid, cmd, data, length);
            // get socket fd by uuid
            int32_t sockfd = get_local_socks(msocks_list,uuid);
            // close socket
            close_local_socks(msocks_list,sockfd);
            free(uuid);
            free(cmd);
            free(data);
            free(length);
        }
    }
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
    uint8_t uuid[UUID_LENGTH+1] = {'\0'};
    get_local_socks_uuid(msocks_list,sockfd,uuid);
    if(uuid[0] == '\0'){
        closeCSock(sockfd);
        return; // no record found
    }
    uint8_t target_addr_bin[TOX_FRIEND_ADDRESS_SIZE +1];
    get_local_socks_addr_bin(msocks_list,sockfd, target_addr_bin);
    if(target_addr_bin[0] == '\0'){
        closeCSock(sockfd);
        return;
    }
    if(uuid[0] == '\0'){
        printf("on_local_data_received error\n");
        exit(1);
    }
    write_data_remote(uuid,target_addr_bin,NULL,data,length);
}

/**
 * this func will be called immediately after remote data received
 * NOTE: remote data is not remote message. it's remote message with fixed format
 * {
 *  uuid
 *  data
 * }
 */
void on_remote_data_received(const uint8_t *uuid, const uint8_t *data, const uint32_t length, const uint8_t *client_id_bin){
    // get sockfd from uuid
    
    uint32_t sockfd = get_local_socks(msocks_list,uuid);
    
    // send data to target socket
    if(sockfd != 0){
        // record found
        writeCSock(sockfd,data,length);
    }else{
        // socket might be closed 
        printf("INVALID SOCKET, CLOSE IT\n");
		printf("uuid:%s\n", uuid);
		print_local_socks_list(msocks_list);
        close_remote_socket(uuid,client_id_bin);
    }
}

/**
 * close remote socket
 */

void close_remote_socket(const uint8_t *uuid, const uint8_t *client_id_bin){
    if(uuid[0] == '\0'){
        printf("close_remote_socket error\n");
        exit(1);
    }
    write_data_remote(uuid,client_id_bin,"CLOSE_SOCK","",strlen(""));
}

void send_create_success(const uint8_t *uuid, const uint8_t *client_id_bin){
    if(uuid[0] == '\0'){
        printf("send_create_success error\n");
        exit(1);
    }
    write_data_remote(uuid,client_id_bin,"CREATE_SOCK_SUCCESS","",strlen(""));
}

void create_remote_socket(const uint8_t *uuid, const uint8_t *client_id_bin,const uint8_t *target_ip, uint32_t target_port){
    json_t *target_port_json = json_pack("i",target_port);
    json_t *target_ip_json = json_pack("s",target_ip);
    json_t *data = json_object();
    json_object_set(data,"target_ip",target_ip_json);
    json_object_set(data,"target_port",target_port_json);
    uint8_t *data_str = json_dumps(data,JSON_INDENT(4));
    if(uuid[0] == '\0'){
        printf("create_remote_socket error \n");
        exit(1);
    }
    write_data_remote(uuid,client_id_bin,"CREATE_SOCK",data_str,strlen(data_str));
    
}

void on_remote_create_sock_received(const uint8_t *target_addr_bin, const uint8_t *uuid, const uint8_t *target_ip, uint32_t target_port){
    int sockfd = init_local_sock(target_ip,target_port);
    if(sockfd == 0){
        //sock create failed
        printf("CREATE LOCAL SOCKET FAILED\n");
        close_remote_socket(uuid,target_addr_bin);
        return;
    }
    // create local sock
    add_local_socks(msocks_list,sockfd,target_addr_bin,target_ip,target_port);
    if(set_local_socks_uuid(msocks_list,sockfd,uuid) == 1){
		printf("uuid:%s\n", uuid);
        printf("SOCKET CREATED\n");
        // start a new thread read sock
        pthread_t new_sock_thread;
        uint32_t *msockfd = (uint32_t *)malloc(sizeof(uint32_t));
        *msockfd = (uint32_t)sockfd;
        pthread_create( &new_sock_thread, NULL, on_local_sock_connect,msockfd);
    }
}

void *on_local_sock_connect(void *msockfd){
    uint32_t sockfd = *((uint32_t *)msockfd);
    uint8_t target_addr_bin_temp[TOX_FRIEND_ADDRESS_SIZE+1];
    get_local_socks_addr_bin(msocks_list,sockfd,target_addr_bin_temp);
    uint8_t target_addr_bin[TOX_FRIEND_ADDRESS_SIZE+1] = {'\0'};
    if(target_addr_bin_temp == NULL){
        printf("ERROR**********************\n");
    }
    uint8_t test[TOX_FRIEND_ADDRESS_SIZE*2+1];
    hex_bin_to_string(target_addr_bin_temp,TOX_FRIEND_ADDRESS_SIZE,test);
    memcpy(target_addr_bin,target_addr_bin_temp,TOX_FRIEND_ADDRESS_SIZE);
    uint8_t uuid_temp[UUID_LENGTH + 1] = {'\0'};
    get_local_socks_uuid(msocks_list,sockfd,uuid_temp);
    if(uuid_temp[0] == '\0'){
        // closed before read.
        printf("closed before read.\n");
        return;
    }
    uint8_t uuid[UUID_LENGTH+1] = {'\0'};
	memcpy(uuid, uuid_temp, UUID_LENGTH);
    // 发送端口创建成功
    send_create_success(uuid,target_addr_bin);
    uint8_t buf[SOCK_BUF_SIZE+1];
    int length = 1;
    while(length > 0){
        memset(buf,0,SOCK_BUF_SIZE);
        length = readCSock(sockfd,buf,SOCK_BUF_SIZE-1);
        //printf("LENGTH:%d\n",length);
        if(length > 0)
            on_local_data_received(buf,length,sockfd);
    }
    // read data error
    // close remote and local sock
    
    // block until all all the sock message send
    if(get_local_socks(msocks_list, uuid) != 0){
        close_remote_socket(uuid,target_addr_bin);
        printf("close remote socket\n");
    }
        
    close_local_socks(msocks_list,sockfd);
    free(msockfd);
	return 0;
}


int main(int argc, char *argv[])
{
    // 添加系统事件监听
    signal(SIGINT, intHandler);
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
    
    my_tox = init_tox();
    init_tox_connection(my_tox);
    load_data(my_tox);
    
    if(argc >3){
        target_id = argv[2];
        target_ip = argv[3];
        target_port = atoi(argv[4]);
    }else{
        target_id = NULL;
        uint8_t my_address_bin[TOX_FRIEND_ADDRESS_SIZE+1];
        uint8_t my_address_str[TOX_FRIEND_ADDRESS_SIZE*2+1];
        tox_get_address(my_tox,my_address_bin);
        hex_bin_to_string(my_address_bin,TOX_FRIEND_ADDRESS_SIZE,my_address_str);
        printf("MYID:%s\n",my_address_str);
    }
    // 虛擬參數
    
    // 初始化消息隊列
    msg_task_queue = createQueue(MAX_MSG_CACHE); // 远程操作消息队列
    
    // 開始tox線程
    pthread_t tox_thread;
    int iret1 = pthread_create( &tox_thread, NULL, tox_works,NULL);
    if(iret1){
        exit(EXIT_FAILURE);
    }
    
    
    // 等待tox成功連接
    while(!tox_isconnected(my_tox)){
#ifdef _WIN32
		Sleep(20);
#else
		usleep(20000);
#endif
        
    }
    printf("TOXCORE:ONLINE\n");

    // create local sockets list
    msocks_list = create_local_socks_list();
    MODE = 1;
    while(1){
#ifdef _WIN32
		Sleep(1000);
#else
		usleep(1000000);
#endif
    }
    
    return 0; 
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
