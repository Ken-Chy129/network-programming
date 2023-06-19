#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAXLINE 512
#define SERV_PORT 1602
#define LISTENQ 64

//static ssize_t my_read(int fd, char *ptr);
ssize_t readline(int fd, void* vptr, size_t maxlen);
ssize_t writen(int fd, const void* vptr, int n);

int main(int argc, char** argv) {
    int i, maxi, maxfd;
    int listen_fd, connfd, sockfd;
    int num_fd, client[FD_SETSIZE], files;
    /*
      FD_SETSIZE是一个宏定义的常量整数，在sys/select.h中定义。
      它表示fd_set类型的最大值，通常为1024。
      数组client用于记录在allset中设置的fd：
      如果client[i]>0，表示client[i]是一个需要检查的fd。client数组的前面项大于0，后面的项小于0。
    */

    ssize_t n;
    /*
      fd_set 是一个数据类型，用于表示一个文件描述符集合。它通常以位图的形式实现，每个位表示一个文件描述符（通常是一个整数）。
      fd_set 是一个长整型数组，通过将对应文件描述符的位设置为1，可以将其添加到集合中。
      在使用 select() 系统调用时，需要通过操作 fd_set 来设置要监听的文件描述符集合。
      select() 将检查 fd_set 中被设置为1的位，以确定哪些文件描述符准备好进行读取、写入或发生异常事件。
    */
    fd_set rset, allset;
    char line[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in client_addr, serv_addr;

    files = open("./tmp.log", O_RDWR);
    if (files > 0) {
        printf("The file tmp.log already exit, please move it.\nThen start this program again\n");
        exit(0);
    }
    files = open("./tmp.log", O_WRONLY | O_CREAT | O_TRUNC);

    // AF_INET 指定了使用 IPv4 地址族，SOCK_STREAM 指定了使用 TCP 协议，0 是传递给 socket 函数的协议参数，表示使用默认协议。
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    /*
      为了确保在使用 serv_addr 变量之前，所有的字段都被初始化为0。
      在网络编程中，常常需要使用结构体来表示网络地址信息，而清零地址结构体是一种常见的初始化方式，以避免出现未初始化的值对程序造成不确定的行为。
    */
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // INADDR_ANY 的值是 0.0.0.0，它表示接受来自任何网络接口的连接请求。通过将套接字绑定到 INADDR_ANY，服务器将能够接受来自本地所有可用 IP 地址的连接。
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);

    bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listen_fd, LISTENQ);
    maxfd = listen_fd;
    maxi = -1;

    /*
      通常在使用 select() 函数进行 I/O 多路复用时，会使用一个数组来记录所有要监视的套接字文件描述符的状态。
      初始化时，将所有的元素设置为 -1 是为了表示这些套接字文件描述符还未被设置为有效的文件描述符。
      在随后的代码中，当有新的套接字文件描述符需要加入到监视列表中时，会将其存储在 client 数组中的一个空闲位置，以便在 select() 函数调用时进行监视。
    */
    for (i = 0; i < FD_SETSIZE; i++) {
        client[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listen_fd, &allset);

    while (1) {

        rset = allset;
        num_fd = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(listen_fd, &rset)) {// listen fd ready, accept it 
            // accept
            connfd = accept(listen_fd, (struct sockaddr*)&client_addr, &clilen);

            for (i = 0; i < FD_SETSIZE; i++) {
                // find the min client not filled with connfd
                if (client[i] < 0) {
                    break;
                }
            }

            if (i == FD_SETSIZE) {// all of clients are filled
                printf("too many clients\n");
                exit(0);
            }

            //add connect fd
            client[i] = connfd;
            FD_SET(connfd, &allset);
            printf("client %s connnection\n", inet_ntoa(client_addr.sin_addr));

            if (connfd > maxfd) {
                maxfd = connfd;
            }
            if (i > maxi) {
                maxi = i;
            }
            if (--num_fd <= 0) continue;
        }

        // some connfd ready
        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client[i]) < 0) continue;
            // if sockfd is set
            if (FD_ISSET(sockfd, &rset)) {
                if ((n = readline(sockfd, line, MAXLINE)) == 0) { //read end
                    close(sockfd);
                    // clear sockfd from allset
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }
                else {
                    //read not end 
                    writen(sockfd, line, n);
                    writen(files, line, n);
                }
                if (--num_fd <= 0) break;
            } // if
        } // for
    }// while
    close(files);
    close(listen_fd);
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

