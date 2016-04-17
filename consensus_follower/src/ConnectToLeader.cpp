/*
 * ConnectToLeader.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstdio>

#include "global.h"

int ConnectTo(const char* ip,int port)
{
	int sock;
	int error;
	sock=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=inet_addr(ip);

	error=connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(error<0)
	{
		perror("connect fail");
		close(sock);
		sock=-1;
	}
	return sock;
}

