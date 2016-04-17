/*
 * LeaderCmdThread.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */
#include "IO.h"
#include <iostream>


using namespace std;


void* LeaderCmdThread(void*)
{
	cout<<"thread 3 start"<<endl;
	static char buf[12];
	size_t len;
	int client_fd;
	int ret;
	while(1)
	{
		RecvWhole(fd_leader,buf,len);
		if(len!=(unsigned)12)
		{
			cout<<"error len!=12"<<endl;
		}
		memcpy(&ret, buf+sizeof(int),sizeof(int));
		memcpy(&client_fd, buf+sizeof(int),sizeof(int));

		SendWhole(client_fd,buf+sizeof(int),sizeof(int));
	}

	pthread_exit(NULL);
}
