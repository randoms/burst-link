/* nTox.c
 *
 * Textual frontend for Tox.
 *
 *  Copyright (C) 2013 Tox project All Rights Reserved.
 *
 *  This file is part of Tox.
 *
 *  Tox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tox.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
#include "utils/utils.c"

#define FRADDR_TOSTR_CHUNK_LEN 8
#define FRADDR_TOSTR_BUFSIZE (TOX_FRIEND_ADDRESS_SIZE * 2 + TOX_FRIEND_ADDRESS_SIZE / FRADDR_TOSTR_CHUNK_LEN + 1)

static void fraddr_to_str(uint8_t *id_bin, char *id_str)
{
    uint32_t i, delta = 0, pos_extra, sum_extra = 0;

    for (i = 0; i < TOX_FRIEND_ADDRESS_SIZE; i++) {
        sprintf(&id_str[2 * i + delta], "%02hhX", id_bin[i]);

        if ((i + 1) == TOX_CLIENT_ID_SIZE)
            pos_extra = 2 * (i + 1) + delta;

        if (i >= TOX_CLIENT_ID_SIZE)
            sum_extra |= id_bin[i];

        if (!((i + 1) % FRADDR_TOSTR_CHUNK_LEN)) {
            id_str[2 * (i + 1) + delta] = ' ';
            delta++;
        }
    }

    id_str[2 * i + delta] = 0;

    if (!sum_extra)
        id_str[pos_extra] = 0;
}

void get_id(Tox *m, char *data)
{
    sprintf(data, "[i] ID: ");
    int offset = strlen(data);
    uint8_t address[TOX_FRIEND_ADDRESS_SIZE];
    tox_get_address(m, address);
    fraddr_to_str(address, data + offset);
}

int main(int argc, char *argv[])
{

    
    char idstring[200] = {0};
    Tox *m;

    m = tox_new(0);

    if ( !m ) {
        fputs("Failed to allocate Messenger datastructure", stderr);
        exit(0);
    }

//     tox_callback_friend_request(m, print_request, NULL);
//     tox_callback_friend_message(m, print_message, NULL);
//     tox_callback_name_change(m, print_nickchange, NULL);
//     tox_callback_status_message(m, print_statuschange, NULL);
//     tox_callback_group_invite(m, print_invite, NULL);
//     tox_callback_group_message(m, print_groupmessage, NULL);
//     tox_callback_file_data(m, write_file, NULL);
//     tox_callback_file_control(m, file_print_control, NULL);
//     tox_callback_file_send_request(m, file_request_accept, NULL);
//     tox_callback_group_namelist_change(m, print_groupnamelistchange, NULL);

    get_id(m, idstring);
    printf("%s",idstring);
    printf("\n");

    uint16_t port = 33445;
    unsigned char *binary_string = hex_string_to_bin("7F613A23C9EA5AC200264EB727429F39931A86C39B67FC14D9ECA4EBE0D37F25");
    int res = tox_bootstrap_from_address(m, "42.96.195.88", 0, port, binary_string);
    free(binary_string);

    if (!res) {
        printf("Failed to convert into an IP address. Exiting...\n");
        exit(1);
    }

    time_t timestamp0 = time(NULL);
    int on = 0;
    while (1) {
        if (on == 0) {
            if (tox_isconnected(m)) {
                printf("[i] connected to DHT\n");
                on = 1;
            } else {
                time_t timestamp1 = time(NULL);

                if (timestamp0 + 10 < timestamp1) {
                    timestamp0 = timestamp1;
                    tox_bootstrap_from_address(m, "42.96.195.88", 0, port, binary_string);
                }
            }
        }else{
            return 0;
        }
    }
}
