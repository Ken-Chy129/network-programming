/*********************
Common routines for resolving address and hostnames

Files:
      resolve.cpp
      resolve.h
 *********************/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "resolve.h"

//Function: PrintAddress
//Description:
//      This routine takes a SOCKADDR structure and its length and prints
//      converts it to a string representation. This string is printed to
//      the console via stdout.
int PrintAddress(SOCKADDR *sa, int salen)
{
  char host[NI_MAXHOST], serv[NI_MAXSERV];
  int hostlen=NI_MAXHOST, servlen=NI_MAXSERV, rc;

  rc=getnameinfo( sa, salen, host, hostlen, serv, servlen, NI_NUMERICHOST|NI_NUMERICSERV);
  
  if(rc!=0){
	fprintf(stderr, "%s: getnameinfo failed:%d\n", __FILE__, rc);
	return rc;
  }

  //if the port is zero then don't print it
  if(strcmp(serv, "0")!=0){
	if(sa->sa_family==AF_INET)
	  printf("[%s]:%s", host, serv);
	else
	  printf("%s:%s", host, serv);
  }else
	printf("%s", host);

  return NO_ERROR;  
}


//Function: FormatAddress
//
//Description:
//      This is similar the PrintAddress function except that instead of
//      printing the string address to the console, it is formatted into 
//      the supplied string buffer.
int FormatAddress(SOCKADDR *sa, int salen, char *addrbuf, int addrbuflen)
{
  char host[NI_MAXHOST], serv[NI_MAXSERV];
  int hostlen=NI_MAXHOST, servlen=NI_MAXSERV, rc;

  rc=getnameinfo( sa, salen, host, hostlen, serv, servlen, NI_NUMERICHOST|NI_NUMERICSERV);
  
  if(rc!=0){
	fprintf(stderr, "%s: getnameinfo failed:%d\n", __FILE__, rc);
	return rc;
  }

  if(strlen(host) + strlen(serv) +1 > (unsigned)addrbuflen)
	return WSAEFAULT;
  if(sa->sa_family==AF_INET)
	sprintf_s(addrbuf,sizeof(addrbuf),"%s:%s", host, serv);
  else
	addrbuf[0]='\0';

  return NO_ERROR;
}


//Function: ResolveAddress
//Description:
//    This routine resolves the specified address and returns a list of addrinfo
//    structure containing SOCKADDR structures representing the resolved address.
//    Note that if 'addr' is non-NULL, then getaddrinfo will resolve it whether 
//    it is a string literal address or a hostname.
struct addrinfo *ResolveAddress(char *addr, char *port, int af, int type, int proto)
{
  struct addrinfo hints, *res=NULL;
  int rc;

  memset(&hints,0, sizeof(hints));
  hints.ai_flags=((addr)?0:AI_PASSIVE);
  hints.ai_family=af;
  hints.ai_socktype=type;
  hints.ai_protocol=proto;

  rc=getaddrinfo(addr, port,&hints, &res);
  if(rc!=0){
	  printf("Invalid address %s, getaddrinfo failed:%d\n", addr, rc);
	  return NULL;
  }

  return res;  
}

//Function: ReserveLookup
//Description:
//      This routine takes a SOCKADDR and does a reverse lookup for the name 
//      corresponding to that address.
int ReverseLookup(SOCKADDR *sa, int salen, char *buf, int buflen)
{
  char host[NI_MAXHOST];
  int hostlen=NI_MAXHOST, rc;

  rc=getnameinfo(sa, salen, host, hostlen, NULL, 0,0);
  if(rc!=0){
	fprintf(stderr, "getnameinfo failed: %d\n",rc);
	return rc;
  }
  strcpy_s(buf,sizeof(buf),host);

  return NO_ERROR;
}
