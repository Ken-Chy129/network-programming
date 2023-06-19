1. 编译并运行05-01.c
命令行：gcc -lpcap -o 05-01 -Wall 05-01.c
运行时使用root身份运行。
在05-01.c源代码中加入判断语句判断返回值是否正确。

2. 编译并运行dns.c
1）命令行：gcc -lnet -o dns -Wall dns.c
2）运行时使用root身份运行，注意命令行是：文件名 -d dst_ip -q query_host [-s src_ip] [-t]
其中，-d dst_ip是dns域名服务器地址，暨大可以用202.116.0.1，-q query_host是带查询的域名，这里要求用www.baidu.com
3）使用tcpdump监视dns查询及返回的数据包，tcpdump port 53。tcpdump这个程序位于/usr/sbin，可能需要写出完整路径才能运行。
使用带界面的wireshark也可以。

3. 将上两个程序合并，合并后功能为：用libnet发送一个dns请求数据包，并使用libpcap抓取响应包，然后输出dns响应查询到的ip地址。
注意：原有的dns.c程序有错误，发送的dns请求数据包不对。即：
	payload_s = snprintf(payload, sizeof payload, "%c%s%c%c%c%c%c", 
			 (char)(strlen(query)&0xff), query, 0x00, 0x00, 0x01, 0x00, 0x01);
	这句。
请自行google dns正确的格式，对比第2步用tcpdump监视的数据包哪里有错。然后才能得到正确的响应包。
或者，你可以在windows 中使用sniffer进行正确的dns查询，查看查询数据包中dns数据段的内容，这个dns包的包头是正确的。
	