/*
 * SynInitDateThread.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */

#include "IO.h"
#include "global.h"

#include <iostream>
#include <map>
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

int WriteToBuffer(char*buf,const string& str)
{
	int len=str.length();
	int total=sizeof(int)+len;
	memcpy(buf,&len,sizeof(int));
	memcpy(buf+sizeof(int),str.c_str(),len);
	return total;
}

void *SynInitDateThread(void *arg)//thread2
{
	cout<<"thread 2 start"<<endl;
	int listen_sockfd;
	int error;
	struct sockaddr_in hints;

	memset(&hints,0,sizeof(struct addrinfo));
	hints.sin_family=AF_INET;
	hints.sin_port=htons(LEADER_PORT_INIT);
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
	struct sockaddr_in client_addr;
#ifdef __CYGWIN__
	int len=sizeof(client_addr);
#else
	unsigned int len=sizeof(client_addr);
#endif

	while(1)
	{
		int client_fd=accept(listen_sockfd,(struct sockaddr*)&client_addr,&len);
		if(client_fd<0)
		{
			perror("accept");
			continue;
		}
		cout<<"incoming fd "<<client_fd<<" from "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;

		pthread_mutex_lock(&map_mutex);
		map<string,string> temp_map(key_map);
		pthread_mutex_unlock(&map_mutex);

		static char send_buf[2048];
		static size_t count=0;
		map<string,string>::iterator it;
		for(it=temp_map.begin();it!=temp_map.end();it++)
		{
			const string& key   = it->first;
			const string& value = it->second;
			//log
			cout<<key<<"->"<<value<<endl;
			int step;
			int total=key.length()+value.length()+3*sizeof(int);
			memcpy(send_buf+count,&total,sizeof(int));
			count+=sizeof(int);
			step = WriteToBuffer(send_buf+count,key);
			count += step;
			step = WriteToBuffer(send_buf+count,value);
			count += step;
			if(count>1024)
			{
				int ret=SendWhole(client_fd,send_buf,count);
				if(ret==false)
				{
					cout<<"sendwhile fail SynInitDataThread"<<endl;
					break;
				}
				else
				{
					count=0;
				}
			}
		}
		if(count!=0)
		{
			int ret=SendWhole(client_fd,send_buf,count);
			if(ret==false)
			{
				cout<<"sendwhile fail SynInitDataThread"<<endl;
			}
		}
		count=0;
		close(client_fd);
	}
	close(listen_sockfd);
	pthread_exit(NULL);
}

