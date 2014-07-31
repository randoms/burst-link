#ifndef _LOCAL_SOCKS_H
#define _LOCAL_SOCKS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h> 
#ifdef _WIN32
#include <rpc.h>
#else
#include <unistd.h>
#include <uuid/uuid.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#endif
#include "defines.h"
#include "toxcore/tox.h"
#include "utils/utils.h"

typedef struct local_socks{
    uint8_t *uuid;
    uint32_t sock;
    uint32_t ready_flag;
    uint32_t target_port;
    uint8_t *target_ip;
    uint8_t *target_addr_bin;
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
void add_local_socks(local_socks_list *mlist, uint32_t sockfd,const uint8_t *target_addr_bin, const uint8_t *target_ip, uint32_t target_port);
void close_local_socks(local_socks_list *mlist, uint32_t sockfd);
uint32_t get_local_socks(local_socks_list *mlist,const uint8_t *uuid);
uint8_t *get_local_socks_uuid(local_socks_list *mlist, uint32_t sockfd);
uint32_t set_local_socks_uuid(local_socks_list *mlist, uint32_t sockfd, const uint8_t *uuid);
const uint8_t *get_local_socks_addr_bin(local_socks_list *mlist, uint32_t sockfd);
int set_local_socks_ready(local_socks_list *mlist, uint32_t sockfd);
int is_local_socks_ready(local_socks_list *mlist, uint32_t sockfd);
int debug_local_socket(local_socks_list *mlist);
void print_local_socks_list(local_socks_list *mlist);
#endif