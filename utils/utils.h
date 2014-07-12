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
void load_data(Tox *m);
int store_data(Tox *m);
#endif