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
  int i;
  int index = -1;
  char buf[128] = {0};

  for(i=0;i<64;i++)
  {
    if(online[i].flage == -1)
    {
      online[i].flage = 1;                //let this user "online"
      strcpy(online[i].name,msg->name);   //*msg.name
      strcpy(online[i].data,msg->data);
      printf("regist %s to %d \n",msg->name,i);
      index = i;
      return index;
    }
  }
  return index;
}

//bordcast-chat func
void broadcast(int index,struct protocol*msg)
{
  int i;
  char buf[128] = {0};

  sprintf(buf,"%s says: %s \n",online[i].name,msg->data);

  for(i=0;i<MAX_USER_NUM;i++)
  {
    if((online[i].fd == -1)||(i == index))
    {
      continue;
    }
    write(online[i].fd,buf,strlen(buf));
  }

}

//finding the destination of online user func
int find_dest_user_online(int sockfd,int *index,struct protocol*msg)
{
  int i;
  
  for(i=0;i<MAX_USER_NUM;i++)
  {
    //this pos not use
    if(online[i].flage == -1)
    {
      continue;
    }

    if((strcmp(msg->name,online[i].name) == 0)&&(strcmp(msg->data,online[i].passwd) == 0))
    {
      if(online[i].fd == -1)
      {
        online[i].fd = sockfd;
        *index = i;
        return OP_OK;
      }
      else
      {
        //user has logged
        printf("%s had login\n",online[i].name);
        return USER_LOGED;
      }
    }
  }
  return NAME_PWD_NMATCH;
}

//finding the destination of user func
int find_dest_user(char *name)
{
  int i;

  for(i=0;i<MAX_USER_NUM;i++)
  {
    if(online[i].flage == -1)  //not registed
    {
      continue;
    }

    if(strcmp(name,online[i].name) == 0)
    {
      return i;
    }
  }
  
  return -1;
}

//private-chat func
void private(int index,struct protocol*msg)
{
  int dest_index;
  char buf[128];
  //find that user
  dest_index = find_dest_user(msg->name);
  if(dest_index == -1)
  {
    sprintf(buf,"there is no user :%s \n",msg->name);
    write(online[i].fd,buf,strlen(buf));
    return;
  }
  else
  {
    sprintf(buf,"%s says to %s : %s \n",online[index].name,online[dest_index].name,msg->data);
    write(online[i].fd,buf,strlen(buf));
    return;
  }
}

//the list of online user func
void list_online_user(int index)
{
  int i;
  struct protocol msg;

  for(i=0;i<MAX_USER_NUM;i++)
  {
    if(online[i].fd == -1)
    {
      continue;
    }
    memset(&msg,0,sizeof(msg));
    msg.cmd = ONLINEUSER;
    msg.state = ONLINEUSER_OK;
    strcpy(msg.name,online[i].name);
    printf("list online[i] = %s\n",online[i].name);
    write(online[i].fd,&msg,sizeof(msg));
  }
  msg.cmd = ONLINEUSER;
  msg.state = ONLINEUSER_OVER;
  write(online[index].fd,&msg,sizeof(msg));
}

//register func
void registe(int sockfd,int *index,struct protocol*msg)
{
  int dest_index;
  char buf[128];
  struct protocol msg_back;
  msg_back.cmd = REGISTE;

  //find that user
  dest_index = find_dest_user(msg->name);     //*msg.name

  if(dest_index == -1)   //no name of this user registed
  {
    *index = add_user(sockfd,msg);

    online[*index].flage = 1;
    msg_back.state = OP_OK;

    printf("user %s regist success!\n",msg->name);
    write(sockfd,&msg_back,sizeof(msg_back));

    return;
  }
  else
  {
    msg_back.state = NAME_EXIST;
    printf("user %s exist!\n",msg->name);

    wirte(sockfd,&msg_back,sizeof(msg_back));
    return;
  }
}

//login func
void login(int sockfd,int *index,struct protocol*msg)
{
  int i;
  int ret;
  char buf[128];
  struct protocol msg_back;

  msg_back.state = LOGIN;     //order is LOGIN

  //let's find that user
  ret = find_dest_user_online(sockfd,index,msg);

  if(ret != OP_OK)
  {
    msg_back.state = ret;
    strcpy(buf,"there is no this user\n");
    printf("user %s login fail!\n",msg->name);
    write(sockfd,&msg_back,sizeof(msg_back));
    return;
  }
  else
  {
    msg_back.state = OP_OK;
    strcpy(msg_back.data,"login success!\n");
    printf("user %s login success! index = %d \n",msg->name,*index);
    write(online[*index].fd,&msg_back,sizeof(msg_back));
  }

  //tell all clients that the user online
  sprintf(buf,"%s online\n",online[*index].name);

  for(i=0;i<MAX_USER_NUM;i++)
  {
    if(online[i].fd != -1)
    {
      write(online[i].fd,buf,strlen(buf));
    }
  }
}


//thread func
void *func(void *arg)
{
  int sockfd = (int)arg;
  char buf[64];
  int index = -1;
  int len;               //the number of returned byte
  struct protocol msg;   //the message of user,like command, status, data, and name

  //when the client connectted,we have to put in a name first
  //there we don't consider the errors about data's type, name repetiton and overcrowding

  //chatting
  while(1)
  {
    len = read(sockfd,&msg,sizeof(msg));
    if(len <= 0)     //0 means that is the end of file,and -1 means that func error 
    {
      //logout
      printf("%s offline\n",online[index].name);
      //delete user from the online list
      del_user_online(index);
      close(sockfd);
      return;
    }
    switch(msg.cmd)   //scan the command that users add
    {
      /*there is a var called msg_back,
       its function is to save the first var 
       that we can use this switch again*/


      case REGISTE:
        registe(sockfd,&index,&msg);
        break;
      case LOGIN:
        login(sockfd,&index,&msg);
        break;
      case BROADCAST:
        broadcast(index,&msg);
        break;
      case PRIVATE:
        private(index,&msg);
        break;
      case ONLINEUSER:
        list_online_user(index);
        break;
      default:
        break;
    }
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


  if(argc != 3)
  {
    printf("Usage:%s hostname portnumber\a\n",argv[0]);
    return -1;
  }
  
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