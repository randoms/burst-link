#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <string.h>


#pragma comment(lib, "ws2_32.lib")

#define TRUE 1


int init_local_sock(int local_port){
    
}

int init_local_sock_serv(int local_port){
    int iServerSock;
    struct sockaddr_in ServerAddr;
    WSADATA WSAData;
    
    if(WSAStartup(MAKEWORD(1, 1), &WSAData)){
        printf("initializationing error!\n");
        WSACleanup();
        exit(0);
    }

    if((iServerSock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
        printf("create socket failed\n");
        WSACleanup();
        exit(0);
    }

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(local_port);//监视的端口号
    ServerAddr.sin_addr.s_addr = INADDR_ANY;//本地IP
    memset(& (ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));
    
    if(bind(iServerSock, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) == -1){
        printf("bind failed!\n");
        WSACleanup();
        exit(0);
    }

    if(listen(iServerSock, 5) == -1){
        printf("listen failed!\n");
        WSACleanup();
        exit(0);
    }
    return iServerSock;
}

void main(void){
    
    int iServerSock = init_local_sock_serv(9992);
    printf("%d\n",iServerSock);
    int iClientSock;
    int sin_size;
    struct sockaddr_in ClientAddr;
    
    char *buf = "hello, world!\n";

    while(TRUE)
    {
        sin_size = sizeof(struct sockaddr_in);
        iClientSock = accept(iServerSock, (struct sockaddr *)&ClientAddr, &sin_size);

        if(iClientSock == -1)
        {
        printf("accept调用失败!\n");
        WSACleanup();
        exit(0);
        }

        printf("服务器连接到%s\n", inet_ntoa(ClientAddr.sin_addr));
        if(send(iClientSock, buf, strlen(buf), 0) == -1)
        {
            printf("send调用失败!");
            closesocket(iClientSock);
            WSACleanup();
            exit(0);
        }
    }
}
