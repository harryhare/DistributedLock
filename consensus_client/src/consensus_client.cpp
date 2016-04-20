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
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <cstdio>
#include <unistd.h>

#include "Message.h"
#include "DistributedLock.h"

using namespace std;

const int LEADER_PORT_CALL	= 8101;
const int LEADER_PORT_SYN	= 8102;
const int LEADER_PORT_INIT	= 8103;
const char LEADER_IP[]	= "127.0.0.1";

int SERVER_PORT 	= 9101;
//const char SERVER_IP[] 	= "47.88.34.145";
char SERVER_IP[] 	= "127.0.0.1";
char USER_NAME[]	= "mayue001";

void test()
{
	char name[]="username1";
	char lock[]="lock1";
	int ret;
	DistributedLock lock1(SERVER_IP,SERVER_PORT,name);
	ret=lock1.ConnectToServer();
	assert(ret);
	ret=lock1.OwnTheLock(string(lock));
	assert(ret==false);
	ret=lock1.TryLock(string(lock));
	assert(ret);
	ret=lock1.OwnTheLock(string(lock));
	assert(ret==true);
	ret=lock1.TryUnlock(string(lock));
	assert(ret);
	ret=lock1.OwnTheLock(string(lock));
	assert(ret==false);
	ret=lock1.DisconnectFromServer();
	assert(ret);
}

void test2()
{
	char name[]="username001";
	char lock[]="lock";
	int ret;
	DistributedLock lock1(SERVER_IP,SERVER_PORT,name);
	ret=lock1.ConnectToServer();
	assert(ret);
	for(int i=0;i<10;i++)
	{
		ret=lock1.TryLock(string(lock)+(char)(i+'0'));
		assert(ret);
	}
	for(int i=0;i<10;i++)
	{
		ret=lock1.OwnTheLock(string(lock)+(char)(i+'0'));
		assert(ret);
	}
	for(int i=0;i<10;i++)
	{
		ret=lock1.TryLock(string(lock)+(char)(i+'0'));
		assert(ret==false);
	}
	ret=lock1.DisconnectFromServer();
	assert(ret);
}



int main(int argc,char**argv) {
//	if(argc!=2)
//	{
//		cout << "client [username]"<<endl;
//		return 0;
//	}
	char*user_name = USER_NAME;
	char*server_ip = SERVER_IP;
	int server_port= SERVER_PORT;
	int ch;
	bool simple_mode=false;
	opterr=0;
	while( (ch=getopt(argc,argv,"u:h:p:s")) != -1)
	{
		switch(ch)
		{
			case 'u':
				//cout<<optarg<<endl;
				user_name=optarg;
				break;
			case 'h':
				//cout<<optarg<<endl;
				server_ip=optarg;
				break;
			case 'p':
				//cout<<optarg<<endl;
				server_port=atoi(optarg);
				break;
			case 's':
				simple_mode=true;
				break;
			default:
				cout<<"usage:"<<endl;
				cout<<"client [-h ip] [-p port] [-u usenrame] [-s]"<<endl;
				cout<<"\t-h:host_ip"<<endl;
				cout<<"\t-p:host_port"<<endl;
				cout<<"\t-u:user name"<<endl;
				cout<<"\t-s:simple mode, no hints"<<endl;
		}
	}

	if(!simple_mode)
	{
		cout<<"server: "<<server_ip<<":"<<server_port<<endl;
		cout<<"log in as: "<<user_name<<endl;

		cout <<"connect to server ..."<<endl;
	}
	int ret;

	DistributedLock lock1(server_ip,server_port, user_name);
	ret=lock1.ConnectToServer();
	if(ret&&!simple_mode)
	{
		cout <<"login success"<<endl;
		cout <<"welcome "<<user_name<<endl;
	}
	if(!simple_mode)
	{
		cout << "usage:" << endl;
		cout << "	lock [lockname]"<<endl;
		cout << "	unlock [lockname]"<<endl;
		cout << "	ownlock [lockname]"<<endl;
	}
	//test();
	//test2();
	static char buf[1024];
	while(gets(buf)!=NULL)
	{
		if(buf[0]==0||buf[0]=='\n')
		{
			continue;
		}
		int i,j,k;
		for(i=0;buf[i]!=0;i++)
		{
			if(!isalpha(buf[i]))
			{
				break;
			}
		}
		buf[i]=0;
		j=i+1;
		while(isblank(buf[j]))
		{
			j++;
		}
		k=j;
		while(isalnum(buf[k])||(buf[k]=='_'))
		{
			k++;
		}
		if(k<=j)
		{
			cout<<"input error"<<endl;
			continue;
		}
		buf[k]=0;
		string lock_name(buf+j);
		if(strcmp("lock",buf)==0)
		{
			//cout<<"lock:"<<lock_name<<endl;
			ret = lock1.TryLock(lock_name);
			cout<<ret<<endl;
		}
		else if(strcmp("unlock",buf)==0)
		{
			//cout<<"unlock:"<<lock_name<<endl;
			ret = lock1.TryUnlock(lock_name);
			cout<<ret<<endl;

		}
		else if(strcmp("ownlock",buf)==0)
		{
			//cout<<"ownlock:"<<lock_name<<endl;
			ret = lock1.OwnTheLock(lock_name);
			cout<<ret<<endl;

		}
		else
		{
			cout<<"input error"<<endl;
		}
	}
	ret=lock1.DisconnectFromServer();
	if(ret&&!simple_mode)
	{
		cout<<"Disconnect from server"<<endl;
	}
	return 0;
}
