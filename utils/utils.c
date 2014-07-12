#include "utils.h"

char *hex_string_to_bin(const char *hex_string)
{
    size_t len = strlen(hex_string);
    char *val = malloc(len);

    if (val == NULL){
        printf("transform failed");
    }

    size_t i;

    for (i = 0; i < len; ++i, hex_string += 2)
        sscanf(hex_string, "%2hhx", &val[i]);

    return val;
}

void fraddr_to_str(uint8_t *id_bin, char *id_str)
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

void hexid_to_str(uint8_t *id_bin, uint8_t *id_str){
    uint8_t length = TOX_FRIEND_ADDRESS_SIZE;
    uint8_t i = 0;
    uint8_t *temp_str;
    printf("called\n");
    for(i = 0; i<length;i++){
        printf(temp_str,"%X",id_bin[i]);
        //sprintf(temp_str,"%X",id_bin[i]);
        //id_str[i] = temp_str[0];
        //id_str[i+1] = temp_str[1];
    }
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

void load_data(Tox *m)
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