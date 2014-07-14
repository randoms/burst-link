#ifndef _SERVER_H
#define _SERVER_H

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

#endif