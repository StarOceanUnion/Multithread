#include "chat.h"

int iClientSocket;
int addrlen;
struct sockaddr_in tServerSocket;


pthread_t pid;

int login_f = -1;


//broadcast chat func
void broadcast(int fd)
{
	struct protocol msg;

	msg.cmd = BROADCAST;
	printf("say:\n#");
	scanf("%s",msg.data);
	write(fd,&msg,sizof(msg));
}


//private chat func
void private(int fd)
{
	struct protocol msg;

	printf("input the name that you want to talk:\n#");
	scanf("%s",msg.name);

	printf("say:\n#");
	scanf("%s",msg.data);
	write(fd,&msg,sizof(msg));

}

//online user list func
void list_online_user(sockfd)
{
	struct protocol msg;

	msg.cmd = ONLINEUSER;

	write(sockfd,&msg,sizeof(msg));
	getchar();
	getchar();
}

//register func
int registe(int fd)
{
	struct protocol msg,msg_back;

	msg.cmd = REGISTE;
	printf("input your name:");
	scanf("%s",msg.name);
	printf("input your password:");
	scanf("%s",msg.data);

	write(sockfd,&msg,sizeof(msg));
	read(sockfd,&msg_back,sizeof(msg_back));
	if(msg_back.state != OP_OK)
	{
		printf("Name had exist,try other names!\n");
		getchar();
		getchar();
		return -1;
	}
	else
	{
		printf("Regist success!\n");
		getchar();
		getchar();
		return 0;
	}

}

//login func
int login(int fd)
{
	struct protocol msg,msg_back;
	msg.cmd = LOGIN;
	printf("input your name:");
	scanf("%s",msg.name);
	printf("input your password:");
	scanf("%s",msg.data);

	write(sockfd,&msg,sizeof(msg));
	read(sockfd,&msg_back,sizeof(msg_back));
	if(msg_back.state != OP_OK)
	{
		printf("name had exist,try other names!\n");
		getchar();
		getchar();
		login_f = -1;
		return NAME_PWD_NMATCH;
	}
	else
	{
		printf("Login success!!\n");
		getchar();
		getchar();
		login_f = 1;
		return OP_OK;
	}
}

//log out func
int logout(int fd)
{
	close(fd);
	login_f = -1;
}

//multithread func
void *func(void *arg)
{
	int len;
	char buf[128] = {0};
	struct protocol *msg;

	while(1)
	{
		if(login_f != 1)
		{
			continue;
		}
		memset(buf,0,sizeof(buf));
		len = read(iClientSocket,buf,sizeof(buf));
		if(len <= 0)
		{
			close(iClientSocket);
			return;
		}
		msg = (struct protocol *)buf;
		//show online user
		if((msg->state == ONLINEUSER_OK) && (msg->cmd == ONLINEUSER))
		{
			printf("%s\t",msg->name);
			continue;
		}
		if((msg->state == ONLINEUSER_OVER) && (msg->cmd == ONLINEUSER))
		{
			printf("\n");
			continue;
		}
		buf[len] = '\0';

		printf("%s\n",buf);
	}
}

//main func
int main(int argc,char **argv)
{
	int sel;
	int iRet;
	int ret;
	int min_sel,max_sel;
	struct protocol msg;


	if(argc != 3)
	{
		printf("Usage:%s hostname portnumber\a\n",argv[0]);
		return -1;
	}

	//socket
	iClientSocket = socket(AF_INET,SOCK_STREAM,0);
	if(iClientSocket == -1)
	{
		printf("socket error!\n");
		return -1;
	}

	//connect
	tServerSocket.sin_addr.s_addr = inet_addr(argv[1]);
	tServerSocket.sin_family = AF_INET;
	tServerSocket.sin_port = htons(SERVER_PORT);

	if (0 == inet_aton(argv[1], &tServerSocket.sin_addr))
 	{
		printf("invalid server_ip\n");
		return -1;
	}
	memset(tServerSocket.sin_zero, 0, 8);

	addrlen = sizeof(struct sockaddr);
	iRet = connect(iClientSocket, (const struct sockaddr*)&tServerSocket,addrlen);
	if (-1 == iRet)
	{
		printf("connect error!\n");
		return -1;
	}

	//create thread
	pthread_create(&pid,NULL,func,NULL);
	while(1)
	{
		/*
			there is a menu to choose what we want to be in
		*/
		system("clear");          //clear the menu
    	if(login_f == -1)         //not login menu
    	{
    	  printf("\t 1 注册 \n");
    	  printf("\t 2 登录 \n");
    	}
    	else if(login_f == 1)     //login menu
    	{
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
    	}
    	else if(login_f == -1)
    	{
    	  min_sel = 1;
    	  max_sel = 2;
    	}
    	if(sel<min_sel || sel > max_sel)      //judge the command
    	{
    	  printf("Valid choice ,try again\n");
    	  continue;
    	}

		//"switch" to judge what kinds of func do the programme in
		switch(sel)
		{
		  case 1:
		  	registe(iClientSocket);
		  	break;
		  case 2:
		  	ret = login(iClientSocket);
		  	break;
		  case 3:
		  	broadcast(iClientSocket);
		  	break;
		  case 4:
		  	private(iClientSocket);
		  	break;
		  case 5:
		  	logout(iClientSocket);
		  	break;
		  case 0:
		  	logout(iClientSocket);
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
