//////////////////////////////////////////////////////////
// TCPClient.cpp�ļ�


#include "InitSock.h"
#include <stdio.h>
CInitSock initSock;		// ��ʼ��Winsock��

int main()
{
	// �����׽���
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{
		printf(" Failed socket() \n");
		return 0;
	}
	
	// Ҳ�������������bind������һ�����ص�ַ
	// ����ϵͳ�����Զ�����
	
	// ��дԶ�̵�ַ��Ϣ
	sockaddr_in servAddr; 
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(4567);
	// ע�⣬����Ҫ��д����������TCPServer�������ڻ�����IP��ַ
	// �����ļ����û��������ֱ��ʹ��127.0.0.1����
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	if(::connect(s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
	{
		printf(" Failed connect() \n");
		return 0;
	}
	
	// �������ݣ������Լ��ظ���������
	char buff[]="hello server...\nmore and more";
	int nSend = ::send(s, buff, strlen(buff), 0);
/*	if(nSend > 0)
	{
		buff[nSend] = '\0';
		printf(" �������ݣ�%s", buff);
	}
	*/
	// �ر��׽���
	::closesocket(s);
	return 0;
}
