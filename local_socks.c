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

void add_local_socks(local_socks_list *mlist, uint32_t sockfd){
    local_socks *mlocal_socks = (local_socks *)malloc(sizeof(local_socks));
    uint8_t *uuid_str = (uint8_t *)malloc(sizeof(uint8_t)*36);
    uuid_t muuid;
    uuid_generate(muuid);
    uuid_unparse(muuid,uuid_str);
    mlocal_socks->uuid = uuid_str;
    mlocal_socks->sock = sockfd;
    add_local_socks_list(mlist,mlocal_socks);
    free(mlocal_socks);
}

void close_local_socks(local_socks_list *mlist, uint32_t sockfd){
    close(sockfd);
    // try to find if this message is registered
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

uint32_t get_local_socks(local_socks_list *mlist, uint8_t *uuid){
    if(mlist == NULL)return 0;
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
    if(mlist == 0)return;
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