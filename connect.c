#include "connect.h"


/**
 * wrap message in json format
 */
uint8_t *send_message_str(const uint8_t *client_id_str, const uint8_t *msg){
    json_t *msg_json = json_object();
    int err;
    err = json_object_set(msg_json,"from",json_string(client_id_str));
    mjson_error(err);
    err = json_object_set(msg_json,"msg",json_string(msg));
    mjson_error(err);
    return json_dumps(msg_json,JSON_INDENT(4));
}


/**
 * init a tox connection
 */
int init_connect(Tox *m, const uint8_t *address_str, Msg_listener_list **msg_listener_list,uint32_t sockfd){
    // check online state
    if(!tox_isconnected(m)){
        printf("401\n");
        return 401;
    }
    uint8_t client_id_str[TOX_CLIENT_ID_SIZE*2];
    address_str_to_client_str(address_str,client_id_str);
    // add friend
    // check if already a friend
    uint32_t friendnum;
    if(tox_get_friend_number(m,hex_string_to_bin(client_id_str)) == -1){
        friendnum = tox_add_friend(m,hex_string_to_bin(address_str),"HANDSHAKE",strlen("HANDSHAKE"));
        if(friendnum < 0){
            printf("%d\n",110+friendnum);
            return 110+friendnum;
        };
    }
    friendnum = tox_get_friend_number(m,hex_string_to_bin(client_id_str));
    uint8_t temp_str[100];
    sprintf(temp_str,"friendNum:%d\n",friendnum);
    write_local_message(sockfd,temp_str);
    //printf("friendnum:%d\n",friendnum);
    // check target online state
    uint32_t retryCount = 0;
    while( tox_get_friend_connection_status(m,friendnum) != 1 && retryCount <100){
        usleep(1000000);
        retryCount ++;
        printf("online num:%d\n",tox_get_num_online_friends(m));
        int i;
        printf("Total friend num:%d\n",tox_count_friendlist(m));
        for(i=0;i<tox_count_friendlist(m);i++){
            printf("online status %d:%d",i,tox_get_friend_connection_status(m,i));
        }
        printf("wait online:%d\n",tox_get_friend_connection_status(m,friendnum));
    }
    
    
    if(tox_get_friend_connection_status(m,friendnum)){
        uint8_t my_addr_str[TOX_FRIEND_ADDRESS_SIZE*2];
        uint8_t my_id_str[TOX_CLIENT_ID_SIZE*2];
        //get_my_client_id_str(m, my_addr_str);
        //address_str_to_client_str(my_addr_str,my_id_str);
        //uint8_t *handshake_msg = send_message_str(my_id_str,"HANDSHAKE");
        uint32_t res = tox_send_message(m,friendnum,"HANDSHAKE",strlen("HANDSHAKE"));
        if(res < 0){
            printf("%d\n",210 + res);
            return 210 +res;
        }
        // wait for replay
        uint32_t timeout = 0; // timeout 4s
        while( is_message_received(msg_listener_list,"HANDSHAKE",client_id_str) == 0 && timeout< 20){
            usleep(200000);
            timeout ++;
        }
        if(is_message_received(msg_listener_list,"HANDSHAKE",client_id_str)){
            // handShake received
            printf("402\n");
            return 402;
        }
        // handShake failed
        printf("401\n");
        return 401;
    }
    // target offline
    printf("401\n");
    return 401;
}

/**
 * accept a connection
 */
int accept_connect(Tox *m,const uint8_t *client_id_str,Msg_listener_list *msg_listener_list,uint32_t sockfd){
    // check online state
    if(!tox_isconnected(m)){
        printf("401\n");
        return 401;
    }
    // add friend
    // check if already a friend
    uint32_t friendnum = 0;
    if(tox_get_friend_number(m,hex_string_to_bin(client_id_str)) == -1){
        // not a friend
        friendnum = tox_add_friend_norequest(m,hex_string_to_bin(client_id_str));
        if(friendnum < 0){
            printf("%d\n",110+friendnum);
            return 110+friendnum;
        }
    }
    friendnum = tox_get_friend_number(m,hex_string_to_bin(client_id_str));
    uint8_t temp_str[100];
    sprintf(temp_str,"ADD_FRIEND:%d\n",friendnum);
    write_local_message(sockfd,temp_str);
    // wait for handshake message
    uint32_t timeout = 0; // timeout 4s
    while(is_message_received(&msg_listener_list,"HANDSHAKE",client_id_str) == 0  && timeout< 100){
        printf("wait online\n");
        usleep(1000000);
        printf("online num:%d\n",tox_get_num_online_friends(m));
        int i;
        printf("Total friend num:%d\n",tox_count_friendlist(m));
        for(i=0;i<tox_count_friendlist(m);i++){
            printf("online status %d:%d",i,tox_get_friend_connection_status(m,i));
        }
        printf("wait online:%d\n",tox_get_friend_connection_status(m,friendnum));
        timeout ++;
    }
    if(is_message_received(&msg_listener_list,"HANDSHAKE",client_id_str)){
        // handShake received
        printf("402\n");
        return 402;
    }
    // handShake failed
    printf("401\n");
    return 401;
}

