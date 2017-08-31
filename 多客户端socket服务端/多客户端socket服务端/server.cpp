#include <stdio.h>
#include <winsock2.h>
#include <process.h>
#include <windows.h>
#include <assert.h>
#pragma comment(lib,"ws2_32.lib")

//�����ṹ�壬�����Ѷ�����������̺߳�����
struct args{
	SOCKET sClient;             //��ǰ���ӵ�socket�׽���
	sockaddr_in remoteAddr;     //��ǰ���ӵĶ˿ں�
};

//ÿ�����ӵ��߳�
unsigned int __stdcall createlink(void* arg){
	args ap = *((args*)arg);        //���ｫ�����������ָ��ת����args*���ͣ���ȡ��ֵ����ʾ�Ľṹ�壬��ֵ��ap
	printf("���ܵ�һ�����ӣ�%s:%d\r\n", inet_ntoa(ap.remoteAddr.sin_addr), ap.remoteAddr.sin_port);
	char revData[255] = {};
	while (1)
	{
		//��������
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
		//��������
		//char * sendData = "����ˣ���ã�TCP�ͻ��ˣ�\n";
		/*char sendData[20];
		sprintf(sendData, "ser: hello! %d!\n", ap.remoteAddr.sin_port);
		send(ap.sClient, sendData, strlen(sendData), 0);*/
	}
	closesocket(ap.sClient);    //��������ǰ�رյ�ǰ���ӵ��׽���
	return 0;
}


int main(int argc, char* argv[])
{
	//��ʼ��WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}
	//�����׽���
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}
	//��IP�Ͷ˿�
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(1920);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}
	//��ʼ����
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}
	//ѭ����������
	printf("�ȴ�����...\n");
	int thrdnum = 0;
	HANDLE handle[5];   //�����߳����飬���5������
	while (1){
		//accept()��ý������ӣ��������ӵ��׽���
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
		//Ϊ���Ӵ����̣߳����������׽��֣��˿ں�
		handle[thrdnum++] = (HANDLE)_beginthreadex(NULL, 0, createlink, &arg, 0, NULL);
	}
	WaitForMultipleObjects(thrdnum, handle, TRUE, INFINITE);    //�ȴ������̶߳�����
	closesocket(slisten);   //�رշ���˵��׽���
	WSACleanup();
	getchar();
	return 0;
}