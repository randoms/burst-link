#include <rpc.h>
#include <stdio.h>
#include <stdint.h>
#pragma comment(lib,"rpcrt4.lib")
int main(void){
    uint8_t *uuid_str = (uint8_t *)malloc(sizeof(uint8_t)*1024);
    UUID uuid;
    UuidCreate(&uuid);
    UuidToString(&uuid,&uuid_str);
    printf("UUID:%s\n",uuid_str);
}
