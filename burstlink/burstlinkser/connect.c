#include "connect.h"

/**
 * accept a connection
 */
int accept_connect(Tox *m,const uint8_t *client_id_str,Msg_listener_list *msg_listener_list){
    // check online state
    if(!tox_isconnected(m)){
		printf("OFFLINE\n");
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
        return 402;
    }
    // handShake failed
	printf("HANDSHAKE TIMEOUT\n");
    return 401;
}


