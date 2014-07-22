#ifndef _UTILES_H
#define _UTILES_H

#define SOCK_BUF_SIZE 256
#define UUID_LENGTH 36
#define CMD_LENGTH 1
#define MESSAGE_LENGTH_BYTE 2
#define MY_MESSAGE_LENGTH UUID_LENGTH + CMD_LENGTH + MESSAGE_LENGTH_BYTE + SOCK_BUF_SIZE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <jansson.h>
#include "../toxcore/tox.h"

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
void buf_to_json_array(json_t *array, const uint8_t *buf, const uint32_t length);
void json_array_to_bin(uint8_t *buf, json_t *array);
void msg_to_bin(uint8_t *bin, const uint8_t *msg);
void bin_to_msg(uint8_t *msg, const uint8_t *bin);
#endif