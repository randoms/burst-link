#include "local_socks.h"

local_socks_list *create_local_socks_list(){
    local_socks_list *mlist = (local_socks_list *)malloc(sizeof(local_socks_list));
    mlist->head = NULL;
    mlist->size = 0;
    return mlist;
}


local_socks_node *add_local_socks_list(local_socks_list *mlist, local_socks *mlocal_socks){
    // add a new record to the head of list
    local_socks *new_record = (local_socks *)malloc(sizeof(local_socks));
    new_record->uuid = mlocal_socks->uuid;
    new_record->sock = mlocal_socks->sock;
    new_record->target_port = mlocal_socks->target_port;
    new_record->target_ip = mlocal_socks->target_ip;
    new_record->target_addr_bin = mlocal_socks->target_addr_bin;
    
    local_socks_node *new_header = (local_socks_node *)malloc(sizeof(local_socks_node));
    new_header->previous = NULL;
    if(mlist->head != NULL){
        new_header->after = mlist->head;
    }else{
        new_header->after = NULL;
    }
    
    new_header->me = new_record;
    // change previous header
    if(mlist->head != NULL)
        (mlist->head)->previous = new_header;
    // change new head
    mlist->head = new_header;
    mlist->size += 1;
    return new_header;
}

void remove_local_socks_list(local_socks_list *mlist, local_socks_node *mnode){
    local_socks_node *previous_node = mnode->previous;
    local_socks_node *after_node = mnode->after;
    
    if(previous_node != NULL){
        previous_node->after = after_node;
    }else{
        // delete a head node
        mlist->head = after_node;
    }
    if(after_node != NULL){
        after_node->previous = previous_node;
    }
    free(mnode->me->target_ip);
    free(mnode->me->target_addr_bin);
    free(mnode->me->uuid);
    free(mnode->me);
    free(mnode);
    mlist->size -= 1;
}

/**
 * print the content of message list
 */
void print_local_socks_list(local_socks_list *mlist){
    if(mlist->head == NULL){
        printf("NULL\n");
        return;
    }
    local_socks_node temp = *(mlist->head);
    local_socks temp_record = *(temp.me);
    while(temp.after != NULL){
        temp_record = *(temp.me);
        printf("local sockets listener ********\n");
        printf("socket:%d\n",temp_record.sock);
        printf("uuid:%s\n",temp_record.uuid);
        temp = *(temp.after);
    }
    
    // print last one
    temp_record = *(temp.me);
    printf("local sockets listener ********\n");
    printf("socket:%d\n",temp_record.sock);
    printf("uuid:%s\n",temp_record.uuid);
    
}

void add_local_socks(local_socks_list *mlist, uint32_t sockfd, const uint8_t *target_addr_bin, const uint8_t *target_ip, uint32_t target_port){
    local_socks *mlocal_socks = (local_socks *)malloc(sizeof(local_socks));
    uint8_t *uuid_str = (uint8_t *)malloc(sizeof(uint8_t)*36);
    uint8_t *mtarget_addr_bin = (uint8_t *)malloc(sizeof(uint8_t)*TOX_FRIEND_ADDRESS_SIZE);
    bufcopy(mtarget_addr_bin,target_addr_bin,TOX_FRIEND_ADDRESS_SIZE);
    uint8_t *mtarget_ip = (uint8_t *)malloc(sizeof(uint8_t)*16);
    bufcopy(mtarget_ip,target_ip,strlen(target_ip));
    uuid_t muuid;
    uuid_generate(muuid);
    uuid_unparse(muuid,uuid_str);
    mlocal_socks->uuid = uuid_str;
    mlocal_socks->sock = sockfd;
    mlocal_socks->ready_flag = 0;
    mlocal_socks->target_addr_bin = mtarget_addr_bin;
    mlocal_socks->target_ip = mtarget_ip;
    mlocal_socks->target_port = target_port;
    add_local_socks_list(mlist,mlocal_socks);
    free(mlocal_socks);
}

void close_local_socks(local_socks_list *mlist, uint32_t sockfd){
    printf("CLOSING LOCAL SOCKET\n");
    shutdown(sockfd,2);
    if(mlist == NULL)return;
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if((temp_sock->sock) == sockfd){
            remove_local_socks_list(mlist, temp);
            return;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if((temp_sock->sock) == sockfd){
        remove_local_socks_list(mlist, temp);
        return;
    }
}

void close_local_socks_uuid(local_socks_list *mlist, const uint8_t *uuid){
    printf("CLOSING LOCAL SOCKET\n");
    if(mlist == NULL)return;
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if(strcmp(temp_sock->uuid,uuid) == 0){
            shutdown(temp_sock->sock,2);
            remove_local_socks_list(mlist, temp);
            return;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if(strcmp(temp_sock->uuid,uuid) == 0){
        shutdown(temp_sock->sock,2);
        remove_local_socks_list(mlist, temp);
        return;
    }
}


uint32_t get_local_socks(local_socks_list *mlist, const uint8_t *uuid){
    if(mlist == NULL)return 0;
    if(mlist->size == 0)return 0;
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if(strcmp(temp_sock->uuid,uuid) == 0){
            return temp_sock->sock;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if(strcmp(temp_sock->uuid,uuid) == 0){
        return temp_sock->sock;
    }
    // not found set to 0
    return 0;
}


uint8_t *get_local_socks_uuid(local_socks_list *mlist, uint32_t sockfd){
    if(mlist == NULL)return NULL;
    if(mlist->size == 0)return NULL;
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if((temp_sock->sock) == sockfd){
            return temp_sock->uuid;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if((temp_sock->sock) == sockfd){
        return temp_sock->uuid;
    }
    // not found set to null
    return NULL;
}

uint32_t set_local_socks_uuid(local_socks_list *mlist, uint32_t sockfd, const uint8_t *uuid){
    if(mlist == NULL)return 0;
    if(mlist->size == 0)return 0;
    if(strlen(uuid) != 36)return 0; // uuid format error
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if((temp_sock->sock) == sockfd){
            strcpy(temp_sock->uuid,uuid);
            return 1;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if((temp_sock->sock) == sockfd){
        strcpy(temp_sock->uuid,uuid);
        return 1;
    }
    // not found set to null
    return 0;
}

const uint8_t *get_local_socks_addr_bin(local_socks_list *mlist, uint32_t sockfd){
    if(mlist == NULL)return 0;
    if(mlist->size == 0)return 0;
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if((temp_sock->sock) == sockfd){
            return temp_sock->target_addr_bin;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if((temp_sock->sock) == sockfd){
        return temp_sock->target_addr_bin;
    }
    // not found set to null
    return 0;
}

int set_local_socks_ready(local_socks_list *mlist, uint32_t sockfd){
    if(mlist == NULL)return 0;
    if(mlist->size == 0)return 0;
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if((temp_sock->sock) == sockfd){
            temp_sock->ready_flag = 1;
            return 1;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if((temp_sock->sock) == sockfd){
        temp_sock->ready_flag = 1;
        return 1;
    }
    // not found set to null
    return 0;
}

int is_local_socks_ready(local_socks_list *mlist, uint32_t sockfd){
    if(mlist == NULL)return 0;
    if(mlist->size == 0)return 0;
    local_socks_node *temp = mlist->head;
    local_socks *temp_sock;
    while(temp->after != NULL){
        temp_sock = temp->me;
        if((temp_sock->sock) == sockfd){
            return temp_sock->ready_flag;
        }
        temp = temp->after;
    }
    // check the last one
    temp_sock = temp->me;
    if((temp_sock->sock) == sockfd){
        return temp_sock->ready_flag;
    }
    // not found set to null
    return 0;
}

// int main(void){
//     local_socks_list *mlist = create_local_socks_list();
//     while(1){
//         add_local_socks(mlist,21321);
//         add_local_socks(mlist,12321);
//         close_local_socks(mlist,12321);
//         close_local_socks(mlist,21321);
//         usleep(10);
//     }
//     
// }