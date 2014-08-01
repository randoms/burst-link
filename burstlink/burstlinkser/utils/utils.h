#ifndef _UTILES_H
#define _UTILES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "../jansson.h"
#include <time.h>
#ifdef _WIN32
#include <winsock.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#endif
#include "../toxcore/tox.h"
#include "../defines.h"

#define TRUE 1
#define FALSE 0
#define FRADDR_TOSTR_CHUNK_LEN 8
#define FRADDR_TOSTR_BUFSIZE (TOX_FRIEND_ADDRESS_SIZE * 2 + TOX_FRIEND_ADDRESS_SIZE / FRADDR_TOSTR_CHUNK_LEN + 1)

char *hex_string_to_bin(uint8_t *bin, const char *hex_string);
void hexid_to_str(uint8_t *id_bin, uint8_t *id_str);
void fraddr_to_str(uint8_t *id_bin, char *id_str);
void address_str_to_client_str(const uint8_t *add_str, uint8_t *client_id_str);
void address_bin_to_client_id_bin(const uint8_t *address_bin, uint8_t *client_id_bin);
void hex_bin_to_string(const uint8_t *hex_bin,const uint32_t bin_length, uint8_t *str);
void get_my_client_id_str(Tox *m, uint8_t *my_id_str);
void load_data(Tox *m);
int store_data(Tox *m);
void write_local_message(uint32_t sockfd, const uint8_t *msg);
void printf_local_message(uint32_t sockfd, const uint8_t* format, uint32_t data);
void bufcopy(uint8_t *target,const uint8_t *origin,uint32_t length);
// void buf_to_json_array(json_t *array, const uint8_t *buf, const uint32_t length);
// void json_array_to_bin(uint8_t *buf, json_t *array);
// void msg_to_bin(uint8_t *bin, const uint8_t *msg);
// void bin_to_msg(uint8_t *msg, const uint8_t *bin);
void pack_msg_bin(uint8_t *msg_bin, const uint8_t *uuid, const uint8_t *cmd, const uint8_t *data, const uint32_t length);
void unpack_msg_bin(const uint8_t *msg_bin, uint8_t *uuid, uint8_t *cmd, uint8_t *data, uint32_t *length);
void debugTargetBin(const uint8_t *bin);
void randomsWait(uint32_t time);
#endif