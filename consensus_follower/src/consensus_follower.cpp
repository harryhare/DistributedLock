//============================================================================
// Name        : consensus_client.cpp
// Author      : mayue
// Version     :
// Copyright   :
// Description : homework of mayue @ sjtu
//============================================================================

#include <iostream>
#include <map>
#include <string>
#include <list>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "global.h"
#include "IO.h"


using namespace std;

const int LEADER_PORT_CMD	= 8101;
const int LEADER_PORT_SYN	= 8102;
const int LEADER_PORT_INIT	= 8103;
const char LEADER_IP[]	= "47.88.34.145";
const int SERVER_PORT 	= 9101;
const char SERVER_IP[] 	= "127.0.0.1";
const int MAX_PENDING_CLIENT =5;//listen


map<string,string> key_map;
queue<MessageE> syn_queue;
pthread_mutex_t map_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fd_leader_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  queue_cond=PTHREAD_COND_INITIALIZER;
int fd_leader;
int fd_leader_syn;
int fd_leader_init;

int ConnectTo(const char* ip,int port);

void* ClientProcessClient(void*);

void* LeaderCmdThread(void*);

void* LeaderInitThread(void*);

void* LeaderSynThread(void*);



int main() {
	cout << "follower start" << endl; // prints !!!Hello World!!!
	cout << "connect to leader" <<endl;

	fd_leader=ConnectTo(LEADER_IP,LEADER_PORT_CMD);
	if(fd_leader<0)
	{
		return 0;
	}
	fd_leader_syn=ConnectTo(LEADER_IP,LEADER_PORT_SYN);
	if(fd_leader_syn<0)
	{
		return 0;
	}
	fd_leader_init=ConnectTo(LEADER_IP,LEADER_PORT_INIT);
	if(fd_leader_init<0)
	{
		return 0;
	}

	pthread_t id1;//leader_syn
	pthread_t id2;//leader_init
	pthread_t id3;//leader_cmd
	pthread_t id4;//client_processer

	int ret;
	pthread_mutex_init(&map_mutex,NULL);
	pthread_mutex_init(&fd_leader_mutex,NULL);
	pthread_mutex_init(&queue_mutex,NULL);
	pthread_cond_init(&queue_cond,NULL);

	ret=pthread_create(&id1,NULL,LeaderSynThread,NULL);
	if(ret!=0)
	{
		perror("thread_create() 1");
		return 0;
	}
	ret=pthread_create(&id2,NULL,LeaderInitThread,NULL);
	if(ret!=0)
	{
		perror("thread_create() 2");
		return 0;
	}
	ret=pthread_create(&id3,NULL,LeaderCmdThread,NULL);
	if(ret!=0)
	{
		perror("thread_create() 3");
		return 0;
	}
	ret=pthread_create(&id4,NULL,ClientProcessClient,NULL);
	if(ret!=0)
	{
		perror("thread_create() 4");
		return 0;
	}

	ret=pthread_join(id4,NULL);
	if(ret!=0)
	{
		perror("thread_join() 4");
	}
	else
	{
		cout<<"thread 4 stop"<<endl;
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

