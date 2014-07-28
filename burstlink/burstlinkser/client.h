#ifndef _CLIENT_H
#define _CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#include "pthread.h"
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#endif


#include <signal.h>
#include <stdbool.h>
#include <time.h>

#include "jansson.h"

#include "toxcore/tox.h"
#include "utils/utils.h"
#include "defines.h"
#include "queue.h"
#include "message_listener.h"
#include "connect.h"
#include "local_socks.h"

#define MY_NAME "BurstLink_SERV"

/**
 * read message from remote message queue, and send to remote
 * this will only send one message in the message queue
 * and write_data_remote will be block at the same time
 */
void write_data_remote(const uint8_t *uuid, const uint8_t *target_addr_bin, const uint8_t* cmd, const uint8_t *data, const uint32_t length);
void send_data_remote();
void close_remote_socket(const uint8_t *uuid, const uint8_t *client_id_bin);
void on_hand_shake_reveived(const uint8_t *target_addr_bin);
void on_init_req_received(const uint8_t *target_addr_bin);
void on_remote_data_received(const uint8_t *uuid, const uint8_t *data, const uint32_t length, const uint8_t *client_id_bin);
void on_remote_create_sock_received(const uint8_t *target_addr_bin, const uint8_t *uuid, const uint8_t *target_ip, uint32_t target_port);
void *on_local_sock_connect(void *msockfd);
void debug_data(const uint8_t *data,uint32_t length);
void debug_msg_bin(const uint8_t *msg_bin);
#endif