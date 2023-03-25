#include "chat.h"

//删除在线用户，即用户下线
void del_user_online(int index)
{

}

//添加用户
int add_user(int sockfd,struct protocol*msg)
{

}

//公聊
void broadcast(int index,struct protocol*msg)
{

}

//查找在线用户的位置
int find_dest_user_online(int sockfd,int *index,struct protocol*msg)
{

}

//查找用户位置
int find_dest_user(char *name)
{

}

//私聊
void private(int index,struct protocol*msg)
{

}

//在线用户表
void list_online_user(int index)
{

}

//注册
void registe(int sockfd,int *index,struct protocol*msg)
{

}

//登录
void login(int sockfd,int *index,struct protocol*msg)
{

}


//线程函数
void *func(void *arg)
{

}


//主函数
int main(int argc,char **argv)
{
  socket();
  bind();
  listen();

  while(1)
  {
    accept();
    pthread_create();
  }
  close();   //关闭accept函数产生的文件描述符
  close();   //关闭套接字
}