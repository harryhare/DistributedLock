/*
 * ProcessCmdThread.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */

#include "global.h"
#include "Message.h"
#include "IO.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;


void *ProcessCmdThread(void *arg)//thread 1
{
	cout<<"thread 1 start"<<endl;
	int listen_sockfd;
	int error;
	int maxfd=-1;
	struct sockaddr_in hints;

	memset(&hints,0,sizeof(struct addrinfo));
	hints.sin_family=AF_INET;
	hints.sin_port=htons(LEADER_PORT_CMD);
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
						continue;
					}
					else
					{
						buff_len[i]+=len;
						while(buff_len[i]>=(int)sizeof(int))
						{
							int package_len;
							memcpy(&package_len,buffer[i],sizeof(int));
							if(buff_len[i]<package_len)
							{
								break;
							}
							MessageE m;
							DeserializeE(m,buffer[i]);

							//log
							cout<<"recv:"<<package_len<<"bytes;";
							cout<<" from:"<<i<<";";
							cout<<" -extend:"<<m.extend;
							cout<<" -client_id:"<<m.client_id<<";";
							cout<<" -key:"<<m.lock_key<<";";
							cout<<" -operate:"<<m.operate;
							if(m.operate==1)
							{
								cout<<" try lock;"<<endl;;
							}
							else if(m.operate==2)
							{
								cout<<" try unlock;"<<endl;
							}
							else if(m.operate==3)
							{
								cout<<" own;"<<endl;
							}

							//process
							int ret=0;//ok
							map<string,string>::iterator it;
							pthread_mutex_lock(&map_mutex);
							it=key_map.find(m.lock_key);
							if(m.operate==1)
							{
								if(it!=key_map.end())
								{
									ret=1;
								}
								else
								{
									//key_map.insert(pair<string,string>(m.lock_key,m.client_id));
									key_map[m.lock_key]=m.client_id;
									ret=0;
								}
							}
							else if(m.operate==2)
							{
								if((it!=key_map.end()) && (it->second==m.client_id))
								{
									key_map.erase(m.lock_key);
									ret=0;
								}
								else
								{
									ret=1;
								}
							}
							else if(m.operate==3)
							{
								if(it!=key_map.end() && it->second==m.client_id)
								{
									ret=0;
								}
								else
								{
									ret=1;
								}
							}
							else
							{
								cout<<"wrong operate :"<<m.operate<<endl;
							}

							pthread_mutex_unlock(&map_mutex);

							cout<<"ret:"<<ret<<endl;

							set<int>::iterator set_it;
							if((ret==0) && (m.operate==1 ||m.operate==2))
							{
								pthread_mutex_lock(&fds_mutex);
								for(set_it=follower_fds.begin();set_it!=follower_fds.end();set_it++)
								{
									int r=SendWhole(*set_it,buffer[i],package_len);
									if(r==false)
									{
										cout<<"sendwhole error ProcessCmdThread"<<endl;
									}
								}
								pthread_mutex_unlock(&fds_mutex);
							}

							//update buffer and buf_len
							int left=buff_len[i]-package_len;
							for(int ii=0;ii<left;ii++)
							{
								buffer[i][ii]=buffer[i][ii+package_len];
							}
							buff_len[i]=left;

							static char send_buf[sizeof(int)*3];
							static int l=sizeof(int)*3;
							memcpy(send_buf, &l, sizeof(int));
							memcpy(send_buf+sizeof(int), &ret, sizeof(int));
							memcpy(send_buf+sizeof(int)*2, &(m.extend), sizeof(int));
							bool r=SendWhole(i,send_buf,sizeof(int)*3);
							if(r==false)
							{
								cout<<"send error consensus_leader"<<endl;
							}

						}
					}
				}
			}
		}
	}


	close(listen_sockfd);

	pthread_exit(NULL);
}


