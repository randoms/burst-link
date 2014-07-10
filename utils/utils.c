#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "../toxcore/tox.h"

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


/*
 * Store Messenger to given location
 * Return 0 stored successfully
 * Return 1 file path is NULL
 * Return 2 malloc failed
 * Return 3 opening path failed
 * Return 4 fwrite failed
 */
int store_data(Tox *m)
{
    char cwd[1024];
    char *path = getcwd(cwd, sizeof(cwd));
    strcat(path,"/data.tox");

    if (path == NULL)
        return 1;

    FILE *fd;
    int len;
    char *buf;

    len = tox_size(m);
    buf = malloc(len);

    if (buf == NULL)
        return 2;

    tox_save(m, (uint8_t *) buf);

    fd = fopen(path, "wb");

    if (fd == NULL) {
        free(buf);
        return 3;
    }

    if (fwrite(buf, len, 1, fd) != 1) {
        free(buf);
        fclose(fd);
        return 4;
    }

    free(buf);
    fclose(fd);
    return 0;
}

static void load_data(Tox *m)
{
    char cwd[1024];
    char *path = getcwd(cwd, sizeof(cwd));
    strcat(path,"/data.tox");
    FILE *fd;
    int len;
    char *buf;
    printf("LOADING\n");

    if ((fd = fopen(path, "rb")) != NULL) {
        fseek(fd, 0, SEEK_END);
        len = ftell(fd);
        fseek(fd, 0, SEEK_SET);

        buf = malloc(len);

        if (buf == NULL) {
            fclose(fd);
            printf("LOAD_DATA:ERROR 1");
            exit(EXIT_FAILURE);
        }

        if (fread(buf, len, 1, fd) != 1) {
            free(buf);
            fclose(fd);
            printf("LOAD_DATA:ERROR 2");
            exit(EXIT_FAILURE);
        }

        tox_load(m, (uint8_t *) buf, len);
        free(buf);
        fclose(fd);
    } else {
        int st;

        if ((st = store_data(m)) != 0)
            printf("LOAD_DATA:ERROR 3");
            exit(EXIT_FAILURE);;
    }
}