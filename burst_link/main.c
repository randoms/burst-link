/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

/**
 * server 主要进行请求的分发，接收到新的请求就把请求分发给route，
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "route.h"

void process_req(int sock);
void error(const char *msg);


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno,pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    //signal(SIGCHLD,SigCatcher);
    signal(SIGCHLD,SIG_IGN);
    if (argc < 2) {
        portno = 9990;
    }else{
        portno = atoi(argv[1]);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0)
            error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while(1){
        newsockfd = accept(sockfd,
              (struct sockaddr *) &cli_addr,
              &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        pid = fork();
        if(pid < 0){
            error("fork error");
        }
        if(pid == 0){
            process_req(newsockfd);
            close(newsockfd);
            exit(0);
        }else{
            close(newsockfd);
        }
    }
    close(sockfd);
    return 0;
}


void process_req(int sock){
    int n;
    char buffer[1024];

    bzero(buffer,1024);
    n = read(sock,buffer,1023);
    if (n < 0) error("ERROR reading from socket");
    printf("Message Received:\n %s",buffer);
    //route(buffer);
    n = write(sock,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
