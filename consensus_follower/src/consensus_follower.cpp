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
#include "tinyxml.h"


using namespace std;

int LEADER_PORT_CMD	= 8101;
int LEADER_PORT_SYN	= 8102;
int LEADER_PORT_INIT	= 8103;
char LEADER_IP[16]	= "47.88.34.145";
int SERVER_PORT 	= 9101;
char SERVER_IP[16] 	= "127.0.0.1";
int MAX_PENDING_CLIENT =5;//listen


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

const char default_conf_file[]="follower_conf.xml";

int main(int argc, char**argv) {
	int ret;

	const char *file_name;
	cout<<"load configuration"<<endl;
	if(argc==2)
	{
		file_name=argv[1];
	}
	else
	{
		file_name=default_conf_file;
	}
	FILE*fp=fopen(file_name,"r");
	if(fp==NULL)
	{
		cout<<"can't open file:"<<file_name<<endl;
	}
	TiXmlDocument conf_doc;
	ret=conf_doc.LoadFile(fp);
	if(ret==false)
	{
		cout<<"load configure file fail"<<endl;
	}
	//cout<<conf_doc.RootElement()->Value()<<endl;
	//cout<<conf_doc.RootElement()->FirstChildElement("leader")->Value()<<endl;
	TiXmlElement *e_leader	= conf_doc.RootElement()->FirstChildElement("leader");
	const char *ip			= e_leader->FirstChildElement("ip")->GetText();
	const char *init_port	= e_leader->FirstChildElement("init_port")->GetText();
	const char *syn_port 	= e_leader->FirstChildElement("syn_port")->GetText();
	const char *cmd_port 	= e_leader->FirstChildElement("cmd_port")->GetText();
	TiXmlElement *e_follower= conf_doc.RootElement()->FirstChildElement("follower");
	const char *f_port		= e_follower->FirstChildElement("cmd_port")->GetText();
	LEADER_PORT_INIT = atoi(init_port);
	LEADER_PORT_SYN  = atoi(syn_port);
	LEADER_PORT_CMD  = atoi(cmd_port);
	SERVER_PORT		 = atoi(f_port);
	strcpy(LEADER_IP,ip);
	fclose(fp);
	cout<<"leader ip:"<<LEADER_IP<<endl;
	cout<<"cmd port:"<<LEADER_PORT_CMD<<endl;
	cout<<"syn port:"<<LEADER_PORT_SYN<<endl;
	cout<<"init port:"<<LEADER_PORT_INIT<<endl;
	cout<<"listen port:"<<SERVER_PORT<<endl;


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

