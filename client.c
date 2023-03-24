#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define SERVER_PORT 8888
#define RECVBUFSIZE 1024

void *func(void *arg)
{
	int new_fd;
	int iRecvLen;
	char buf[RECVBUFSIZE];

	new_fd = (int)arg;
	while(1)
	{
		iRecvLen = recv(new_fd,buf,RECVBUFSIZE,0);
		if(iRecvLen == -1)
		{
			printf("Read error\n");
			return -1;
		}
		buf[iRecvLen] = '\0';
		printf("successfully received:%s\n",buf);
	}
}


int main(int argc,char **argv)
{
	int iClientSocket;
	char buf[RECVBUFSIZE];
	struct sockaddr_in tServerSocket;
	int iRet;
	int iSendLen;
	pthread_t tid;



	//检测参数个数
	if(argc != 3)
	{
		printf("Usage:%s hostname portnumber\a\n",arg[0]);
		return -1;
	}

	//socket
	iClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(iClientSocket == -1)
	{
		printf("socket error!\n");
		return -1;
	}

	//connect
	//tClientSocket.sin_addr.s_addr = INADDR_ANY;
	tServerSocket.sin_family = AF_INET;
	tServerSocket.sin_port = htons(SERVER_PORT);
	if (0 == inet_aton(argv[1], &tServerSocket.sin_addr))
 	{
		printf("invalid server_ip\n");
		return -1;
	}
	memset(tServerSocket.sin_zero, 0, 8);
	iRet = connect(iClientSocketm, (const struct sockaddr*)tServerSocket,sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("connect error!\n");
		return -1;
	}
	
	//pthread_create创建线程
	iRet = pthread_create(&tid,NULL,func,(void *)iClientSocket);
	if(iRet < 0)
	{
		printf("pthread create error\n");
		return -1;
	}

	//循环发送信息
	while(1)
	{
		#if 1
		printf("input message:");
		fgets(buf, RECVBUFSIZE, stdin);
		iSendLen = send(iClientSocket,buf,strlen(buf),0);
		if(iSendLen == -1)
		{
			printf("write error!\n");
			return -1
		}
		#endif
	}
	close(iClientSocket);
	exit(0);
}