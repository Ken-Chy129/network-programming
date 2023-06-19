本实验共用到4个程序：
1.08-05udp_server.c：建立udp server，然后调用08-05dg_echo.c
2.08-05dg_echo.c：接收客户端发送来的数据，并回传回去
3.08-05udp_client.c：建立udp client，然后以stdin参数调用08-05dg_client.c
4.08-05dg_client.c：以行为单位读入文件（08-05udp_client.c中的调用参数是stdin，因此是从标准输入读取）内容，发送到给udp server，然后接收udp server返回的数据

说明：
1.原有程序是以K&R函数原型写成，改成ANSI格式。
2.原有程序头文件未写，请自行填入。
3.原有程序中的err_dump函数未定义，请自己定义。这是一个严重错误，在调用时必须满足：输出err_dump参数中的字符串，然后用exit退出程序。
4.程序中的地址和端口使用#define自己写入。
