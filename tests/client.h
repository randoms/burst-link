#ifndef _CLIENT_H
#define _CLIENT_H

#define SOCK_BUF_SIZE 1024
#define UUID_LENGTH 36
#define CMD_LENGTH 1
#define MESSAGE_LENGTH_BYTE 2
#define MY_MESSAGE_LENGTH UUID_LENGTH + CMD_LENGTH + MESSAGE_LENGTH_BYTE + SOCK_BUF_SIZE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <jansson.h>

#include "toxcore/tox.h"
#include "utils/utils.h"
#include "queue.h"
#include "route.h"
#include "message_listener.h"
#include "connect.h"
#include "local_socks.h"

#define BOOTSTRAP_ADDRESS "42.96.195.88"
#define BOOTSTRAP_PORT 33445
#define BOOTSTRAP_KEY "7F613A23C9EA5AC200264EB727429F39931A86C39B67FC14D9ECA4EBE0D37F25"
#define MAX_MSG_CACHE 128

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
void on_remote_data_received(const uint8_t *data, const uint8_t *client_id_bin);
void on_remote_create_sock_received(const uint8_t *target_addr_bin, const uint8_t *uuid, const uint8_t *target_ip, uint32_t target_port);
void *on_local_sock_connect(void *msockfd);
#endif