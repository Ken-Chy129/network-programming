共有两个程序：其中server程序不完整，需要在仔细阅读程序的前提下填出其中空白（以...............）表示的语句。

1. 客户端程序：
使用select来将文件读取和socket混合在一起。当读取文件准备就绪时，读取文件并发送到服务器；当socket就绪时，从server读数据。
客户端命令行：select_cli server_addr file_name

2.服务器端程序：
用select处理多个客户端请求，测试时可以同时启动多个客户端程序。
先读懂其中的maxi，maxfd，num_fd, client[FD_SETSIZE]几个变量的作用，再考虑select的使用方法。
