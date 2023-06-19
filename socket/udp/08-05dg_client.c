#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "08-05dg_dump.c"

#define MAXLINE 512

void dg_client(FILE *fp, int sockfd, struct sockaddr *pserv_addr, int servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    while (fgets(sendline, MAXLINE, fp) != NULL)
    {
        n = strlen(sendline);
        if (sendto(sockfd, sendline, n, 0, pserv_addr, servlen) != n)
            err_dump("dg_client: sendto error on socket");
        
        /* now read a message from the socket and write it 
         * to our standard output. */
        n = recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *)0, (int *)0);
        if (n < 0)
            err_dump("dg_client: recvfrom error");

        recvline[n] = 0; /* null terminate */
        fputs(recvline, stdout);
    }

    if (ferror(fp))
        err_dump("dg_client: error reading file");
}