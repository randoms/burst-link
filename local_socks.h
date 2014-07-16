#ifndef _LOCAL_SOCKS_H
#define _LOCAL_SOCKS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <uuid/uuid.h>
#include "toxcore/tox.h"

typedef struct local_socks{
    uint8_t *uuid;
    uint32_t sock;
}local_socks;


struct local_socks_node{
    struct local_socks_node *previous;
    struct local_socks_node *after;
    local_socks *me;
};

typedef struct local_socks_node local_socks_node;

typedef struct local_socks_list{
    local_socks_node *head;
    uint32_t size;
}local_socks_list;

local_socks_list *create_local_socks_list();
void add_local_socks(local_socks_list *mlist, uint32_t sockfd);
void close_local_socks(local_socks_list *mlist, uint32_t sockfd);
uint32_t get_local_socks(local_socks_list *mlist, uint8_t *uuid);
uint8_t *get_local_socks_uuid(local_socks_list *mlist, uint32_t sockfd);

#endif