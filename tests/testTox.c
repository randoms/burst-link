#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#include "toxcore/tox.h"

#include "route.h"
#include "utils/utils.h"

#define BOOTSTRAP_ADDRESS "42.96.195.88"
#define BOOTSTRAP_PORT 33445
#define BOOTSTRAP_KEY "7F613A23C9EA5AC200264EB727429F39931A86C39B67FC14D9ECA4EBE0D37F25"
#define FRIEND_ID "341CCFBCC4D41C5B3AB89E31E7561C5D37E201D5DDBFA7AFC6B4EDD2D6A82F4B7D06A2ED3DE4"

#define FRADDR_TOSTR_CHUNK_LEN 8
#define FRADDR_TOSTR_BUFSIZE (TOX_FRIEND_ADDRESS_SIZE * 2 + TOX_FRIEND_ADDRESS_SIZE / FRADDR_TOSTR_CHUNK_LEN + 1)

void get_id(Tox *m, char *data)
{
    sprintf(data, "[i] ID: ");
    int offset = strlen(data);
    uint8_t address[TOX_FRIEND_ADDRESS_SIZE];
    tox_get_address(m, address);
    fraddr_to_str(address, data + offset);
}


int main(){
    uint8_t ipv6enabled = 0;
    int on = 0;
//     char idstring[200] = {0};
    Tox *m;
    m = tox_new(ipv6enabled);
    if ( !m ) {
        fputs("Failed to allocate Messenger datastructure", stderr);
        exit(0);
    }
//     get_id(m, idstring);
    unsigned char *binary_string = hex_string_to_bin("7F613A23C9EA5AC200264EB727429F39931A86C39B67FC14D9ECA4EBE0D37F25");
    int res = tox_bootstrap_from_address(m, "42.96.195.88", 0, htons(33445), binary_string);
    free(binary_string);

    if (!res) {
        //printf("Failed to convert \"%s\" into an IP address. Exiting...\n", argv[argvoffset + 1]);
        printf("error\n");
//         endwin();
        exit(1);
    }
    uint8_t name[TOX_MAX_NAME_LENGTH + 1];
    uint16_t namelen = tox_get_self_name(m, name);
    name[namelen] = 0;
    if (namelen > 0) {
        char whoami[128 + TOX_MAX_NAME_LENGTH];
        snprintf(whoami, sizeof(whoami), "[i] your current username is: %s", name);
    }
    time_t timestamp0 = time(NULL);
    while (1) {
        if (on == 0) {
            if (tox_isconnected(m)) {
                //printf("hello\n");
                on = 1;
            } else {
//                 time_t timestamp1 = time(NULL);
//                 if (timestamp0 + 10 < timestamp1) {
//                     timestamp0 = timestamp1;
//                     //tox_bootstrap_from_address(m, argv[argvoffset + 1], ipv6enabled, port, binary_string);
//                     tox_bootstrap_from_address(m, "42.96.195.88", 0, htons(33445), binary_string);
//                 }
            }
        }
        time_t timestamp1 = time(NULL);
        uint8_t send_flag = 0;
        if (timestamp0 + 2 < timestamp1) {
            timestamp0 = timestamp1;
            printf("sending message\n");
            uint32_t friend_num = tox_add_friend(m,hex_string_to_bin(FRIEND_ID),"HELLO",strlen("HELLO"));
            if(friend_num == -4){
                friend_num = tox_get_friend_number(m,hex_string_to_bin(FRIEND_ID));
                printf("friend num:%d\n",friend_num);
            }
            printf("adding\n");
            printf("add res:%d\n",friend_num);
            uint32_t online_friend_num = tox_get_num_online_friends(m);
            printf("online friend num:%d\n",online_friend_num);
            uint32_t message_id;
            if(send_flag == 0)message_id = tox_send_message(m,friend_num,"hi",strlen("hi"));
            if(message_id >0)send_flag =1;
        }
        tox_do(m);
    }

    tox_kill(m);
    return 0;
}