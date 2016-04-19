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
const int SERVER_PORT 	= 9101;

const char SERVER_IP[] 	= "47.88.34.145";
//const char SERVER_IP[] 	= "127.0.0.1";


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
	if(argc!=2)
	{
		cout << "client [username]"<<endl;
		return 0;
	}

	cout <<"connect to server ..."<<endl;
	int ret;

	DistributedLock lock1(SERVER_IP,SERVER_PORT,argv[1]);
	ret=lock1.ConnectToServer();
	if(ret)
	{
		cout <<"login success"<<endl;
		cout <<"welcome "<<argv[1]<<endl;
	}

	cout << "usage:" << endl;
	cout << "	lock [lockname]"<<endl;
	cout << "	unlock [lockname]"<<endl;
	cout << "	ownlock [lockname]"<<endl;
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
	if(ret)
	{
		cout<<"Disconnect from server"<<endl;
	}
	return 0;
}
