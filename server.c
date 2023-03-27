#include "chat.h"

struct ONLINE online[MAX_USER_NUM];    //MAX_USER_NUM is 64, declear a struct of online user

//delete the online user who wants to offline
void del_user_online(int index)
{
  int i;
  char buf[128] = {0};

  if(index < 0)
  {
    return;
  }

  online[index].fd = -1;                           //let this user "offline"
  sprintf(buf,"%s offline",online[index].name);    //show that someone offline

  //tell all client that user offline
  for(i = 0;i < MAX_USER_NUM;i ++)
  {
    if(online[i].fd == -1)
    {
      continue;
    }
    //if there is a user online ,we write the data(online[i].fd) to buf
    write(online[i].fd,buf,strlen(buf));

  }
  return;
}

//add user func
int add_user(int sockfd,struct protocol*msg)
{

}

//bordcast-chat func
void broadcast(int index,struct protocol*msg)
{

}

//finding the destination of online user func
int find_dest_user_online(int sockfd,int *index,struct protocol*msg)
{

}

//finding the destination of user func
int find_dest_user(char *name)
{

}

//private-chat func
void private(int index,struct protocol*msg)
{

}

//the list of online user func
void list_online_user(int index)
{

}

//register func
void registe(int sockfd,int *index,struct protocol*msg)
{

}

//login func
void login(int sockfd,int *index,struct protocol*msg)
{

}


//thread func
void *func(void *arg)
{
  int sockfd = (int)arg;
  char buf[64];


  while(1)
  {
    
  }
}


//main func
int main(int argc,char **argv)
{
  int iServerSocket;
  int iRet;
  int i;
  int new_fd;
  pthread_t tid;
  struct sockaddr_in tServerAddr;
  struct sockaddr_in tClientAddr;


  //socket
  iServerSocket = socket(AF_INET,SOCK_STREAM,0);
  if(iServerSocket == -1)
  {
    printf("socket error!\n");
    return -1;
  }

  //bind
  tServerAddr.sin_port = htons(SEVER_PORT);
  tServerAddr.sin_family = AF_INET;
  tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(tServerAddr.sin_zero,0,8);
  iRet = bind(iServerSocket,(const struct sockaddr*)&tServerAddr,sizeof(struct sockaddr));
  if(iRet == -1)
  {
    printf("bind error!\n");
    return -1;
  }

  //listen
  iRet = listen(iServerSocket,BACK_LOG);
  if(iRet == -1)
  {
    printf("listen error!\n");
    return -1;
  }
  for(i = 0;i < 64;i++)
  {
    online[i].fd = -1;
    online[i].flage = -1;
  }

  while(1)
  {
    tClientAddr = sizeof(struct sockaddr);
    new_fd = accept(iServerSocket,(const struct sockaddr*)&tClientAddr,&tClientAddr);
    if(new_fd != -1)
    {
      printf("client:ip:%s   port:%d   \n", inet_ntoa(tClientAddr.sin_addr),tClientAddr.sin_port);
      iRet = pthread_create(&tid,NULL,func,(void *)new_fd);
      if(iRet == -1)
      {
        printf("thread create error!\n");
        return -1;
      }
    }
    
  }
  close(new_fd);          //close the file descriptor which was created by accpet()
  close(iServerSocket);   //close the server socket
}