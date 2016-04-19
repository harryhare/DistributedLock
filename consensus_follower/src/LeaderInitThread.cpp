/*
 * LeaderInitThread.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */

#include "global.h"

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>

using namespace std;



void* LeaderInitThread(void*)
{
	cout<<"thread 2 start"<<endl;
	static char buf[1024];
	int recved=0;
	while(1)
	{
		int l=recv(fd_leader_init,buf+recved,sizeof(buf)-recved,0);
		if(l<0)
		{
			perror("LeaderInitThread::recv");
			exit(0);
		}
		if(l==0)
		{
			assert(recved==0);
			cout<<"sys init data done"<<endl;
			break;
		}

		recved+=l;
		while(recved>=(int)sizeof(int))
		{
			int len;
			int len1;
			int len2;
			int index=0;
			memcpy(&len, buf, sizeof(int));
			if(recved<len)
			{
				break;
			}
			index+=sizeof(int);

			memcpy(&len1,buf+index,sizeof(int));
			index+=sizeof(int);
			string key_name(buf+index,len1);
			index+=len1;

			memcpy(&len2,buf+index,sizeof(int));
			index+=sizeof(int);
			string client_id(buf+index,len2);
			index+=len2;

			assert(index==len);

			pthread_mutex_lock(&map_mutex);
			key_map[key_name]=client_id;
			pthread_mutex_unlock(&map_mutex);

			int left=recved-len;
			for(int i=0;i<left;i++)
			{
				buf[i]=buf[i+len];
			}
			recved=left;
		}
	}

	while(1)
	{
		pthread_mutex_lock(&queue_mutex);
		while(syn_queue.empty())
		{
			pthread_cond_wait(&queue_cond,&queue_mutex);
		}
		MessageE m=syn_queue.front();
		syn_queue.pop();
		pthread_mutex_unlock(&queue_mutex);

		if(m.operate==1)
		{
			key_map[m.lock_key]=m.client_id;
			cout<<"map add:"<<m.lock_key<<"->"<<m.client_id<<endl;
		}
		else if(m.operate==2)
		{
			map<string,string>::iterator it;
			it=key_map.find(m.lock_key);
			assert(it!=key_map.end());
			assert(it->second==m.client_id);
			key_map.erase(it);
			cout<<"map rm:"<<m.lock_key<<"->"<<m.client_id<<endl;
		}
		else
		{
			assert(0);
		}
	}

	pthread_exit(NULL);
}


