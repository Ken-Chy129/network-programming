//usage: select_cli addr file
//read file and send it to server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAXLINE 512
#define SERV_PORT 1602

ssize_t readline(int fd, void* vptr, size_t maxlen);
ssize_t writen(int fd, const void* vptr, int n);

int max(int a, int b) {
    if (a > b) return a;
    else return b;
}

void handle_client(FILE* fp, int sockfd) {
    int maxfdp1, stdineof, n;
    fd_set rset;
    char sendline[MAXLINE], recvline[MAXLINE];

    stdineof = 0;
    FD_ZERO(&rset);

    while (1) {
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset);	//add file descriptor to rset
        FD_SET(sockfd, &rset);		//add socket to rset
        maxfdp1 = max(fileno(fp), sockfd) + 1;

        n = select(maxfdp1, &rset, NULL, NULL, NULL);
        //printf("ready:%d\n",n);

        if (FD_ISSET(sockfd, &rset)) {		//socket ready
            //printf("socket ready  %d\n",stdineof);
            if ((readline(sockfd, recvline, MAXLINE)) == 0) {	//read a line from socket
                //if it's NULL, 
                if (stdineof == 1) return;
                else {
                    printf("handle_client: server	terminated permaturely\n");
                    exit(0);
                }
            }
            fputs(recvline, stdout);
        }
        if (FD_ISSET(fileno(fp), &rset)) {
            printf("file read ready\n");
            //read chars for file
            if ((fgets(sendline, MAXLINE, fp)) == NULL) {
                //end reading file
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            writen(sockfd, sendline, strlen(sendline));
            fputs(sendline, stdout);
        }
    }
}

int main(int argc, char** argv) {
    int client_sockfd;
    FILE* client_file;
    struct sockaddr_in serv_addr;
    //char *file;

    //check parameters
    if (argc != 3) {
        printf("parameter error!\n");
        exit(0);
    }

    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    //serv_addr.sin_addr.s_addr = INADDR_ANY;
    inet_aton(argv[1], &serv_addr.sin_addr);
    if (connect(client_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("connect error\n");
        exit(0);
    }
    
    client_file = fopen(argv[2], "r");
    printf("%d\n", fileno(client_file));
    handle_client(client_file, client_sockfd);
    fclose(client_file);
    close(client_sockfd);
    return 0;
}

ssize_t readline(int fd, void* vptr, size_t maxlen) {
    int n, rc;
    char c, * ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n') break;
        }
        else if (rc == 0) {
            if (n == 1) return 0;
            else break;
        }
        else
            return -1;	//errno, errno set by read();
    }
    *ptr = 0;
    return(n);
}

ssize_t writen(int fd, const void* vptr, int n) {
    int n_left;
    int n_write;
    const char* nptr;

    nptr = vptr;
    n_left = n;
    while (n_left > 0) {
        if ((n_write = write(fd, nptr, n_left)) <= 0) {
            if (errno == EINTR) n_write = 0;
            else return -1;
        }
        n_left -= n_write;
        nptr += n_write;
    }
    return (n);
}
