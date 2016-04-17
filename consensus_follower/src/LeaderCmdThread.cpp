/*
 * LeaderCmdThread.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */
#include "IO.h"
#include "global.h"
#include <iostream>
#include <cstring>


using namespace std;


void* LeaderCmdThread(void*)
{
	cout<<"thread 3 start"<<endl;
	static char buf[sizeof(int)*3];
	size_t len;
	int client_fd;
	int ret;
	while(1)
	{
		RecvWhole(fd_leader,buf,len);
		if(len!=(unsigned)(sizeof(int)*3))
		{
			cout<<"error len!=12"<<endl;
		}
		memcpy(&ret, buf+sizeof(int),sizeof(int));
		memcpy(&client_fd, buf+sizeof(int)*2,sizeof(int));

		cout<<"recv: 12bytes; from leader;";
		cout<<" ret:"<<ret<<";";
		cout<<" client_fd:"<<client_fd<<endl;
		SendWhole(client_fd,buf+sizeof(int),sizeof(int));
	}

	pthread_exit(NULL);
}
