#include "message_listener.h"

Msg_listener_list *create_msg_listener_list(){
    Msg_listener_list *mlist = (Msg_listener_list *)malloc(sizeof(Msg_listener_list));
    mlist->head = NULL;
    mlist->size = 0;
    return mlist;
}


Msg_listener_node *add_msg_listener_list(Msg_listener_list *mlist, MsgListener *msg_listener){
    // add a new record to the head of list
    MsgListener *new_record = (MsgListener *)malloc(sizeof(MsgListener));
    new_record->msg = msg_listener->msg;
    new_record->id = msg_listener->id;
    new_record->is_received = msg_listener->is_received;
    Msg_listener_node *new_header = (Msg_listener_node *)malloc(sizeof(Msg_listener_node));
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

void remove_msg_listener_list(Msg_listener_list *mlist, Msg_listener_node *mnode){
    Msg_listener_node *previous_node = mnode->previous;
    Msg_listener_node *after_node = mnode->after;
    
    if(previous_node != NULL){
        previous_node->after = after_node;
    }else{
        // delete a head node
        mlist->head = after_node;
    }
    if(after_node != NULL){
        after_node->previous = previous_node;
    }
    free(mnode);
    mlist->size -= 1;
}

/**
 * print the content of message list
 */
void print_msg_listener_list(Msg_listener_list *mlist){
    if(mlist->head == NULL){
        printf("NULL\n");
        return;
    }
    Msg_listener_node temp = *(mlist->head);
    MsgListener temp_lisener = *(temp.me);
    while(temp.after != NULL){
        temp_lisener = *(temp.me);
        printf("message listener ********\n");
        printf("message:%s\n",temp_lisener.msg);
        printf("id:%s\n",temp_lisener.id);
        printf("is_received:%d\n",temp_lisener.is_received);
        temp = *(temp.after);
    }
    
    // print last one
    temp_lisener = *(temp.me);
    printf("message listener **********\n");
    printf("message:%s\n",temp_lisener.msg);
    printf("id:%s\n",temp_lisener.id);
    printf("is_received:%d\n",temp_lisener.is_received);
    
}

uint8_t is_message_received(Msg_listener_list **msg_listener_list, const uint8_t *msg, const uint8_t *id){
    // init
    if(*msg_listener_list == NULL){
        MsgListener *mlistener = (MsgListener *)malloc(sizeof(MsgListener));
        mlistener->msg = msg;
        mlistener->id = id;
        mlistener->is_received = 0;
        *msg_listener_list = create_msg_listener_list();
        add_msg_listener_list(*msg_listener_list,mlistener);
        free(mlistener);
        return 0;
    }else{
        // check exist
        Msg_listener_node *temp = (*msg_listener_list)->head;
        MsgListener *temp_lisener;
        while(temp->after != NULL){
            temp_lisener = temp->me;
            if(strcmp(temp_lisener->msg,msg) == 0 && strcmp(temp_lisener->id,id) == 0){
                // already added
                int res = temp_lisener->is_received;
                return res;
            }
            temp = temp->after;
        }
        // check the last one
        temp_lisener = temp->me;
        if(strcmp(temp_lisener->msg,msg) == 0 && strcmp(temp_lisener->id,id) == 0){
            // already added;
            int res = temp_lisener->is_received;
            return res;
        }
        
        // first time add
        temp_lisener = (MsgListener *)malloc(sizeof(MsgListener));
        temp_lisener->msg = msg;
        temp_lisener->id = id;
        temp_lisener->is_received = 0;
        add_msg_listener_list(*msg_listener_list,temp_lisener);
        free(temp_lisener);
        return 0;
    }
}

void trigger_msg_listener(Msg_listener_list *msg_listener_list,const uint8_t *msg, const uint8_t *id){
    // try to find if this message is registered
    if(msg_listener_list == NULL)return;
    Msg_listener_node *temp = msg_listener_list->head;
    MsgListener *temp_lisener;
    while(temp->after != NULL){
        temp_lisener = temp->me;
        if(strcmp(temp_lisener->msg,msg) == 0 && strcmp(temp_lisener->id,id) == 0){
            // already added
            temp_lisener->is_received = 1;
            return;
        }
        temp = temp->after;
    }
    // check the last one
    temp_lisener = temp->me;
    if(strcmp(temp_lisener->msg,msg) == 0 && strcmp(temp_lisener->id,id) == 0){
        // already added
        temp_lisener->is_received = 1;
        return;
    }
}

void trigger_msg_listener_debug(Msg_listener_list *msg_listener_list,const uint8_t *msg, const uint8_t *id,uint32_t sockfd){
    // try to find if this message is registered
    if(msg_listener_list == NULL)return;
    Msg_listener_node *temp = msg_listener_list->head;
    MsgListener *temp_lisener;
    while(temp->after != NULL){
        temp_lisener = temp->me;
        if(strcmp(temp_lisener->msg,msg) == 0 && strcmp(temp_lisener->id,id) == 0){
            // already added
            temp_lisener->is_received = 1;
            return;
        }
        temp = temp->after;
    }
    // check the last one
    temp_lisener = temp->me;
    if(strcmp(temp_lisener->msg,msg) == 0 && strcmp(temp_lisener->id,id) == 0){
        // already added
        temp_lisener->is_received = 1;
        return;
    }
}

// int main(void){
//     Msg_listener_list *msg_listener_list = NULL;
//     while(1){
//         is_message_received(&msg_listener_list, "HELLO","one");
//         is_message_received(&msg_listener_list,"HELLO","two");
//         msg_listener_list->size;
//         //print_msg_listener_list(msg_listener_list);
//         int res = is_message_received(&msg_listener_list,"HELLO","two");
//         //printf("two:%d\n",res);
//         trigger_msg_listener(msg_listener_list,"HELLO","two");
//         res = is_message_received(&msg_listener_list,"HELLO","two");
//         //printf("two:%d\n",res);
//         res = is_message_received(&msg_listener_list,"HELLO","two");
//         //printf("two:%d\n",res);
//         usleep(1000);
//     }
//     
//     
// }