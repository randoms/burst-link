#include "utils.h"

char *hex_string_to_bin(uint8_t *bin, const char *hex_string)
{
    size_t len = strlen(hex_string)/2;

    if (bin == NULL){
//         printf("transform failed");
    }

    size_t i;
	for (i = 0; i < len; i++, hex_string += 2){
		int num;
#ifdef _WIN32
		sscanf_s(hex_string, "%2hhx", &num);
        bin[i] = (uint8_t)num;
#else
		sscanf(hex_string, "%2hhx", &bin[i]);
#endif
		
	}
    return bin;
}

void address_bin_to_client_id_bin(const uint8_t *address_bin, uint8_t *client_id_bin){
    uint32_t i;
    for(i=0;i<TOX_CLIENT_ID_SIZE;i++){
        client_id_bin[i] = address_bin[i];
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
#ifdef _WIN32
	char *path = _getcwd(cwd, sizeof(cwd));
	strcat_s(path,1024,"/data.tox");
#else
	char *path = getcwd(cwd, sizeof(cwd));
	strcat(path, "/data.tox");
#endif
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
#ifdef _WIN32
	fopen_s(&fd, path, "wb");
#else
	fd = fopen(path, "wb");
#endif
    

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
#ifdef _WIN32
	char *path = _getcwd(cwd, sizeof(cwd));
	strcat_s(path,1024,"/data.tox");
#else
	char *path = getcwd(cwd, sizeof(cwd));
	strcat(path, "/data.tox");
#endif
    
    FILE *fd;
    int len;
    char *buf;
//     printf("LOADING\n");
#ifdef _WIN32
	fopen_s(&fd, path, "rb");
	if (fd != NULL) {
#else
	if ((fd = fopen(path, "rb")) != NULL) {
#endif
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

void hex_bin_to_string(const uint8_t *hex_bin, const uint32_t bin_length, uint8_t *str){

	uint32_t str_length = 2 * bin_length + 1;
	uint8_t temp_str[10] = { 0 };
	uint32_t i = 0;
#ifdef _WIN32
	strcpy_s(str, str_length, "");
#else
	strcpy(str, "");
#endif
	for (i = 0; i<bin_length; i++){
#ifdef _WIN32
		memset(temp_str, 0, 10);
		if (hex_bin[i]<16){
			sprintf_s(temp_str, 10, "0%X", hex_bin[i]);
		}
		else{
			sprintf_s(temp_str, 10, "%X", hex_bin[i]);
		}
		strcat_s(str, str_length, temp_str);
#else
		if (hex_bin[i]<16){
			sprintf(temp_str, "0%X", hex_bin[i]);
		}
		else{
			sprintf(temp_str, "%X", hex_bin[i]);
		}
		strcat(str, temp_str);
#endif
	}
}

void get_my_client_id_str(Tox *m, uint8_t *my_id_str){
    uint8_t addr_bin[TOX_FRIEND_ADDRESS_SIZE+1];
    uint8_t my_addr_str[TOX_FRIEND_ADDRESS_SIZE*2+1];
    
    tox_get_address(m,addr_bin);
    hex_bin_to_string(addr_bin,TOX_FRIEND_ADDRESS_SIZE,my_addr_str);
    address_str_to_client_str(my_addr_str,my_id_str);
}


void printf_local_message(uint32_t sockfd, const uint8_t* format, uint32_t data){
    uint8_t temp_str[1024];
#ifdef _WIN32
	sprintf_s(temp_str,1024,format,data);
#else
	sprintf(temp_str, format, data);
#endif
}

void bufcopy(uint8_t *target, const uint8_t *origin,uint32_t length){
    uint32_t i;
    for(i=0;i<length;i++){
        target[i] = origin[i];
    }
}

// void buf_to_json_array(json_t *array, const uint8_t *buf, const uint32_t length){
//     uint32_t i = 0;
//     for(i=0;i<length;i++){
//         json_t *data = json_pack("i",buf[i]);
//         json_array_append(array,data);
//         json_decref(data);
//     }
// }
// 
// void json_array_to_bin(uint8_t *buf, json_t *array){
//     uint32_t length = (uint32_t)json_array_size(array);
//     int i = 0;
//     for(i=0;i<length;i++){
//         buf[i] = (uint8_t)json_integer_value(json_array_get(array,i));
//     }
// }

// void msg_to_bin(uint8_t *bin,const uint8_t *msg){
//     json_error_t err;
//     json_t *msg_json = json_loads(msg,0,&err);
//     json_t *data_json = json_object_get(msg_json,"data");
//     json_t *length_json = json_object_get(msg_json,"length");
//     json_t *cmd_json = json_object_get(msg_json,"cmd");
//     json_t *uuid = json_object_get(msg_json,"uuid");
//     
//     const uint8_t *uuid_str = json_string_value(uuid);
//     bzero(bin,MY_MESSAGE_LENGTH);
//     // add uuid to buf
//     uint32_t i = 0;
//     for(i=0;i<UUID_LENGTH;i++){
//         bin[i] = uuid_str[i];
//     }
//     // add cmd
//     if(cmd_json != NULL){
//         const uint8_t *cmd_str = json_string_value(cmd_json);
//         if(strcmp(cmd_str,"CLOSE_SOCK") == 0)
//             bin[UUID_LENGTH] = 2;
//         if(strcmp(cmd_str,"CREATE_SOCK") == 0)
//             bin[UUID_LENGTH] = 1;
//         // add cmd content to bin
//         const uint8_t *data_str = json_string_value(data_json);
//         for(i=0;i<strlen(data_str);i++){
//             bin[UUID_LENGTH + CMD_LENGTH + MESSAGE_LENGTH_BYTE + i] = data_str[i];
//         }
//         bin[UUID_LENGTH + CMD_LENGTH + MESSAGE_LENGTH_BYTE + strlen(data_str)] = '\0';
//     }else{
//         bin[UUID_LENGTH] = 0;
//     }
//     
//     // add message length
//     const uint32_t length = json_integer_value(length_json);
//     uint8_t hight_byte = length/256;
//     uint8_t low_byte = length%256;
//     bin[UUID_LENGTH+1] = hight_byte;
//     bin[UUID_LENGTH+2] = low_byte;
//     
//     // add message content
//     if(cmd_json == NULL){
//         for(i=0;i<length;i++){
//             bin[UUID_LENGTH+CMD_LENGTH+MESSAGE_LENGTH_BYTE+i] = (uint8_t)json_integer_value(json_array_get(data_json,i));
//         }
//     }
// }

// void bin_to_msg(uint8_t *msg, const uint8_t *bin){
//     // get uuid
//     uint8_t uuid_str[UUID_LENGTH];
//     uint32_t i =0 ;
//     for(i=0;i<UUID_LENGTH;i++){
//         uuid_str[i] = bin[i];
//     }
//     uuid_str[UUID_LENGTH] = '\0';
//     
//     // add cmd
//     uint8_t *cmd_str;
//     json_t *data_json = NULL;
//     switch(bin[UUID_LENGTH]){
//         case 0:{
//             cmd_str = NULL;
//             break;
//         }
//             
//         case 1:{
//             cmd_str = "CREATE_SOCK";
//             break;
//         }
//             
//         case 2:{
//             cmd_str = "CLOSE_SOCK";
//             break;
//         }
//     }
//     if(bin[UUID_LENGTH] != 0){
//         uint8_t data_str[MY_MESSAGE_LENGTH-UUID_LENGTH-CMD_LENGTH-MESSAGE_LENGTH_BYTE];
//         bzero(data_str,MY_MESSAGE_LENGTH-UUID_LENGTH-CMD_LENGTH-MESSAGE_LENGTH_BYTE);
//         
//         for(i=0;i<MY_MESSAGE_LENGTH-UUID_LENGTH-CMD_LENGTH-MESSAGE_LENGTH_BYTE;i++){
//             data_str[i] = bin[UUID_LENGTH+CMD_LENGTH+MESSAGE_LENGTH_BYTE+i];
//         }
//         data_str[MY_MESSAGE_LENGTH-UUID_LENGTH-CMD_LENGTH-MESSAGE_LENGTH_BYTE] = '\0';
//         data_json = json_pack("s",data_str);
//     }
//     
//     // add length
//     uint8_t hight_byte = bin[UUID_LENGTH+CMD_LENGTH];
//     uint8_t low_byte = bin[UUID_LENGTH+CMD_LENGTH + 1];
//     uint32_t length = hight_byte*256 +low_byte;
//     
//     // add message content
//     if(data_json == NULL){
//         data_json = json_array();
//         for(i=0;i<length;i++){
//             json_array_append(data_json,json_pack("i",bin[i + UUID_LENGTH + CMD_LENGTH + MESSAGE_LENGTH_BYTE]));
//         }
//     }
//     json_t *msg_json = json_object();
//     json_object_set(msg_json,"uuid",json_pack("s",uuid_str));
//     json_object_set(msg_json,"data",data_json);
//     json_object_set(msg_json,"cmd",json_pack("s",cmd_str));
//     json_object_set(msg_json,"length",json_pack("i",length));
//     strcpy(msg,json_dumps(msg_json,JSON_INDENT(4)));
// }

void pack_msg_bin(uint8_t *msg_bin, const uint8_t *uuid, const uint8_t *cmd, const uint8_t *data, const uint32_t length){
    uint32_t i=0;
    memset(msg_bin,0,MY_MESSAGE_LENGTH);
    // set uuid bytes
    for(i=0;i<UUID_LENGTH;i++){
        msg_bin[i] = uuid[i];
    }
    
    // set cmd data mark byte
    if(cmd == NULL){
        msg_bin[UUID_LENGTH] = 0;
    }else if(strcmp(cmd,"CREATE_SOCK") == 0){
        msg_bin[UUID_LENGTH] = 1;
    }else if(strcmp(cmd, "CLOSE_SOCK") == 0){
        msg_bin[UUID_LENGTH] = 2;
    }else{
        printf("INVAILD COMMAND\n");
        exit(1);
    }
    
    // set data length bytes
    uint8_t hight_byte = length/256;
    uint8_t low_byte = length%256;
    msg_bin[UUID_LENGTH+1] = hight_byte;
    msg_bin[UUID_LENGTH+2] = low_byte;
    
    // set data bytes
    for(i=0;i<length;i++){
        msg_bin[UUID_LENGTH+CMD_LENGTH+MESSAGE_LENGTH_BYTE+i] = data[i];
    }
    
}

void unpack_msg_bin(const uint8_t *msg_bin, uint8_t *uuid, uint8_t *cmd, uint8_t *data, uint32_t *length){
    uint32_t i=0;
    
    // get uuid bytes
    memset(uuid,0,UUID_LENGTH);
    memset(cmd,0,32);
    memset(data,0,SOCK_BUF_SIZE);
    for(i=0;i<UUID_LENGTH;i++){
        uuid[i] = msg_bin[i];
    }
    uuid[UUID_LENGTH] = '\0';
    
    // get data length
    uint8_t hight_byte = msg_bin[UUID_LENGTH+CMD_LENGTH];
    uint8_t low_byte = msg_bin[UUID_LENGTH+CMD_LENGTH + 1];
    *length = hight_byte*256 +low_byte;
    
    //get cmd data byte
    if(msg_bin[UUID_LENGTH] == 0){
        // raw data type
#ifdef _WIN32
		strcpy_s(cmd,1024,"RAW_DATA");
#else
		strcpy(cmd, "RAW_DATA");
#endif
        
        
        // unpack data
        for(i=0;i<*length;i++){
            data[i] = msg_bin[UUID_LENGTH+CMD_LENGTH+MESSAGE_LENGTH_BYTE+i];
        }
    }else{
        // cmd received
#ifdef _WIN32
		if(msg_bin[UUID_LENGTH] == 1){
			strcpy_s(cmd,1024, "CREATE_SOCK");
		}
		else if (msg_bin[UUID_LENGTH] == 2){
			strcpy_s(cmd, 1024, "CLOSE_SOCK");
		}
		else{
			strcpy_s(cmd, 1024, "UNKNOWN_CMD");
		}
#else
		if (msg_bin[UUID_LENGTH] == 1){
			strcpy(cmd, "CREATE_SOCK");
		}
		else if (msg_bin[UUID_LENGTH] == 2){
			strcpy(cmd, "CLOSE_SOCK");
		}
		else{
			strcpy(cmd, "UNKNOWN_CMD");
		}
#endif
        for(i=0;i<*length;i++){
            data[i] = msg_bin[UUID_LENGTH+CMD_LENGTH+MESSAGE_LENGTH_BYTE+i];
        }
        data[*length] = '\0';
    }
}


void debugTargetBin(const uint8_t *bin){
    uint8_t i = 0;
    uint8_t addr_str[] = "6CE307E29A0D7F869777F175525BF91AFF638A64F629258EE6747F9D927F3263B95FCCD8FABA";
    uint8_t addr_bin[TOX_FRIEND_ADDRESS_SIZE+1];
    hex_string_to_bin(addr_bin,addr_str);
    for(i=0;i<TOX_CLIENT_ID_SIZE;i++){
        if(bin[i] != addr_bin[i]){
            printf("addr ERROR\n");
            exit(0);
        }
    }
}