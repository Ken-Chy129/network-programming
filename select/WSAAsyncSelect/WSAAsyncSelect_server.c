#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <string.h>

#define USERPORT 10001
//#define HOST_IP_ADDR "192.1.128.128"

int main(int argc, char** argv)
{
	char buf[1024];
	struct sockaddr_in client;
	struct sockaddr_in server;
	int s;
	int ns;
	int namelen;
	int pktlen;
	int ret;

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket()");
		return -1;
	}

	bzero((char*)&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(USERPORT);
	server.sin_addr.s_addr = INADDR_ANY;
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
		perror("Bind()");
		return -1;
	}
	if (listen(s, 1) != 0) {
		perror("Listen()");
		return -1;
	}

	namelen = sizeof(client);
	while (1) {
		if ((ns = accept(s, (struct sockaddr*)&client, (socklen_t*)&namelen)) == -1) {
			perror("Accept()");
			return -1;
		} //if

		printf("accept from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		ret = fork();
		if (ret < 0) {
			printf("fork error\n");
			exit(-1);
		}
		else if (ret == 0) {
			for (;;) {
				if ((pktlen = recv(ns, buf, 1024, 0)) < 0) {
					perror("Recv()");
					break;
				}
				else if (pktlen == 0) {
					printf("Recv():return FAILED,connection is shut down!\n");
					break;
				}//else 
				  //printf("Recv():return SUCCESS,packet length = %d\n",pktlen); 

				sleep(1);
				if (send(ns, buf, pktlen, 0) < 0) {
					perror("Send()");
					break;
				}//else
				//printf("Send():return SUCCESS,packet length = %d\n",pktlen); 
			}//for
		}//else if
		close(ns);
	}//while

	//close(ns); 
	close(s);
	printf("Server ended successfully\n");
	return 0;
}

