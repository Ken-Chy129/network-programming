#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "08-05dg_echo.c"

#define SERV_UDP_PORT 1602

void err_dump(const char *str);
void dg_echo(int sockfd, struct sockaddr *pcli_addr, int clilen);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in cli_addr, serv_addr;

    char *pname = argv[0];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        err_dump("server: can't open datagram socket.");

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_UDP_PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        err_dump("server: can't bind local address.");

    dg_echo(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr));

    return 0;
}