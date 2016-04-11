//============================================================================
// Name        : consensus_client.cpp
// Author      : mayue
// Version     :
// Copyright   :
// Description : homework of mayue @ sjtu
//============================================================================

#include <iostream>
#include <string>
#include <list>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Message.h"

using namespace std;

const int LEADER_PORT_CALL	= 8101;
const int LEADER_PORT_SYN	= 8102;
const int LEADER_PORT_INIT	= 8103;
const char LEADER_IP[]	= "127.0.0.1";
const int SERVER_PORT 	= 9101;
const char SERVER_IP[] 	= "127.0.0.1";
const int MAX_PENDING_CLIENT =100;//listen


void ProcessClient()
{
	int listen_sockfd;
	int error;
	int maxfd=-1;
	struct sockaddr_in hints;

	memset(&hints,0,sizeof(struct addrinfo));
	hints.sin_family=AF_INET;
	hints.sin_port=htons(SERVER_PORT);
	hints.sin_addr.s_addr=INADDR_ANY;

	listen_sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_sockfd<0)
	{
		perror("socket create error");
	}
	error=bind(listen_sockfd,(struct sockaddr*)&hints,sizeof(hints));
	if(error<0)
	{
		perror("bind error");
	}
	error=listen(listen_sockfd,MAX_PENDING_CLIENT);
	if(error<0)
	{
		perror("listen error");
	}
	error=fcntl(listen_sockfd,F_SETFD,O_NONBLOCK);
	if(error<0)
	{
		perror("fcntl error");
	}



//	while(1)
//	{
//		struct sockaddr_in client_addr;
//		int len=sizeof(client_addr);
//		int client1=accept(listen_sockfd,(struct sockaddr*)&client_addr,&len);
//		if(client1<0)
//		{
//			perror("accept");
//		}
//		cout<<"incoming fd"<<client1<<" from "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;
//		//write(client1,"hello\n",6);
//		send(client1,"hello\n",6,0);
//		close(client1);
//	}



	static char buffer[128][256];
	static int buff_len[128];
	fd_set master,readfds;
	FD_ZERO(&master);
	FD_ZERO(&readfds);
	FD_SET(listen_sockfd,&master);
	maxfd=listen_sockfd;
	while(1)
	{
		memcpy(&readfds,&master,sizeof(master));
		int n_ready=select(maxfd+1,&readfds,NULL,NULL,NULL);
		if(error==-1)
		{
			perror("select error");
		}
		for(int i=0;i<maxfd && n_ready>0;i++)
		{
			if(FD_ISSET(i,&readfds))
			{
				n_ready--;
				if(i==listen_sockfd)
				{//accept
					struct sockaddr_in client_addr;
					unsigned int len=sizeof(client_addr);
					int client_fd=accept(listen_sockfd,(struct sockaddr*)&client_addr,&len);
					if(client_fd<0)
					{
						perror("accept");
						continue;
					}
					cout<<"incoming fd"<<client_fd<<" from "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;
					error=fcntl(client_fd,F_SETFD,O_NONBLOCK);
					if(error<0)
					{
						perror("fcntl error");
						continue;
					}
					//max connection???
					FD_SET(client_fd,&master);
					if(maxfd<client_fd)
					{
						maxfd=client_fd;
					}
				}
				else
				{//recv
					int len=recv(i,buffer[i]+buff_len[i],sizeof(buffer[0])-buff_len[i],0);
					if(len<=0)
					{
						close(i);
						FD_CLR(i,&master);
						continue;
					}
					else
					{
						static char buffer[1024];
						int len=recv(i,buffer,sizeof(buffer),0);
						buffer[len]=0;
						cout<<buffer<<endl;
						send(i,"ok\n",3,0);
//						buff_len[i]+=len;
//						if(buff_len[i]>=(int)sizeof(int))
//						{
//							int package_len;
//							memcpy(&package_len,buffer[i],sizeof(int));
//							if(buff_len[i]>=package_len)
//							{
//								Message m;
//								Deserialize(buffer[i],m);
//								int left=buff_len[i]-package_len;
//								for(int ii=0;ii<left;ii++)
//								{
//									buff_len[ii]=buff_len[ii+package_len];
//								}
//								buff_len[i]=left;
//
//								//log
//								cout<<"recv "<<m.total_length<<endl;
//								cout<<"operate"<<m.operate<<endl;
//								cout<<"client_id "<<m.client_id<<endl;
//								cout<<"key"<<m.lock_key<<endl;
//
//								int ret=0;//ok
//								send(i,&ret,sizeof(int),0);
//							}
//						}
					}
				}
			}
		}
	}


	close(listen_sockfd);

}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	ProcessClient();


	return 0;
}

