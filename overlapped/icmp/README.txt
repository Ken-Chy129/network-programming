��4���ļ�������Ҫ��д�Ĳ�����ping.cpp�С�

�������̲��裺
1. ����-Microsoft Visual Studio 2008-����Microsoft Visual Studio 2008
2. �ļ�-�½�-��Ŀ-��������-Visual c++
�ұ�ѡ��Win32����̨Ӧ�ó��������������ƣ�icmp
3. ��һ��
4. ����ѡ���У�ѡ������Ŀ����Ȼ����ɡ���
5. �ұߵġ����������Դ���������У�
ѡ��ͷ�ļ������Ҽ������-������-�ֱ�����ļ�iphdr.h��resolve.h
ѡ��Դ�ļ������Ҽ������-������-�ֱ�����ļ�Resolve.cpp��ping.cpp


ping.cpp�е�main����ִ�в��裺
1. ����ʹ�ñ���
����������ǣ�
WSAOVERLAPPED recvol;	�����ص��������ں�����
struct addrinfo *dest=NULL, *local=NULL;	����getaddrinfoϵͳ���ã����Ǹ�ipv4��ipv6ͨ�õĻ�ȡ��ַ��Ϣ�ĺ�����ipv4�汾�и����õ���gethostbyname����������http://msdn.microsoft.com/en-us/library/windows/desktop/ms738520(v=vs.85).aspx
2. WSAStartup
3. ���������в�����ValidateArgs(argc,argv);
4. ��ȡ�������е�Ŀ�ĵ�ַ��ResolveAddress()�����������Resolve.cpp�С�
5. ��ȡ������ַ

���²�����Բο��Ͽ�ʹ�õ�ppt���ص�I/O��̲��衱
7. ppt���ص�I/O��̲��衱�е�1������һ��raw socket
8. ��ʼ��icmpͷ
9. bind
10.ppt���ص�I/O��̲��衱�е�3��Ϊsocket�½�һ��WSAOVERLAPPED�ṹ����Ϊ�ýṹ����һ���¼���������Ҳ���¼������������һ���¼����飬�Ա��Ժ���WSAWaitForMultipleEvents����ʹ�á�
��������ֻ��һ��socket������ֻʹ����һ���¼������Ժ�ʹ�õ���WSAWaitForSingleObject��
11. ppt���ص�I/O��̲��衱�е�4��ʹ�ú�����Resolve.cpp�е�PostRecvFrom��
�������һ��᷵��һ��WSA_IO_PENDING����
12. ��Ŀ����������icmp request��
13. ppt���ص�I/O��̲��衱�е�5������WSAWaitForMultipleEvents���������ȴ����ص����ù�����һ����¼����롰�Ѵ��š�״̬�������ȴ��Ǹ��¼��ġ�����������
����ʹ�õ���WSAWaitForSingleObject��
14. ppt���ص�I/O��̲��衱�е�7��ʹ��WSAGetOverlappedResult�������ж��ص����õķ���״̬��ʲô��
15. ppt���ص�I/O��̲��衱�е�6��WSAWaitForMultipleEvents������ɺ�����¼����飬����WSAResetEvent�������¼�����������λ�¼����󣬲�����ɵ��ص�������д���
16. ppt���ص�I/O��̲��衱�е�8�����׽�����Ͷ����һ���ص�WSARecv����
17. �ظ��������衣



