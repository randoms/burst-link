#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>

char *hex_string_to_bin(const char *hex_string)
{
    size_t len = strlen(hex_string);
    char *val = malloc(len);

    if (val == NULL)
       printf("transform failed");

    size_t i;

    for (i = 0; i < len; ++i, hex_string += 2)
        sscanf(hex_string, "%2hhx", &val[i]);

    return val;
}

static uint64_t current_unix_time;

void update_unix_time(void)
{
    current_unix_time = (uint64_t) time(NULL);
}

uint64_t get_unix_time(void)
{
    return current_unix_time;
}

int timed_out(uint64_t timestamp, uint64_t curtime, uint64_t timeout)
{
    return timestamp + timeout <= curtime;
}