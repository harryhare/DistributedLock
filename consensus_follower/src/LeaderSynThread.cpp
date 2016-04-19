/*
 * LeaderSynThread.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */

#include "Message.h"
#include "global.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>

using namespace std;


void* LeaderSynThread(void*)
{
	cout<<"thread 1 start"<<endl;
	static char buf[1024];
	int recved=0;
	while(1)
	{
		int l=recv(fd_leader_syn,buf+recved,sizeof(buf)-recved,0);
		if(l<0)
		{
			perror("LeaderInitThread::recv");
			exit(0);
		}
		if(l==0)
		{
			assert(recved==0);
			cout<<"Syn connection close"<<endl;
			break;
		}

		recved+=l;
		while(recved>=(int)sizeof(int))
		{
			int len;
			memcpy(&len,buf,sizeof(int));
			if(recved<len)
			{
				break;
			}

			MessageE m;
			bool temp=DeserializeE(m,buf);
			assert(temp==true);
			//log
			cout<<"syn:"<<len<<"bytes; ";
			cout<<"operate:"<<m.operate<<"; ";
			cout<<"key:"<<m.lock_key<<"; ";
			cout<<"client:"<<m.client_id<<";i "<<endl;
			pthread_mutex_lock(&queue_mutex);
			syn_queue.push(m);
			pthread_mutex_unlock(&queue_mutex);
			pthread_cond_signal(&queue_cond);

			int left=recved-len;
			for(int i=0;i<left;i++)
			{
				buf[i]=buf[i+len];
			}
			recved=left;
		}
	}



	pthread_exit(NULL);
}
