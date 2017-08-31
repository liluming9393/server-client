#include <stdio.h>
#include <winsock2.h>
#include <process.h>
#include <windows.h>
#include <assert.h>
#pragma comment(lib,"ws2_32.lib")

//参数结构体，用来把多个参数传入线程函数，
struct args{
	SOCKET sClient;             //当前连接的socket套接字
	sockaddr_in remoteAddr;     //当前连接的端口号
};

//每个连接的线程
unsigned int __stdcall createlink(void* arg){
	args ap = *((args*)arg);        //这里将传入的无类型指针转换成args*类型，再取其值所表示的结构体，赋值给ap
	printf("接受到一个连接：%s:%d\r\n", inet_ntoa(ap.remoteAddr.sin_addr), ap.remoteAddr.sin_port);
	char revData[255] = {};
	while (1)
	{
		//接收数据
		int ret = recv(ap.sClient, revData, 255, 0);
		printf("%d:%d\n", ap.remoteAddr.sin_port, ret);
		if (ret <= 0)
		{
			break;
		}
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
			printf("\n");
		}
		//发送数据
		//char * sendData = "服务端：你好，TCP客户端！\n";
		/*char sendData[20];
		sprintf(sendData, "ser: hello! %d!\n", ap.remoteAddr.sin_port);
		send(ap.sClient, sendData, strlen(sendData), 0);*/
	}
	closesocket(ap.sClient);    //结束连接前关闭当前连接的套接字
	return 0;
}


int main(int argc, char* argv[])
{
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}
	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}
	//绑定IP和端口
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(1920);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}
	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}
	//循环接收数据
	printf("等待连接...\n");
	int thrdnum = 0;
	HANDLE handle[5];   //连接线程数组，最大5个连接
	while (1){
		//accept()获得接入连接，返回连接的套接字
		sockaddr_in remoteAddr;
		int nAddrlen = sizeof(remoteAddr);
		SOCKET sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
		}
		args arg;
		arg.sClient = sClient;
		arg.remoteAddr = remoteAddr;
		//为连接创建线程，传入连接套接字，端口号
		handle[thrdnum++] = (HANDLE)_beginthreadex(NULL, 0, createlink, &arg, 0, NULL);
	}
	WaitForMultipleObjects(thrdnum, handle, TRUE, INFINITE);    //等待所有线程都结束
	closesocket(slisten);   //关闭服务端的套接字
	WSACleanup();
	getchar();
	return 0;
}