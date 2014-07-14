#ifndef _MESSAGE_LISTENER_H
#define _MESSAGE_LISTENER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include "toxcore/tox.h"

typedef struct MsgListener{
    const uint8_t *msg;
    const uint8_t *id;
    uint8_t is_received;
}MsgListener;


struct Msg_listener_node{
    struct Msg_listener_node *previous;
    struct Msg_listener_node *after;
    MsgListener *me;
};

typedef struct Msg_listener_node Msg_listener_node;

typedef struct Msg_listener_list{
    Msg_listener_node *head;
    uint32_t size;
}Msg_listener_list;

Msg_listener_list *create_msg_listener_list();
Msg_listener_node *add_msg_listener_list(Msg_listener_list *mlist, MsgListener *msg_listener);
void remove_msg_listener_list(Msg_listener_list *mlist, Msg_listener_node *mnode);
void print_msg_listener_list(Msg_listener_list *mlist);
uint8_t is_message_received(Msg_listener_list **msg_listener_list,const uint8_t *msg, const uint8_t *id);
void trigger_msg_listener(Msg_listener_list *msg_listener_list, const uint8_t *msg, const uint8_t *id);

#endif