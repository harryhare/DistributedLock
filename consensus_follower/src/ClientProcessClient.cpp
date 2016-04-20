#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Message.h"
#include "global.h"
#include "IO.cpp"

using namespace std;

void* ClientProcessClient(void*)
{
	cout<<"Thread 4 start"<<endl;
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

							Message m;
							Deserialize(m,buffer[i]);
							int left=buff_len[i]-package_len;
							for(int ii=0;ii<left;ii++)
							{
								buffer[i][ii]=buffer[i][ii+package_len];
							}
							buff_len[i]=left;

							//log
							cout<<"recv:"<<m.total_length<<"bytes;";
							cout<<" from:"<<i<<";";
							cout<<" -client_id:"<<m.client_id<<";";
							cout<<" -key:"<<m.lock_key<<";";
							cout<<" -operate:"<<m.operate;
							if(m.operate==1)
							{
								cout<<" try lock;"<<endl;
							}
							else if(m.operate==2)
							{
								cout<<" try unlock;"<<endl;
							}
							else if(m.operate==3)
							{
								cout<<" own;"<<endl;
							}

							if(m.operate==1||m.operate==2)
							{
								MessageE me;
								me.extend=i;
								me.operate=m.operate;
								me.client_id=m.client_id;
								me.lock_key=m.lock_key;
								static char buf_send[256];
								int len=SerializeE(buf_send,me);
								SendWhole(fd_leader,buf_send,len);
							}
							else if(m.operate==3)
							{
								int ret=0;//ok
								map<string,string>::iterator it;
								pthread_mutex_lock(&map_mutex);
								it=key_map.find(m.lock_key);
								if(it!=key_map.end() && it->second==m.client_id)
								{
									ret=0;
								}
								else
								{
									ret=1;
								}
								pthread_mutex_unlock(&map_mutex);
								send(i,&ret,sizeof(int),0);
							}
						}
					}
				}
			}
		}
	}


	close(listen_sockfd);

}
