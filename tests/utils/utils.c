#include "utils.h"

char *hex_string_to_bin(const char *hex_string)
{
    size_t len = strlen(hex_string);
    char *val = malloc(len);

    if (val == NULL){
//         printf("transform failed");
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

void address_bin_to_client_id_bin(const uint8_t *address_bin, uint8_t *client_id_bin){
    uint32_t i;
    for(i=0;i<TOX_CLIENT_ID_SIZE;i++){
        client_id_bin[i] = address_bin[i];
    }
    client_id_bin[TOX_CLIENT_ID_SIZE] = '\0';
}

void hexid_to_str(uint8_t *id_bin, uint8_t *id_str){
    uint8_t length = TOX_FRIEND_ADDRESS_SIZE;
    uint8_t i = 0;
    uint8_t *temp_str;
    for(i = 0; i<length;i++){
//         printf(temp_str,"%X",id_bin[i]);
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
//     printf("LOADING\n");

    if ((fd = fopen(path, "rb")) != NULL) {
        fseek(fd, 0, SEEK_END);
        len = ftell(fd);
        fseek(fd, 0, SEEK_SET);

        buf = malloc(len);

        if (buf == NULL) {
            fclose(fd);
//             printf("LOAD_DATA:ERROR 1");
            exit(EXIT_FAILURE);
        }

        if (fread(buf, len, 1, fd) != 1) {
            free(buf);
            fclose(fd);
//             printf("LOAD_DATA:ERROR 2");
            exit(EXIT_FAILURE);
        }

        tox_load(m, (uint8_t *) buf, len);
        free(buf);
        fclose(fd);
    } else {
        int st;

        if ((st = store_data(m)) != 0)
//             printf("LOAD_DATA:ERROR 3");
            exit(EXIT_FAILURE);;
    }
}


void address_str_to_client_str(const uint8_t *add_str, uint8_t *client_id_str){
    uint32_t max_length = TOX_CLIENT_ID_SIZE*2;
    uint32_t i = 0;
    for(i = 0;i<max_length;i++){
        client_id_str[i] = add_str[i];
    }
    client_id_str[max_length] = '\0';
}

void hex_bin_to_string(const uint8_t *hex_bin,const uint32_t bin_length, uint8_t *str){
    
    uint32_t str_length = 2*bin_length;
    uint8_t temp_str[10];
    uint32_t i =0;
    strcpy(str,"");
    for(i=0;i<bin_length;i++){
        if(hex_bin[i]<16){
            sprintf(temp_str,"0%X",hex_bin[i]);
        }else{
            sprintf(temp_str,"%X",hex_bin[i]);
        }
        strcat(str,temp_str);
    }
}

void get_my_client_id_str(Tox *m, uint8_t *my_id_str){
    uint8_t addr_bin[TOX_FRIEND_ADDRESS_SIZE];
    uint8_t my_addr_str[TOX_FRIEND_ADDRESS_SIZE*2];
    
    tox_get_address(m,addr_bin);
    hex_bin_to_string(addr_bin,TOX_FRIEND_ADDRESS_SIZE,my_addr_str);
    address_str_to_client_str(my_addr_str,my_id_str);
}

void write_local_message(uint32_t sockfd,const uint8_t *msg){
    uint8_t local_msg[2048];
    local_msg[0] = '\0';
    strcat(local_msg,"LOCAL:");
    strcat(local_msg,msg);
    strcat(local_msg,"\n");
    write(sockfd,local_msg,strlen(local_msg));
}

void printf_local_message(uint32_t sockfd, const uint8_t* format, uint32_t data){
    uint8_t temp_str[1024];
    sprintf(temp_str,format,data);
    write_local_message(sockfd,temp_str);
}

void bufcopy(uint8_t *target, const uint8_t *origin,uint32_t length){
    uint32_t i;
    for(i=0;i<length;i++){
        target[i] = origin[i];
    }
}