本程序用于了解和测试Visual Studio的使用。

服务器端：
在linux中编译运行，无需你去做，服务器已启动。。

客户端（WSAAsyncSelect程序）编译步骤：
1. 启动MS Visual Studio 2008;
2. File-New-Projects-Win32 Application，填入Project Name和目录，Next；
3. 在What Kind of Windows application中，选择An empty project;
4. 在File View（左下角）中：
1)选择Source Files，右键-Add files to folder，选09-01client.c；
2)选择Resource Files，右键-Add files to folder，选09-01client.rc
5. Project-Settings，选项目名
1)Linker——输入（左边），附加依赖项，加入文件ws2_32.lib;
2) 常规（左），字符集，选择“未设置”。
3）调试（左），命令参数，填入服务器ip地址
6. 运行，点菜单Server-Start...。如果程序正确连接到linux server上，将在窗口中显示一个数字，数字从1-1024变化。