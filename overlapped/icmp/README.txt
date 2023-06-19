有4个文件，其中要填写的部分在ping.cpp中。

创建工程步骤：
1. 程序-Microsoft Visual Studio 2008-启动Microsoft Visual Studio 2008
2. 文件-新建-项目-其他语言-Visual c++
右边选“Win32控制台应用程序，下面填入名称：icmp
3. 下一步
4. 附加选项中，选”空项目“，然后”完成“。
5. 右边的”解决方案资源管理器“中，
选择”头文件“，右键，添加-现有项-分别加入文件iphdr.h和resolve.h
选择”源文件“，右键，添加-现有项-分别加入文件Resolve.cpp和ping.cpp


ping.cpp中的main函数执行步骤：
1. 声明使用变量
其中特殊的是：
WSAOVERLAPPED recvol;	用于重叠操作的内核数据
struct addrinfo *dest=NULL, *local=NULL;	用于getaddrinfo系统调用，这是个ipv4和ipv6通用的获取地址信息的函数（ipv4版本中更常用的是gethostbyname）。函数见http://msdn.microsoft.com/en-us/library/windows/desktop/ms738520(v=vs.85).aspx
2. WSAStartup
3. 处理命令行参数：ValidateArgs(argc,argv);
4. 获取命令行中的目的地址：ResolveAddress()，这个函数在Resolve.cpp中。
5. 获取本机地址

以下步骤可以参考上课使用的ppt“重叠I/O编程步骤”
7. ppt“重叠I/O编程步骤”中的1：创建一个raw socket
8. 初始化icmp头
9. bind
10.ppt“重叠I/O编程步骤”中的3：为socket新建一个WSAOVERLAPPED结构，并为该结构分配一个事件对象句柄。也将事件对象句柄分配给一个事件数组，以便稍后由WSAWaitForMultipleEvents函数使用。
这里由于只有一个socket，所以只使用了一个事件对象，稍后使用的是WSAWaitForSingleObject。
11. ppt“重叠I/O编程步骤”中的4。使用函数是Resolve.cpp中的PostRecvFrom。
这个函数一般会返回一个WSA_IO_PENDING错误。
12. 向目的主机发送icmp request。
13. ppt“重叠I/O编程步骤”中的5：调用WSAWaitForMultipleEvents函数，并等待与重叠调用关联在一起的事件进入“已传信”状态（即，等待那个事件的“触发”）。
这里使用的是WSAWaitForSingleObject。
14. ppt“重叠I/O编程步骤”中的7：使用WSAGetOverlappedResult函数，判断重叠调用的返回状态是什么。
15. ppt“重叠I/O编程步骤”中的6：WSAWaitForMultipleEvents函数完成后，针对事件数组，调用WSAResetEvent（重设事件）函数，复位事件对象，并对完成的重叠请求进行处理。
16. ppt“重叠I/O编程步骤”中的8：在套接字上投递另一个重叠WSARecv请求。
17. 重复上述步骤。



