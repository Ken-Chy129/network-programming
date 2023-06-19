#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "08-05dg_dump.c"

#define MAXMSG 2048

void dg_echo(int sockfd, struct sockaddr *pcli_addr, int maxclilen)
{
    int n, clilen;
    char mesg[MAXMSG];

    for (;;)
    {
        clilen = maxclilen;
        n = recvfrom(sockfd, mesg, MAXMSG, 0, pcli_addr, &clilen);
        if (n < 0)
            err_dump("dg_echo:recvfrom error");
        if (sendto(sockfd, mesg, n, 0, pcli_addr, clilen) != n)
            err_dump("dg_echo:sendto error");
    }
}