#include "chat.h"




//多线程函数
void *func(void *arg)
{

}


//公聊
void broadcast(int fd)
{

}


//私聊
void private(int fd)
{

}

//在线列表
void list_online_user(sockfd)
{

}

//注册
int registe(int fd)
{

}

//登录
int login(int fd)
{

}

//登出
int logout(int fd)
{

}

//主函数
int main(int argc,char **argv)
{
	socket();
	connect();
	pthread_create();
	while(1)
	{
		/*
		此处用来做menu，利用flag来判断是哪个界面，区分登录和功能界面
		*/

		//用来判断进入什么函数，比如登录注册等
		switch(click)
		{

		}
	}
}
