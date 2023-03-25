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
#define BACK_LOG 10
#define RECVBUFFER_SIZE 2048

//线程调用的函数
static void *thread_func(void *arg)
{
  int RecvLen;    //recv函数接收到的数据
  int SendLen;    //send函数传出去的数据
  int new_fd;     //传入的参数应为accept后的文件描述符
  char recvbuf[RECVBUFFER_SIZE];
  new_fd =(int)arg;
  



  while(1) {
    RecvLen = recv(new_fd ,recvbuf ,RECVBUFFER_SIZE, 0);
    if(RecvLen <= 0)
    {
      printf("client error!\n");
      close(new_fd);
      return -1;
    }
    else
    {
      recvbuf[RecvLen] = '\0';
      printf("Get Msg: %s\n", recvbuf);
    }
    SendLen = send(new_fd, recvbuf, strlen(recvbuf) ,0);
    if(SendLen == -1)
    {
      printf("write error!\n");
      return -1;
    }
  }
}



int main(int argc,char **argv)
{
  int iServerSocket;
  int iClientSocket;
  int iRet;
  struct sockaddr_in tServerAddr;
  struct sockaddr_in tClientAddr;
  int iAddrLen;
  pthread_t tid;    //定义线程号类型

  //输出参数不为2个，打印用法
  if(argc != 2)
  {
    printf("Usage:%s portnumber\n",argv[0]);
    return -1;
  }

  //输入的端口不对，打印用法
  if(atoi(argv[1]) < 0)
  {
    printf("Usage:%s portnumber\n",argv[0]);
    return -1;
  }


  //socket
  iServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  

  //bind
  tServerAddr.sin_family = AF_INET;
  tServerAddr.sin_port = htons(SERVER_PORT);
  tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(tServerAddr.sin_zero,0,8);
  iRet = bind(iServerSocket,(const struct sockaddr*)&tServerAddr,sizeof(struct sockaddr));
  if(iRet == -1)
  {
    printf("bind error\n");
    return -1;
  }


  //listen
  iRet = listen(iServerSocket,BACK_LOG);
  if(iRet == -1)
  {
    printf("listen error\n");
    return -1;
  }

  //accept
  while(1)  //循环接收连接
  { 
    iAddrLen = sizeof(struct sockaddr);
    iClientSocket = accept(iServerSocket,(struct sockaddr*)&tClientAddr,&iAddrLen);
    
    if(iClientSocket != -1)
    {
      iRet = pthread_create(&tid, NULL, thread_func, (void *)iClientSocket);
      if(iRet == -1)
      {
        printf("thread create error!\n");
        return -1;
      }
    }
    else
    {
      printf("accept error!\n");
      return -1;
    }
  }
  exit(0);
}
