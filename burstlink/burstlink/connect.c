#include "connect.h"


/**
 * init a tox connection
 */
int init_connect(Tox *m, const uint8_t *address_str, Msg_listener_list **msg_listener_list){
    // check online state
    if(!tox_isconnected(m)){
        printf("401\n");
        return 401;
    }
	
	uint8_t client_id_str[TOX_CLIENT_ID_SIZE*2 + 1];
    address_str_to_client_str(address_str,client_id_str);
    uint8_t client_addr_bin[TOX_FRIEND_ADDRESS_SIZE];
    hex_string_to_bin(client_addr_bin, address_str);

    // add friend
    // check if already a friend
    uint32_t friendnum;
    if(tox_get_friend_number(m,client_addr_bin) == -1){
        friendnum = tox_add_friend(m,client_addr_bin,"HANDSHAKE",strlen("HANDSHAKE"));
        if(friendnum < 0){
            printf("%d\n",110+friendnum);
            return 110+friendnum;
        };
    }
    friendnum = tox_get_friend_number(m,client_addr_bin);
    //printf("friendnum:%d\n",friendnum);
    // check target online state
    uint32_t retryCount = 0;
    while( tox_get_friend_connection_status(m,friendnum) != 1 && retryCount <100){
#ifdef _WIN32
		Sleep(1000);
#else
		usleep(1000000);
#endif
        retryCount ++;
    }
    
    
    if(tox_get_friend_connection_status(m,friendnum)){
        printf("online\n");
        // add listener
        uint32_t res = tox_send_message(m,friendnum,"HANDSHAKE",strlen("HANDSHAKE"));
        if(res < 0){
            printf("%d\n",210 + res);
            return 210 +res;
        }
        // wait for replay
        uint32_t timeout = 0; // timeout 4s
        while( is_message_received(msg_listener_list,"HANDSHAKE",client_id_str) == 0 && timeout< 20){
#ifdef _WIN32
			Sleep(200);
#else
			usleep(200000);
#endif
            
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
int accept_connect(Tox *m,const uint8_t *client_id_str,Msg_listener_list *msg_listener_list){
    // check online state
    if(!tox_isconnected(m)){
        printf("401\n");
        return 401;
    }
    // add friend
    // check if already a friend
    uint32_t friendnum = 0;
    uint8_t addr_bin[TOX_FRIEND_ADDRESS_SIZE];
    hex_string_to_bin(addr_bin,client_id_str);
    if(tox_get_friend_number(m,addr_bin) == -1){
        // not a friend
        friendnum = tox_add_friend_norequest(m,addr_bin);
        if(friendnum < 0){
            printf("%d\n",110+friendnum);
            return 110+friendnum;
        }
    }
    friendnum = tox_get_friend_number(m,addr_bin);
    printf("ADD_FRIEND:%d\n",friendnum);
    // wait for handshake message
    uint32_t timeout = 0; // timeout 4s
    printf("ID:%s\n",client_id_str);
    printf("WAITING HANDSHAKE\n");
    
    while(is_message_received(&msg_listener_list,"HANDSHAKE",client_id_str) == 0  && timeout< 100){
#ifdef _WIN32
		Sleep(200);
#else
		usleep(200000);
#endif
        
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


