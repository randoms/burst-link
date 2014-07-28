#ifndef _CONNECT_H
#define _CONNECT_H

#include "jansson.h"
#include "toxcore/tox.h"
#include "utils/utils.h"
#include "message_listener.h"

int init_connect(Tox *m, const uint8_t *id, Msg_listener_list **msg_listener_list);
int accept_connect(Tox *m,const uint8_t *id,Msg_listener_list *msg_listener_list);

#endif