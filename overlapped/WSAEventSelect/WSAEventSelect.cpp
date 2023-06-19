//////////////////////////////////////////////////
// WSAEventSelect�ļ�

#include "initsock.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>

// ��ʼ��Winsock��
CInitSock theSock;

int main()
{
	// �¼�������׽��־����
	WSAEVENT	eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET		sockArray[WSA_MAXIMUM_WAIT_EVENTS];
	int nEventTotal = 0;

	USHORT nPort = 4567;	// �˷����������Ķ˿ں�

	// ���������׽���
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if(::bind(sListen, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf(" Failed bind() \n");
		return -1;
	}
	::listen(sListen, 5);

	// �����¼����󣬲��������µ��׽���
	WSAEVENT event = ::WSACreateEvent();
	::WSAEventSelect(sListen, event, FD_ACCEPT|FD_CLOSE);
	// ��ӵ�����
	eventArray[nEventTotal] = event;
	sockArray[nEventTotal] = sListen;	
	nEventTotal++;

	// ���������¼�
	while(TRUE)
	{
		// �������¼������ϵȴ�, ��ÿ���¼�����WSAWaitForMultipleEvents�������Ա�ȷ������״̬
		int nIndex = WSAWaitForMultipleEvents(nEventTotal, eventArray, false, WSA_INFINITE, false);
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		for(int i=nIndex; i<nEventTotal; i++)
		{	
			//��Ծ����¼��ٴε��ã��Ա�ȷ���Ƿ����¼�
			nIndex = WSAWaitForMultipleEvents(1, &eventArray[i], false, WSA_INFINITE, false);
			if(nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
			else
			{
				// ��ȡ������֪ͨ��Ϣ��WSAEnumNetworkEvents�������Զ����������¼�
				WSANETWORKEVENTS event;
				WSAEnumNetworkEvents(sockArray[i], eventArray[i], &event);
				if(event.lNetworkEvents & FD_ACCEPT)				// ����FD_ACCEPT֪ͨ��Ϣ
				{
					if(event.iErrorCode[FD_ACCEPT_BIT] == 0)
					{
						if(nEventTotal > WSA_MAXIMUM_WAIT_EVENTS)
						{
							printf(" Too many connections! \n");
							continue;
						}
						SOCKET sNew = ::accept(sockArray[i], NULL, NULL);
						WSAEVENT event = ::WSACreateEvent();
						WSAEventSelect(sNew, event, FD_READ | FD_WRITE | FD_CLOSE);
						// ��ӵ�����
						eventArray[nEventTotal] = event;
						sockArray[nEventTotal] = sNew;	
						nEventTotal++;
					}
				}
				else if(event.lNetworkEvents & FD_READ)			// ����FD_READ֪ͨ��Ϣ
				{
					if(event.iErrorCode[FD_READ_BIT] == 0)
					{
						char szText[256];
						int nRecv = ::recv(sockArray[i], szText, strlen(szText), 0);
						if(nRecv > 0)				
						{
							szText[nRecv] = '\0';
							printf("���յ����ݣ�%s \n", szText);
						}
					}
				}
				else if(event.lNetworkEvents & FD_CLOSE)		// ����FD_CLOSE֪ͨ��Ϣ
				{
					if(event.iErrorCode[FD_CLOSE_BIT] == 0)
					{
						::closesocket(sockArray[i]);
						for(int j=i; j<nEventTotal-1; j++)
						{
							sockArray[j] = sockArray[j+1];
							sockArray[j] = sockArray[j+1];	
						}
						nEventTotal--;
					}
				}
				else if(event.lNetworkEvents & FD_WRITE)		// ����FD_WRITE֪ͨ��Ϣ
				{
				}
			}
		}
	}
	return 0;
}