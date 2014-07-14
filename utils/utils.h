#ifndef _UTILES_H
#define _UTILES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "../toxcore/tox.h"

#define FRADDR_TOSTR_CHUNK_LEN 8
#define FRADDR_TOSTR_BUFSIZE (TOX_FRIEND_ADDRESS_SIZE * 2 + TOX_FRIEND_ADDRESS_SIZE / FRADDR_TOSTR_CHUNK_LEN + 1)
char *hex_string_to_bin(const char *hex_string);
void hexid_to_str(uint8_t *id_bin, uint8_t *id_str);
void fraddr_to_str(uint8_t *id_bin, char *id_str);
void address_str_to_client_str(const uint8_t *add_str, uint8_t *client_id_str);
void address_bin_to_client_id_bin(const uint8_t *address_bin, uint8_t *client_id_bin);
void hex_bin_to_string(const uint8_t *hex_bin,const uint32_t bin_length, uint8_t *str);
void get_my_client_id_str(Tox *m, uint8_t *my_addr_str);
void load_data(Tox *m);
int store_data(Tox *m);
void write_local_message(uint32_t sockfd, const uint8_t *msg);
#endif