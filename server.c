#include "chat.h"

struct ONLINE online[MAX_USER_NUM];


void del_user_online(int index)
{
  int i;
  char buf[128]={0};

  if(index <0)
  {
    return;
  }
  online[index].fd = -1;
  sprintf(buf,"%s offline\n",online[index].name);
  //通知所有客户端，某个用户下线了
  for(i=0;i<MAX_USER_NUM;i++)
  {
    if(online[i].fd == -1)
    {
      continue;
    }
    write(online[i].fd,buf,strlen(buf));  
  }  
  
  
  return;
}
int add_user(int sockfd,struct protocol*msg)
{
  int i,index = -1;
  char buf[128]={0};
  
  for(i=0;i<64;i++)//添加到在线用户列表
  {
    if(online[i].flage == -1)
    {
      online[i].flage= 1;
      strcpy(online[i].name,msg->name);
      strcpy(online[i].passwd,msg->data);
      printf("regist %s to %d \n",msg->name,i);
      index = i;
      return index;
    }    
  }
  return index;
}
void broadcast(int index,struct protocol*msg)
{
  int i;
  char buf[128]={0};
  
  sprintf(buf,"%s say:%s\n",online[index].name ,msg->data);
  
  for(i=0;i<MAX_USER_NUM;i++)
  {// jump offline and sender self
    if((online[i].fd == -1)||(i == index))
    {
      continue;
    }
    write(online[i].fd,buf,strlen(buf));  
  }  
}
int find_dest_user_online(int sockfd,int *index,struct protocol*msg)
{
  int i;
  
  for(i=0;i<MAX_USER_NUM;i++)
  {
  //this pos not use
    if(online[i].flage== -1)
    {
      continue;      
    }
    
    if((strcmp(msg->name,online[i].name)==0)&&(strcmp(msg->data,online[i].passwd)==0))
    {
      if(online[i].fd == -1)
      {
        online[i].fd = sockfd;
        *index = i ;
        return OP_OK;
      }else{
        //user had loged
        printf("%s had login\n",online[i].name);
        return USER_LOGED;
      }
          
    }
  }
  return NAME_PWD_NMATCH;
}
int find_dest_user(char *name)
{
  int i;
  
  for(i=0;i<MAX_USER_NUM;i++)
  {
  
    if(online[i].flage == -1)
    {
      continue;      
    }
    
    if(strcmp(name,online[i].name)==0)
    {
      return i;      
    }
  }
  return -1;
}

void private(int index,struct protocol*msg)
{
  int dest_index;
  char buf[128];
  //找到那个人
  dest_index = find_dest_user(msg->name);
  
  if(dest_index == -1)
  {  
    sprintf(buf,"there is no  user :%s \n",msg->name);
    write(online[index].fd,buf,strlen(buf));
    return;
  }else{
    sprintf(buf,"%s say to %s:%s\n",online[index].name ,online[dest_index].name,msg->data);
    write(online[dest_index].fd,buf,strlen(buf));
    return;
  }
  
  
}
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
    
    printf("list online[i].name =%s \n",online[i].name);
    
    write(online[index].fd ,&msg,sizeof(msg));
  }
  msg.cmd = ONLINEUSER;
  msg.state = ONLINEUSER_OVER;
    
  write(online[index].fd ,&msg,sizeof(msg));
}

void registe(int sockfd,int *index,struct protocol*msg)
{
  int dest_index;
  char buf[128];
  struct protocol msg_back;

  msg_back.cmd = REGISTE;  
  //找到那个人
  dest_index = find_dest_user(msg->name);

  if(dest_index == -1)
  {  // this user can registe
    *index = add_user(sockfd,msg);
    
    online[*index].flage = 1;
    msg_back.state = OP_OK;
    
    printf("user %s regist success!\n",msg->name);
    write(sockfd,&msg_back,sizeof(msg_back));
    
    return;
  }else{
    msg_back.state = NAME_EXIST;
    printf("user %s exist!\n",msg->name);

    write(sockfd,&msg_back,sizeof(msg_back));
    return;
  }  
}
void login(int sockfd,int *index,struct protocol*msg)
{
  int i;
  int ret;
  char buf[128];
  struct protocol msg_back;

  msg_back.cmd = LOGIN;  
  
  //找到那个人
  ret = find_dest_user_online(sockfd,index,msg);
  
  if(ret != OP_OK)
  {
    msg_back.state = ret;
    strcpy(buf,"there is no this user\n");
    printf("user %s login fail!\n",msg->name);
    
    write(sockfd,&msg_back,sizeof(msg_back));
    return;
  }else{
    msg_back.state = OP_OK;
    strcpy(msg_back.data,"login success\n");
    printf("user %s login success!index =%d \n",msg->name,*index);
    write(online[*index].fd,&msg_back,sizeof(msg_back));
  }
  //通知所有客户端，某个用户上线了
  sprintf(buf,"%s online\n",online[*index].name);
  
  for(i=0;i<MAX_USER_NUM;i++)
  {
    if(online[i].fd != -1)
    {
      write(online[i].fd,buf,strlen(buf));
    }      
  }
  
}
void *func(void *arg)
{
  int sockfd = *((int*)arg);
  char buf[64];
  int len;
  int index = -1;//该用户在在线用户列表的下标
  struct protocol msg;
  
  free(arg);  
  //客户端连接后 必须先输入一个名字,不考虑数据类型出错，以及重名问题,也不考虑人数满了的情况
//  read(sockfd,&msg,sizeof(msg));
  
//  index = add_user_online(sockfd,&msg);

  //进入聊天了
  while(1)
  {
    len = read(sockfd,&msg,sizeof(msg));
    if(len<=0)
    {//下线
      printf("%s offline\n",online[index].name);
      //从在线列表中删除
      del_user_online(index);
      close(sockfd);
      return;
    }
    
    switch(msg.cmd)
    {
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

int main(int argc, char **argv)
{
  int lsfd,newfd;
  int addrLen,cliaddrlen;
  struct sockaddr_in   my_addr; 
  struct sockaddr_in   cli_adr;  
  char buf[64]="xuezhiqian fuhele\n";
  pthread_t pid;
  int *arg;
  int i;
  int portnumber;
  
  if(argc<2)
  {
    printf("cmd: %s  portnumber\n",argv[0]);
    return;
  }
/*判断端口是否正确*/
  if((portnumber=atoi(argv[1]))<0)
  {
    fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
    exit(1);
  }  
  lsfd = socket(PF_INET,SOCK_STREAM,0);  
  if(lsfd<0)
  {
    perror("socket() fail\n");
    return;
  }
  bzero(&my_addr,sizeof(struct sockaddr_in));
  my_addr.sin_family =  PF_INET;  
  my_addr.sin_port   =  htons(portnumber);
  my_addr.sin_addr.s_addr   =  htonl(INADDR_ANY);
  addrLen = sizeof(struct sockaddr_in);
  
  if(bind(lsfd,(struct sockaddr* )&my_addr ,addrLen)<0)
  {
    perror("bind() fail\n");
    return;    
  }
  
  listen(lsfd,5);
  cliaddrlen = sizeof(struct sockaddr_in);
  
  for(i=0;i<64;i++)
  {
    online[i].fd = -1;
    online[i].flage= -1;
  }
  while(1)
  {
    newfd = accept(lsfd,(struct sockaddr *)&cli_adr,&cliaddrlen);
    printf("client:ip:%s   port:%d  \n",
      inet_ntoa(cli_adr.sin_addr),cli_adr.sin_port);
        
    arg = malloc(sizeof(int));
    *arg = newfd;//必须搞清楚为什么要申请内存
    
          pthread_create(&pid,NULL,func, (void*)arg);  
  }
  close(newfd);
  close(lsfd);
}