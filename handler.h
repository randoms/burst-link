#ifndef _HANDLER_H
#define _HANDLER_H

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

#include <jansson.h>
#include "toxcore/tox.h"
#include "utils/utils.h"
/**
 * call this when new message arrives
 */
void mtox_on_message(char *msg);


/**
 * call this when want to send message
 */
int mtox_send_message(Tox *m, const char *friendID, int sock,json_t *message);

/**
 * call to connect to server
 */
void mtox_connect(char *target_info);


/**
 * call to add a friend
 */
int mtox_friend_add(Tox *m, const char *friendID, int sock);
int mtox_friend_accept(Tox *m, const char *friendID,int sock);

#endif