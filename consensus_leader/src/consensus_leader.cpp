//============================================================================
// Name        : consensus_client.cpp
// Author      : mayue
// Version     :
// Copyright   :
// Description : homework of mayue @ sjtu
//============================================================================

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <map>

#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Message.h"
#include "IO.h"
#include "global.h"

using namespace std;

const int LEADER_PORT_CMD	= 8101;
const int LEADER_PORT_SYN	= 8102;
const int LEADER_PORT_INIT	= 8103;
const char LEADER_IP[]	= "127.0.0.1";
const int SERVER_PORT 	= 9101;
const char SERVER_IP[] 	= "127.0.0.1";

const int MAX_PENDING_CLIENT =5;//listen

map<string,string> key_map;
set<int> follower_fds;
pthread_mutex_t map_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fds_mutex = PTHREAD_MUTEX_INITIALIZER;

void *ProcessCmdThread(void *arg);//thread 1
void *SynInitDateThread(void *arg);//thread2
void *SynTemporaryDataThread(void *arg);//thread3


int main()
{
	cout<<"leader start"<<endl;

	pthread_t id1;
	pthread_t id2;
	pthread_t id3;
	pthread_mutex_init(&map_mutex,NULL);
	pthread_mutex_init(&fds_mutex,NULL);

	int ret;
	ret=pthread_create(&id1,NULL,ProcessCmdThread,NULL);
	if(ret!=0)
	{
		perror("thread_create() 1");
		return 0;
	}
	ret=pthread_create(&id2,NULL,SynInitDateThread,NULL);
	if(ret!=0)
	{
		perror("thread_create() 2");
		return 0;
	}
	ret=pthread_create(&id3,NULL,SynTemporaryDataThread,NULL);
	if(ret!=0)
	{
		perror("thread_create() 3");
		return 0;
	}


	ret=pthread_join(id3,NULL);
	if(ret!=0)
	{
		perror("thread_join() 3");
	}
	else
	{
		cout<<"thread 3 stop"<<endl;
	}
	ret=pthread_join(id2,NULL);
	if(ret!=0)
	{
		perror("thread_join() 2");
	}
	else
	{
		cout<<"thread 2 stop"<<endl;
	}
	ret=pthread_join(id1,NULL);
	if(ret!=0)
	{
		perror("thread_join() 1");
	}
	else
	{
		cout<<"thread 1 stop"<<endl;
	}

	return 0;
}
