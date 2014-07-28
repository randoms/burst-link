#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 2046
#define BACKLOG 10
#define TRUE 1
#define MAXDATASIZE 100


int init_local_sock(int local_port){
    
    int iClientSock;
    struct sockaddr_in ServerAddr;
    
    WSADATA WSAData;


    if(WSAStartup(MAKEWORD(1, 1), &WSAData)){
        printf("initializationing error!\n");
        WSACleanup();
        exit(0);
    }

    if((iClientSock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
        printf("create socket failed!\n");
        WSACleanup();
        exit(0);
    }

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(local_port);
    ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));

    if(connect(iClientSock, (struct sockaddr *) & ServerAddr, sizeof(struct sockaddr)) == -1){
        printf("connect failed");
        WSACleanup();
        exit(0);
    }
    
    return iClientSock;
}

void main(void)
{
    int numbytes;
    char buf[ MAXDATASIZE ];
    int iClientSock = init_local_sock(9992);
    numbytes = recv(iClientSock, buf, MAXDATASIZE, 0);

    if(numbytes == -1){
        printf("receive failed!");
        WSACleanup();
        exit(0);
    }

    buf[ numbytes ] = '\0';

    printf("Received: %s", buf);

    closesocket(iClientSock);
    WSACleanup();
}