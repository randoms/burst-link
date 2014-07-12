#include "connect.h"


/**
 * init a tox connection
 */
int init_connect(Tox *m, const uint8_t *id, Msg_listener_list *msg_listener_list){
    // check online state
    if(!tox_isconnected(m)){
        printf("401\n");
        return 401;
    }
    // add friend
    // check if already a friend
    uint32_t friendnum;
    if(tox_get_friend_number(m,id) == -1){
        friendnum = tox_add_friend(m,hex_string_to_bin(id),"HANDSHAKE",strlen("HANDSHAKE"));
        if(friendnum < 0){
            printf("%d\n",110+friendnum);
            return 110+friendnum;
        };
    }
    // check target online state
    uint32_t retryCount = 0;
    while( tox_get_friend_connection_status(m,friendnum) != 1 && retryCount <40){
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
        uint32_t res = tox_send_message(m,friendnum,"HANDSHAKE",strlen("HANDSHAKE"));
        if(res < 0){
            printf("%d\n",210 + res);
            return 210 +res;
        }
        // wait for replay
        uint32_t timeout = 0; // timeout 4s
        while( is_message_received(msg_listener_list,"HANDSHAKE",id) == 0 && timeout< 20){
            usleep(200000);
            timeout ++;
        }
        if(is_message_received(msg_listener_list,"HANDSHAKE",id)){
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
int accept_connect(Tox *m,const uint8_t *id,Msg_listener_list *msg_listener_list){
    // check online state
    if(!tox_isconnected(m)){
        printf("401\n");
        return 401;
    }
    // add friend
    // check if already a friend
    uint32_t friendnum;
    if(!tox_get_friend_number(m,id)){
        // not a friend
        friendnum = tox_add_friend_norequest(m,id);
        if(friendnum < 0){
            printf("%d\n",110+friendnum);
            return 110+friendnum;
        }
    }
    printf("add friend success\n");
    // wait for handshake message
    uint32_t timeout = 0; // timeout 4s
    while(!is_message_received(msg_listener_list,"HANDSHAKE",id) && timeout< 20){
        printf("wait online\n");
        usleep(200000);
        timeout ++;
    }
    if(is_message_received(msg_listener_list,"HANDSHAKE",id)){
        // handShake received
        printf("402\n");
        return 402;
    }
    // handShake failed
    printf("401\n");
    return 401;
}
