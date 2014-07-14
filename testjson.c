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
#include <pthread.h>
#include <jansson.h>
#include "toxcore/tox.h"

uint8_t *testdata;

void hex_bin_to_string(uint8_t *hex_bin, uint8_t *str){
    testdata = (uint8_t *)malloc(sizeof(uint8_t)*10);
    strcpy(testdata,"Hello world");
    uint32_t bin_length = strlen(hex_bin);
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
    printf("%s\n",str);
}

int main(int argc, char *argvs[]){
    uint8_t test[100];
    sprintf(test,"HI:%d\n",10000);
    printf("%s\n",test);
}

