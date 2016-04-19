/*
 * SynTemporaryDataThread.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */
#include "global.h"

#include <iostream>
#include <list>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>


#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
using namespace std;



void *SynTemporaryDataThread(void *arg)//thread3
{
	cout<<"thread 3 start"<<endl;	cout<<"thread 2 start"<<endl;
	int listen_sockfd;
	int error;
	int maxfd=-1;
	struct sockaddr_in hints;

	memset(&hints,0,sizeof(struct addrinfo));
	hints.sin_family=AF_INET;
	hints.sin_port=htons(LEADER_PORT_SYN);
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
		if(n_ready==-1)
		{
			perror("select error");
		}
		for(int i=0;i<=maxfd && n_ready>0;i++)
		{
			if(FD_ISSET(i,&readfds))
			{
				n_ready--;
				if(i==listen_sockfd)
				{//accept
					struct sockaddr_in client_addr;
#ifdef __CYGWIN__
					int len=sizeof(client_addr);
#else
					unsigned int len=sizeof(client_addr);
#endif
					int client_fd=accept(listen_sockfd,(struct sockaddr*)&client_addr,&len);
					if(client_fd<0)
					{
						perror("accept");
						continue;
					}
					cout<<"incoming fd "<<client_fd<<" from "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;
					error=fcntl(client_fd,F_SETFD,O_NONBLOCK);
					if(error<0)
					{
						perror("fcntl error");
						continue;
					}
					//max connection???
					FD_SET(client_fd,&master);

					pthread_mutex_lock(&fds_mutex);
					follower_fds.insert(i);
					pthread_mutex_unlock(&fds_mutex);

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
						//log
						cout<<"closed fd "<<i<<endl;

						close(i);
						FD_CLR(i,&master);
						pthread_mutex_lock(&fds_mutex);
						follower_fds.erase(i);
						pthread_mutex_unlock(&fds_mutex);
						continue;
					}
					else
					{
						cout<<"error, this port "<<LEADER_PORT_SYN<<" should not recv any thring"<<endl;
					}
				}
			}
		}
	}
	close(listen_sockfd);
	pthread_exit(NULL);

}




