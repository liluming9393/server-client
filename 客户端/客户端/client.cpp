#include <cstdio>
#include <winsock2.h>
#include <ctype.h>
#pragma  comment(lib,"ws2_32.lib")

int communicate(SOCKET sclient)
{
	char send_buffer[10] = { 0 };
	for (int i = 0; i < 10; i++)
	{
		send_buffer[i] = i + '0';
		send(sclient, send_buffer, 10, 0);
		Sleep(1000);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	//初始化套接字
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(1920);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		return 0;
	}
	//连接服务器
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		return 0;
	}
	//进行通讯函数
	communicate(sclient);
	//关闭套接字，结束客户端
	closesocket(sclient);
	WSACleanup();
	printf("END\n");
	getchar();
	return 0;
}