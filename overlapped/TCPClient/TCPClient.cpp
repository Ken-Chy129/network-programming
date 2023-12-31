//////////////////////////////////////////////////////////
// TCPClient.cpp文件


#include "InitSock.h"
#include <stdio.h>
CInitSock initSock;		// 初始化Winsock库

int main()
{
	// 创建套节字
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{
		printf(" Failed socket() \n");
		return 0;
	}
	
	// 也可以在这里调用bind函数绑定一个本地地址
	// 否则系统将会自动安排
	
	// 填写远程地址信息
	sockaddr_in servAddr; 
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(4567);
	// 注意，这里要填写服务器程序（TCPServer程序）所在机器的IP地址
	// 如果你的计算机没有联网，直接使用127.0.0.1即可
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	if(::connect(s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
	{
		printf(" Failed connect() \n");
		return 0;
	}
	
	// 发送数据，可以自己重复定义数据
	char buff[]="hello server...\nmore and more";
	int nSend = ::send(s, buff, strlen(buff), 0);
/*	if(nSend > 0)
	{
		buff[nSend] = '\0';
		printf(" 发送数据：%s", buff);
	}
	*/
	// 关闭套节字
	::closesocket(s);
	return 0;
}
