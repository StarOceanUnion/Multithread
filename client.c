#include "chat.h"

int sockfd;
int addrlen;
struct sockaddr_in   server_addr; 

pthread_t pid;

int login_f =  -1;
  
void *func(void *arg)
{
  int len;
  char buf[128]={0};
  struct protocol *msg;
  
  while(1)
  {
    if(login_f != 1)
    {
      continue;
    }  
    memset(buf,0,sizeof(buf));
    len = read(sockfd,buf,sizeof(buf));
    if(len<=0)
    {
      close(sockfd);
      return;
    }
    msg = (struct protocol *)buf;
    //  this is show online user,
    if((msg->state == ONLINEUSER_OK)&&(msg->cmd == ONLINEUSER))
    {
      printf("%s\t",msg->name);
      continue;
    }
    if((msg->state == ONLINEUSER_OVER)&&(msg->cmd == ONLINEUSER))
    {
      printf("\n");
      continue;
    }
    
    buf[len]='\0';
    
    printf("%s\n",buf);    
  }  
}
void broadcast(int fd)
{
  struct protocol msg;
  
  msg.cmd = BROADCAST;
  printf("say:\n#");
  
  scanf("%s",msg.data);
  
  write(fd,&msg,sizeof(msg));
}
void private(int fd)
{
  struct protocol msg;
  
  msg.cmd = PRIVATE;
  
  printf("input name you want to talk:\n#");  
  scanf("%s",msg.name);
  
  printf("say:\n#");  
  scanf("%s",msg.data);

  write(fd,&msg,sizeof(msg));
}
void list_online_user(sockfd)
{
  struct protocol msg;
  
  msg.cmd = ONLINEUSER;
  
  write(sockfd,&msg,sizeof(msg));
  getchar();
  getchar();
  /*
  while(1)
  {
    read(sockfd ,&msg,sizeof(msg));
    if(msg.state == ONLINEUSER_OK)
    {
      printf("%s\t",msg.name);
    }else{
      break;
    }
  }*/
  
}
int  registe(int fd)
{
  struct protocol msg,msgback;

  msg.cmd = REGISTE;  
  printf("input your name\n");  
  scanf("%s",msg.name);
  printf("input your passwd\n");  
  scanf("%s",msg.data);

  write(sockfd,&msg,sizeof(msg));
  read(sockfd,&msgback,sizeof(msgback));
  if(msgback.state != OP_OK)
  {
    printf("Name had exist,try again!\n");  
    getchar();
    getchar();
    return -1;
  }else{
    printf("Regist success!\n");
    getchar();
    getchar();
    return 0  ;
  }
}
int login(int fd)
{
  struct protocol msg,msgback;

  msg.cmd = LOGIN;  
  printf("input your name\n");  
  scanf("%s",msg.name);
  printf("input your passwd\n");  
  scanf("%s",msg.data);

  write(sockfd,&msg,sizeof(msg));
  read(sockfd,&msgback,sizeof(msgback));
  if(msgback.state != OP_OK)
  {
    printf("Name had exist,try again!\n");
    getchar();
    getchar();
    login_f = -1;
    return NAME_PWD_NMATCH;
  }else{
    printf("Login success!\n");
    getchar();
    getchar();
    login_f = 1;
    return OP_OK  ;
  }
}
int logout(int fd)
{
  close(fd);
  login_f = -1;
}
int main(int argc, char **argv)
{
  int    sel;
  int ret; 
  int min_sel,max_sel;
  int portnumber;
  
  struct protocol msg;
  
  
  if(argc<3)
  {
    printf("cmd: %s ip portnumber\n",argv[0]);
    return;
  }
  //argv2 存放的是端口号 ，读取该端口，转换成整型变量
  if((portnumber=atoi(argv[2]))<0)
  {
    fprintf(stderr,"Usage:%s hostname portnumber\a\n",argv[0]);
    exit(1);
  }
  sockfd = socket(PF_INET,SOCK_STREAM,0);  
  if(sockfd<0)
  {
    perror("socket() fail\n");
    return;
  }
  
  server_addr.sin_family =  PF_INET;  
  server_addr.sin_port   =  htons(portnumber);
  server_addr.sin_addr.s_addr   =  inet_addr(argv[1]);
  
  addrlen = sizeof(struct sockaddr_in);
  
  connect(sockfd,(struct sockaddr* )&server_addr,addrlen);
  pthread_create(&pid, NULL,func, NULL);    
  while(1)
  {
    //getchar();
    system("clear");
    if(login_f == -1){
      printf("\t 1 注册 \n");
      printf("\t 2 登录 \n");
    }else if(login_f == 1){
      printf("\t 3 公聊\n");
      printf("\t 4 私聊\n");
      printf("\t 5 在线列表\n");            
    }  
    printf("\t 0 退出\n");
    
    fflush(stdin);
    scanf("%d",&sel);
    if(sel == 0)
    {
      break;
    }
    if(login_f == 1)
    {
      min_sel = 3;
      max_sel = 5;
    }else if(login_f == -1){
      min_sel = 1;
      max_sel = 2;
    }
    
    if(sel<min_sel || sel > max_sel)
    {
      printf("Valid choice ,try again\n");
      continue;
    }
    switch(sel)
    {
      case 1:
        registe(sockfd);
        break;
      case 2:
        ret = login(sockfd);
        break;
      case 3:
        broadcast(sockfd);
        break;
      case 4:
        private(sockfd);
        break;
      case 5:
        list_online_user(sockfd);
        break;
      case 0:
        logout(sockfd);
        break;
      default:
        break;
    }
    if(sel == 0)
    {
      exit(0);
    }
  }
  
  

}
