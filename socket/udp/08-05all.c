/* Read “n” bytes from a descriptor.
use in place of read() when fd is a stream socket. */

int	 readn(fd,ptr,nbytes)
register int fd;
register char *ptr;
register int nbytes;
{
  int 	nleft,nread;
  
  nleft=nbytes;
  while(nleft>0)
{
  nread=read(fd,ptr,nleft);
  if ( nread < 0)
     return(nread);		/* error,return < 0 */
    else if (nread==0)
           break;		/* EOF */
  nleft - = nread;
      ptr  + = nread;
}
  return(nbytes-nleft);		/* return >= 0 */
}
   The following function writes to a stream socket.

/* Write “n” bytes to a descriptor.
use in place of write() when fd is a stream socket. */

int  writen(fd,ptr,nbytes)
register int fd;
register char *ptr;
register int nbytes;
{
  int 	nleft,nwritten;
  
  nleft=nbytes;
  while(nleft>0)
{
  nwritten=write(fd,ptr,nleft);
  if ( nwritten <= 0)
     return(nwritten);		/* error,return <= 0 */
    
  nleft - = nwritten;
      ptr  + = nwritten;
}
  return(nbytes-nleft);		/* return >= 0 */
}
  The following function can be used to read a line from a stream socket. It is frequently used in UNIX.
/* Read a line from a descriptor. Read the line one byte at a time,
* looking for the newline.We store the newline in the buffer,then
* follow it with a null (the same as fgets(3)). We return the number of * characters up to,but not including the null (the same as strlen(3)).
*/

int 	readline(fd,ptr,maxlen)
register int fd;
register char *ptr;
register int maxlen;
{
  int n,rc;
  char c;

  for (n=1;n<maxlen;n++)
{
  if (rc=read(fd,&c,1))==1)
    {
      *ptr++=c;
      if (c==’\n’)  break;
     }
    else if (rc==0)
         {
           if (n==1)   return (0);	/* EOF,no data read */
             else bread;	  /* EOF,some data was read*/
          }
         else  retuen(-1);		/* error */
}
  *ptr=0;
  return(n);
}
下面四个实例用于演示流方式（面向连接）和数据报方式socket的工作原理.
/* Read s tream socket one line at a time,and write each line 
* back to the sender.
* Return when the connection is terminated.
*/

#define MAXLINE	512

stream_echo (sockfd)
int 	sockfd;
{
  int 	n;
  char 	line[MAXLINE];

  for ( ; ; )
{
  n=readline(sockfd,line,MAXLINE);
  if (n == 0)
      return;		/*connection terminated */
    else if (n<0)
           err_dump(“stream_echo:readline error”);
  if (written(sockfd,line,n)!=n)
     err_dump(“stream_echo:written error”);
}
}
/* Read the contents of the FILE *fd,and write each line to the stream
* socket(to the server process),then read a line back from the socket
* and write it to the standard output.
* Return to caller when an EOF is encountered on the input file.
*/

#include <stdio.h>
#define MAXLINE	512

stream_client (fp,sockfd)
register FILE *fd;
register int 	sockfd;
{
  int 	n;
  char 	sendline[MAXLINE],recvline[MAXLINE+1];

  while (fgets(sendline,MAXLINE,fd) != NULL)
{
  n=strlen(sendline);
  if (written(sockfd,sendline,n) != n)
     err_sys(“stream_client:written error on socket”);
 /* now read a line from the socket and write it to 
* our standard output.
*/
n=readline(sockfd,recvline,MAXLINE);
if (n < 0)
   err_dump(“stream_client:readline error”);
fputs(recvline,stdout);  
}
if (ferror(fp))
   err_sys(“stream_client:error reading file”);
}
1．数据报收发演示
/* Read a datagram from a connectionless socket 
* and write it back to the sender.
* We never return,as we never know when a datagram chlient is done.
*/

#include <sys/types.h>
#include <sys/socket.h>

#define MAXMSG	2048

dg_echo(sockfd,pcli_addr,maxclilen)
int		sockfd;
struct sockaddr *pcli_addr;	/* ptr to appropriate sockaddr_xx structure */
int      maxclilen;	/* sizeof(pcli_addr) */
{
  int  n,clilen;
  char mesg[MAXMSG];

  for ( ; ; )
    {
      clilen=maxclilen;
      n=recvfrom(fockfd,mesg,MASMSG,0,pcli_addr,&clilen);
      if (n < 0 )
         err_dump(“dg_echo:recvfrom error”);
      if (sendto(sockfd,mesg,n,0,pcli_addr,clilen)!=n)
         err_dump(“dg_echo:sendto error”);
    }
}
/* Read the contents of the FILE *fd,write each line to the datagram * cket,then read a line back from the datagram socket and write it * the standard output.

*  Return to caller when an EOF is encountered on the input file.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLINE	512

dg_clieng(fp,sockfd,pserv_addr,servlen)
FILE *fp;
int   sockfd;
struct sockaddr *pserv_addr;
int 	  servlen;
{
  int n;
  char sendline[MAXLINE],recvline[MAXLINE+1];
  
  while(fgets(sendline,MAXLINE,fp)!=NULL)
{
  n=strlen(sendline);
  if (sendto(sockfd,sendline,n,0,pserv_addr,servlen)!=n)
     err_dump(“dg_client:sendto error on socket”);
 /* now read a message from the socket and write it 
* to our standard output. */
n=recvfrom(sockfd,recvline,MAXLINE,0,(struct sockaddr *)0,(int *)0);
if (n < 0 )
   err_dump(“dg_client:recvfrom error”);
recvline[n]=0;		/* null terminate *;
fputs(recvline,stdout);
}
  if (ferror(fp))
    err_dump(“dg_client:error reading file.”);
}

1.	TCP example
/*
/* definitions for TCP and UDP client/server programs.
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_UDP_PORT	6543
#define SERV_TCP_PORT	6543
#define SERV_HOST_ADDR	 “192.43.235.6”

char *pname;

/*
/* Example of server using TCP protocol.服务器端
*/

#include “inet.h”

main(argc,argv)
int	argc;
char *argc[];
{
  int 	sockfd,newsockfd,clildlen,childid;
  struct sockaddr_in	 cli_addr,serv_addr;

  pname=argv[0];
/* open a TCP socket (an Internet stream socket) */

  if ((sockfd=socket(AR_INET,SOCK_STREAM,0)<0)
err_dump(“server:can’t open stream socket.”);

/* Bind our local address so that the client can send to us. */
  bzero(char *)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port      =htons(SERV_TCP_PORT);

  if (bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))
err_dump(“server: can’t bind local address.”);
  listen(sockfd,5);
  for( ; ; )
{
/* Wait for a connection from a client process.
*This is an example of a concurrent server.
*/
 clildlen=sizeof(cli_addr);
 newsockfd=accept(sockfd,(struct sockaddr *)&cli_addr,&clildlen);
 if (newsockfd<0)
   err_dump(“server: accept error.”);
 if ((childpid=fork())<0)
   err_dump(“server: fork error.”);
  else if (childpid==0)
        { /* child process */
          close(sockfd);
          stream_echo(newsockfd);/* process the request */
          exit(0);
         }
        close(newsockfd);			/* parent process */
}      
}
 
======================================
/*
/* Example of client using TCP protocol.客户端
*/

#include “inet.h”

main(argc,argv)
int	argc;
char *argc[];
{
  int 	sockfd;
  struct sockaddr_in	 serv_addr;

  pname=argv[0];

/* Fill in the structure “serv_addr” with the address of 
the server that we want to connect with. */

  bzero(char *)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=inet_addr(SERV_HOST_ADDR);
  serv_addr.sin_port      =htons(SERV_TCP_PORT);

/* open a TCP socket (an Internet stream socket) */

  if ((sockfd=socket(AR_INET,SOCK_STREAM,0)<0)
err_dump(“client:can’t open stream socket.”);

/* connect to the server  */

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))
err_dump(“client: can’t connect to the server.”);
 
stream_client(stdin,sockfd);		/* do it all */
close (sockfd);
exit(0);
}
2.	UDP example
/*
/* Example of server using UDP protocol.服务器端
*/

#include “inet.h”

main(argc,argv)
int	argc;
char *argc[];
{
  int 	sockfd ;
  struct sockaddr_in	 cli_addr,serv_addr;

  pname=argv[0];

/* open a UDP socket (an Internet stream socket) */

  if ((sockfd=socket(AR_INET,SOCK_DGRAM,0)<0)
err_dump(“server:can’t open datagram socket.”);

/* Bind our local address so that the client can send to us. */
  bzero(char *)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port      =htons(SERV_UDP_PORT);

 if (bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
err_dump(“server: can’t bind local address.”);
  dg_echo(sockfd,(struct sockaddr *)&cli_addr,sizeof(cli_addr));
                 /* NOTREACHED */
}
 
======================================
/*
/* Example of client using UDP protocol.客户端
*/

#include “inet.h”

main(argc,argv)
int	argc;
char *argc[];
{
  int 	sockfd;
  struct sockaddr_in	 serv_addr,cli_addr;

  pname=argv[0];

/* Fill in the structure “serv_addr” with the address of 
the server that we want to connect with. */

  bzero(char *)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=inet_addr(SERV_HOST_ADDR);
  serv_addr.sin_port      =htons(SERV_UDP_PORT);

/* open a UDP socket (an Internet datagram socket) */

  if ((sockfd=socket(AR_INET,SOCK_DGRAM,0)<0)
err_dump(“client:can’t open datagram socket.”);

/* bind any local address for us */

bzero(char *)&cli_addr,sizeof(cli_addr));
  cli_addr.sin_family=AF_INET;
  cli_addr.sin_addr.s_addr=inet_addr(INADDR_ANY);
  cli_addr.sin_port      =htons(0);
  if (bind(sockfd,(struct sockaddr *)&cli_addr,sizeof(cli_addr))<0)
err_dump(“client: can’t bind local address.”);

dg_client(stdin,sockfd,(struct sockaddr *)&serv)addr,sizeof(serv_addr));	
close (sockfd);
exit(0);
}
