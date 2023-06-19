#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "08-05dg_client.c"

#define SERV_HOST_ADDR "127.0.0.1"
#define SERV_UDP_PORT 1602

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;

    char *pname = argv[0];

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
    serv_addr.sin_port = htons(SERV_UDP_PORT);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        err_dump("client: can't open datagram socket.");

    bzero(&cli_addr, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cli_addr.sin_port = htons(0);
    if (bind(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0)
        err_dump("client: can't bind local address.");

    dg_client(stdin, sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    close(sockfd);
    exit(0);
}