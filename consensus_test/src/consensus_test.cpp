//============================================================================
// Name        : consensus_test.cpp
// Author      : mayue
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "DistributedLock.h"

using namespace std;

void test()
{
	char server_ip[]="47.88.34.145";
	int server_port[5]={9101,9102,9103,9104,9105};
	char user_name[100][16];
	char lock_name[20][16];
	DistributedLock *clients[100];
	for(int i=0;i<100;i++)
	{
		sprintf(user_name[i],"user%03d",i);
	}
	for(int i=0;i<20;i++)
	{
		sprintf(lock_name[i],"lock%03d",i);
	}

	int ret;

	int max=20;
	for(int i=0;i<max;i++)
	{
		clients[i]=new DistributedLock(server_ip,server_port[i%5],user_name[i]);
	}
	for(int i=0;i<max;i++)
	{
		ret=clients[i]->ConnectToServer();
		assert(ret);
	}
	for(int j=0;j<max;j++)
	{
		string lock1=string(user_name[j])+lock_name[0];
		ret=clients[j]->TryLock(lock1);
		assert(ret);
		for(int i=0;i<max;i++)
		{
			if(i==j)
			{
				continue;
			}
			ret=clients[i]->TryLock(lock1);
			assert(ret==false);
		}
	}
	for(int j=0;j<max;j++)
	{
		string lock1=string(user_name[j])+lock_name[0];
		ret=clients[j]->OwnTheLock(lock1);
		assert(ret);
		for(int i=0;i<max;i++)
		{
			if(i==j)
			{
				continue;
			}
			ret=clients[i]->OwnTheLock(lock1);
			//assert(ret==false);
		}
	}
	for(int j=0;j<max;j++)
	{
		string lock1=string(user_name[j])+lock_name[0];
		for(int i=0;i<max;i++)
		{
			if(i==j)
			{
				continue;
			}
			ret=clients[i]->TryUnlock(lock1);
			assert(ret==false);
		}
		ret=clients[j]->TryUnlock(lock1);
		assert(ret);
	}

	for(int i=0;i<max;i++)
	{
		ret=clients[i]->DisconnectFromServer();
		assert(ret);
	}
	for(int i=0;i<max;i++)
	{
		delete clients[i];
	}
}



int main() {
	cout << "" << endl; // prints 
	test();
	return 0;
}
