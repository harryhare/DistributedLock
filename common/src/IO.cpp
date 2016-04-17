/*
 * IO.cpp
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */

#include "IO.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>


bool RecvWhole(int fd, char*buf, size_t &total_len)
{
	size_t recved=0;
	size_t len=sizeof(int);
	int error;
	while(recved<len)
	{
		error=recv(fd,buf+recved,len-recved,0);
		if(error<0)
		{
			perror("recv fial");
			close(fd);
			return false;
		}
		else if(error==0)
		{
			close(fd);
			return false;
		}
		recved+=error;
	}
	memcpy(&len,buf,sizeof(int));
	while(recved<len)
	{
		error=recv(fd,buf+recved,len-recved,0);
		if(error<0)
		{
			perror("recv fial");
			close(fd);
			return false;
		}
		else if(error==0)
		{
			close(fd);
			return false;
		}
		recved+=error;
	}
	total_len=recved;
	return true;
}
bool SendWhole(int fd, const char*buf, size_t len)
{
	int error;
	size_t sended=0;
	while(sended<len)
	{
		error=send(fd,buf+sended,len-sended,0);
		if(error<0)
		{
			perror("send fail");
			return false;
		}
		sended+=error;
	}
	return true;
}

