/*********************
sample: ipv4 ping sample
Files:
     iphdr.h ping.cpp resolve.cpp resolve.h

Description:
     This sample illustrates how to use raw sockets to send icmp
     echo requests and receive their response. This sample performs
     ipv4 icmp echo requests. When using raw sockets, the protocol
     value supplied to the sockets API is used as the protocol field
     (or next header field) of the IP packets. Then as a part of the
     data submitted to sendto, we include both the icmp request and data.

Compile:
     cl -o myping.exe ping.cpp resolve.cpp ws2_32.lib

Command Line Options:
     myping.exe [-i ttl] [-l datasize] [-r] [host]
        -i ttl		TTL value to set on socket
        -l size 	Amount of data to send as part of the icmp request
        -r 			use ipv4 record route
        host		Hostname or literal address
*********************/

# pragma comment(lib,"Ws2_32.lib")

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "resolve.h"
#include "iphdr.h"

#define DEFAULT_DATA_SIZE	32
#define DEFAULT_SEND_COUNT  4
#define DEFAULT_RECV_TIMEOUT 6000
#define DEFAULT_TTL 128

int 	gAddressFamily	=AF_UNSPEC,
  gProtocol		=IPPROTO_ICMP,
  gTtl			=DEFAULT_TTL,
  gDataSize		=DEFAULT_DATA_SIZE;
BOOL bRecordRoute=FALSE;
char *gDestination=NULL;

//Function: usage
//Description:
//     Print usage information.
void usage(char *progname)
{
  printf("		-i ttl 		Time to live\n");
  printf("		-l bytes	Amount of data send\n");
  printf("		-r			Record route(Ipv4 only)\n");
  printf("		host		Remote machine to ping\n");
  ExitProcess(-1);
}

//Function:InitIcmpHeader
//Description:
//     Helper Function to fill invarious stuff in our icmp request.
void InitIcmpHeader(char *buf, int datasize)
{
  ICMP_HDR *icmp_hdr=NULL;
  char *datapart=NULL;

  icmp_hdr=(ICMP_HDR *)buf;
  icmp_hdr->icmp_type=ICMPV4_ECHO_REQUEST_TYPE;
  icmp_hdr->icmp_code=ICMPV4_ECHO_REQUEST_CODE;
  icmp_hdr->icmp_id=(USHORT)GetCurrentProcessId();
  icmp_hdr->icmp_checksum=0;
  icmp_hdr->icmp_sequence=0;
  icmp_hdr->icmp_timestamp=GetTickCount();

  datapart=buf+sizeof(ICMP_HDR);
  memset(datapart,'E',datasize);
}

//Function:checksum
//Description:
//     This function calculates the 16-bit one's complement sum of the
//     supplied buffer(icmp) header.
USHORT checksum(USHORT *buffer, int size)
{
  unsigned long cksum=0;
  
  while(size>1){
	cksum+=*buffer++;
	size-=sizeof(USHORT);
  }
  if(size){
	cksum+=*(UCHAR*)buffer;
  }
  cksum=(cksum >> 16)+(cksum & 0xffff);
  cksum+=(cksum >> 16);
  return (USHORT)(~cksum);
}

//Function:ValidateArgs
//Description:
//     Parse the command line arguments.
void ValidateArgs(int argc, char **argv)
{
  int i;
  for(i=1;i<argc;i++){
	if((argv[i][0]=='-') || (argv[i][0]=='/')){
	  switch(tolower(argv[i][1])){
	  case 'i':
		if(i+1>=argc) usage(argv[0]);
		gTtl=atoi(argv[++i]);
		break;
	  case 'l':
		if(i+1>=argc) usage(argv[0]);
		gDataSize=atoi(argv[++i]);
		break;
	  case 'r':
		bRecordRoute=TRUE;
		break;
	  default:
		usage(argv[0]);
		break;
	  }//switch
	}//if
	else
	  gDestination=argv[i];
  }//for
  
  return;
}

//Function:SetIcmpSequence
//Description:
//     This routine sets the sequence number of the icmp request packet.
void  SetIcmpSequence(char *buf)
{
  ULONG sequence=0;
  
  sequence=GetTickCount();
  if(gAddressFamily==AF_INET){
	ICMP_HDR *icmpv4=NULL;
	icmpv4=(ICMP_HDR *)buf;
	icmpv4->icmp_sequence=(USHORT)sequence;
  }
}

//function:ComputeIcmpChecksum
//Description:
void ComputeIcmpChecksum(SOCKET s, char *buf, int packetlen, struct addrinfo *dest)
{
  if(gAddressFamily==AF_INET){
	ICMP_HDR *icmpv4=NULL;
	icmpv4=(ICMP_HDR *)buf;
	icmpv4->icmp_checksum=0;
	icmpv4->icmp_checksum=checksum((USHORT *)buf, packetlen);
  }
}

//Function:PostRecvFrom
//Description:
//     This routine posts an overlapped WSARecvFrom on the raw socket.
int PostRecvFrom(SOCKET s, char *buf, int buflen, SOCKADDR *from ,int *fromlen, WSAOVERLAPPED *ol)
{
  WSABUF wbuf;
  DWORD flags, bytes;
  int rc;

  wbuf.buf=buf;
  wbuf.len=buflen;
  flags=0;
  rc=WSARecvFrom(s, &wbuf, 1, &bytes, &flags, from, fromlen, ol, NULL);
  if(rc==SOCKET_ERROR){
	if(WSAGetLastError()!=WSA_IO_PENDING){
	  fprintf(stderr, "WSARecvFrom failed: %d\n", WSAGetLastError());
	  return SOCKET_ERROR;
	}
  }
  return NO_ERROR;	  
}

//Function:PrintPayload
//Description:
//     This routine is for ipv4 only. It determines if there are any IP options
//     present(by seeing if the ip header length is greater than 20 bytes) and 
//     if so it prints the ip record route options.
void PrintPayload(char *buf, int bytes)
{
  int hdrlen=0, routes=0, i;
  if(gAddressFamily==AF_INET){
	SOCKADDR_IN hop;
	IPV4_OPTION_HDR *v4opt=NULL;
	IPV4_HDR *v4hdr=NULL;

	hop.sin_family=(USHORT)gAddressFamily;
	hop.sin_port=0;
	v4hdr=(IPV4_HDR *)buf;
	hdrlen=(v4hdr->ip_verlen & 0x0f)*4;

	if(hdrlen>sizeof(IPV4_HDR)){
	  v4opt=(IPV4_OPTION_HDR *)(buf+sizeof(IPV4_HDR));
	  routes=(v4opt->opt_ptr / sizeof(ULONG)) - 1;
	  for (i=0;i<routes;i++) {
		hop.sin_addr.s_addr=v4opt->opt_addr[i];

		if(i==0)
		  printf("   Route:");
		else
		  printf("        ");
		PrintAddress((SOCKADDR *)&hop, sizeof(hop));

		if (i<routes-1)
		  printf("->\n");
		else
		  printf("\n");
	  }//for
	}//if hdr
  }//if gaddr
  return;
}

//Function:SetTtl
//Description:Sets the TTL on the socket.
int SetTtl(SOCKET s, int ttl)
{
  int optlevel, option, rc;
  rc=NO_ERROR;
  if(gAddressFamily==AF_INET){
	optlevel=IPPROTO_IP;
	option=IP_TTL;
  }else
	rc=SOCKET_ERROR;

  if(rc==NO_ERROR)
	rc=setsockopt(s, optlevel, option, (char *)&ttl, sizeof(ttl));
  if(rc==SOCKET_ERROR)
	fprintf(stderr, "SetTtl: setsockopt failed: %d\n",WSAGetLastError());
  
  return rc;
}

//Function:main
//Description:
//      Setup the icmp raw socket and create the icmp header. Add
//		the appropriate ip option header and start sending icmp
//		echo request to the endpoint. For each send and receive we 
//		set a timeout value so that we don'n wait forever for a 
//		reponse in case the endpoint is not responding. When we 
//		receive a packet decode it.
int _cdecl main(int argc, char **argv)
{
  WSADATA wsd;
  WSAOVERLAPPED recvol;
  SOCKET s=INVALID_SOCKET;
  char *icmpbuf=NULL, recvbuf[0xffff];
  struct addrinfo *dest=NULL, *local=NULL;
  IPV4_OPTION_HDR ipopt;
  SOCKADDR_STORAGE from;
  DWORD bytes, flags;
  int packetlen=0, recvbuflen=0xffff, fromlen, time=0, rc, i;

  //load winsock
  if(WSAStartup(MAKEWORD(2,2), &wsd)!=0){
	printf("WSAStartup failer: %d\n", WSAGetLastError());
	return -1;
  }

  //parse the command line
  ValidateArgs(argc,argv);

  //获得命令行的目的地址
  dest=ResolveAddress(gDestination, NULL, gAddressFamily, 0,0);
  if(dest==NULL){
	printf("bad name %s\n", gDestination);
	return -1;
  }
  gAddressFamily=dest->ai_family;

  if(gAddressFamily==AF_INET)
	gProtocol=IPPROTO_ICMP;

  //获得本机地址
  char *localhost = new char[strlen("172.20.81.146") + 1];
  strcpy_s(localhost, strlen("172.20.81.146") + 1, "172.20.81.146");
  local = ResolveAddress(localhost, NULL, gAddressFamily, 0, 0);
  if(local==NULL){
	printf("Unable to obtain the bind address!\n");
	return -1;
  }

  //create raw socket
  s=socket(gAddressFamily, SOCK_RAW, gProtocol);
  if (s==INVALID_SOCKET){
	printf("socket failed: %d\n",WSAGetLastError());
	return -1;
  }
  SetTtl(s,gTtl);

  //figure out the size of the icmp header and payload
  if(gAddressFamily==AF_INET)
	packetlen+=sizeof(ICMP_HDR);

  packetlen+=gDataSize;
  
  //alolocate the buffer that will contain the icmp request
  icmpbuf=(char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, packetlen);
  if(icmpbuf==NULL){
	fprintf(stderr, "HeapAlloc failed: %d\n", GetLastError());
	return -1;
  }

  //initialize the icmp headers
  if(gAddressFamily==AF_INET){
	if(bRecordRoute){
	  ZeroMemory(&ipopt, sizeof(ipopt));
	  ipopt.opt_code=IP_RECORD_ROUTE;
	  ipopt.opt_ptr=4;
	  ipopt.opt_len=39;

	  rc=setsockopt(s, IPPROTO_IP, IP_OPTIONS, (char *)&ipopt, sizeof(ipopt));
	  if(rc==SOCKET_ERROR){
		fprintf(stderr, "setsockopt(IP_OPTIONS) failed: %d\n",WSAGetLastError());
		return -1;
	  }
	}//if brecord
	InitIcmpHeader(icmpbuf, gDataSize);
  }//if gaddr
				
  //bind the socket
  rc = bind(s, local->ai_addr, local->ai_addrlen);
  if (rc==SOCKET_ERROR) {
	fprintf(stderr, "bind failed: %d\n",WSAGetLastError());
	return -1;   
  }

  memset(&recvol,0,sizeof(recvol));
  //为WSAOVERLAPPED结构分配一个事件对象句柄
  recvol.hEvent= WSACreateEvent();
  
  //post the first overlapped receive
  fromlen=sizeof(from);
  //在套接字上投递一个异步WSARecv请求，指定参数为WSAOVERLAPPED结构。
  PostRecvFrom(s, recvbuf, recvbuflen, (SOCKADDR *)&from, &fromlen, &recvol);

  printf("\nPinging ");
  PrintAddress(dest->ai_addr, dest->ai_addrlen);
  printf(" with %d bytes of data\n\n", gDataSize);
  
  //start sending the icmp requests
  for(i=0;i<DEFAULT_SEND_COUNT; i++){
	SetIcmpSequence(icmpbuf);
	ComputeIcmpChecksum(s, icmpbuf, packetlen, dest);
  
	time=GetTickCount();
	rc=sendto(s, icmpbuf, packetlen, 0, dest->ai_addr, dest->ai_addrlen);
	if(rc==SOCKET_ERROR){
	  fprintf(stderr, "sendto failed: %d\n",WSAGetLastError());
	  return -1;
	}

	//wait for a response
	//调用WSAWaitForMultipleEvents函数，并等待与重叠调用关联在一起的事件进入“已传信”状态（即，等待那个事件的“触发”）。
	rc = WSAWaitForMultipleEvents(1, &recvol.hEvent, TRUE, DEFAULT_RECV_TIMEOUT, FALSE);
	if(rc==WAIT_FAILED){
	  fprintf(stderr, "WaitForSingleObject failed: %d\n", GetLastError());
	  return -1;
	}
	else if (rc==WAIT_TIMEOUT){
	  printf("Request timed out.\n");
	}
	else{
	  //ppt“重叠I/O编程步骤”中的7：使用WSAGetOverlappedResult函数，判断重叠调用的返回状态是什么。
	  rc = WSAGetOverlappedResult(s, &recvol, &bytes, FALSE, &flags);
	  if(rc==FALSE)
		fprintf(stderr, "WSAGetOverlappedResult failed: %d\n", WSAGetLastError());
	  
	  time=time-GetTickCount();
	  //ppt“重叠I/O编程步骤”中的6：WSAWaitForMultipleEvents函数完成后，针对事件数组，调用WSAResetEvent（重设事件）函数
	  WSAResetEvent(recvol.hEvent);
	  printf("Reply from ");
	  PrintAddress((SOCKADDR *)&from, fromlen);
	  if(time<=0)
		printf(":bytes=%d time<1ms TTL=%d\n",gDataSize, gTtl);
	  else
		printf(":bytes=%d time=%dms TTL=%d\n", gDataSize, time ,gTtl);

	  PrintPayload(recvbuf, bytes);
	  if(i<DEFAULT_SEND_COUNT){
		fromlen=sizeof(from);
		//ppt“重叠I/O编程步骤”中的8：在套接字上投递另一个重叠WSARecv请求
		PostRecvFrom(s, recvbuf, recvbuflen, (SOCKADDR*)&from, &fromlen, &recvol);
	  }
	}//else
	Sleep(1000);
  }//for
  
  //cleanup
  freeaddrinfo(dest);
  freeaddrinfo(local);
  if(s!=INVALID_SOCKET)
	closesocket(s);
  HeapFree(GetProcessHeap(),0,icmpbuf);

  WSACleanup();
  return 0;
}
