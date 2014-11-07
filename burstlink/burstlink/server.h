#ifndef _SERVER_H
#define _SERVER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>

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


/*
#define BOOTSTRAP_ADDRESS "109.169.46.133"
#define BOOTSTRAP_PORT 33445
#define BOOTSTRAP_KEY "7F31BFC93B8E4016A902144D0B110C3EA97CB7D43F1C4D21BCAE998A7C838821"
*/


/**
 * read message from remote message queue, and send to remote
 * this will only send one message in the message queue
 * and write_data_remote will be block at the same time
 */
void send_data_remote();
void close_remote_socket(const uint8_t *uuid, const uint8_t *client_id_bin);
void on_remote_data_received(const uint8_t *uuid, const uint8_t *data, const uint32_t length, const uint8_t *client_id_bin);
void debug_data(const uint8_t *data,uint32_t length);
void debug_msg_bin(uint8_t *msg_bin);
int32_t closeCSock(uint32_t sockfd);
void *on_remote_sock_created(void *msockfd);
#endif