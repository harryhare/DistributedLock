/*
 * DistributedLock.cpp
 *
 *  Created on: 2016Äê4ÔÂ12ÈÕ
 *      Author: mayue
 */

#include "DistributedLock.h"
#include "Message.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

using namespace std;

DistributedLock::DistributedLock(const char*ip,int port,const char*name)
{
	fd_=-1;
	server_ip_ = string(ip);
	server_port_ = port;
	is_connected_ = false;
	client_id_ = DistributedLock::GetClientId(name);
}

DistributedLock::~DistributedLock()
{
	if(fd_!=-1 && is_connected_)
	{
		close(fd_);
	}
}

string DistributedLock::GetClientId(const char*name)
{
	return string(name);
}

bool DistributedLock::ConnectToServer()
{
	int sock;
	int error;
	sock=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(server_port_);
	server_addr.sin_addr.s_addr=inet_addr(server_ip_.c_str());

	error=connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(error<0)
	{
		perror("connect fail");
		return false;
	}
	fd_=sock;
	is_connected_=true;
	return true;
}
bool DistributedLock::DisconnectFromServer()
{
	int error;
	if((is_connected_==true) && fd_>=0)
	{
		error=close(fd_);
		if(error==0)
		{
			is_connected_=false;
			fd_=-1;
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

char* DistributedLock::GetMessage(int &len,int operate,const std::string &lock_key)
{
	static char buf[256];
	Message m;
	m.operate=operate;
	m.client_id=client_id_;
	m.lock_key=lock_key;
	len=Serialize(buf,m);
	assert(len<=(int)sizeof(buf));
	return buf;

}

bool DistributedLock::SendMessage(const char* buf, int len)
{
	int error;
	int sended=0;
	while(sended<len)
	{
		error=send(fd_,buf+sended,len-sended,0);
		if(error<0)
		{
			perror("send fail");
			return false;
		}
		sended+=error;
	}
	return true;
}

bool DistributedLock::RecvMessage(int &reply)
{
	int recved=0;
	int len=sizeof(int);
	int error;
	static char buf[256];
	while(recved<len)
	{
		error=recv(fd_,buf+recved,len-recved,0);
		if(error<0)
		{
			perror("recv fial");
			return false;
		}
		else if(error==0)
		{
			close(fd_);
			is_connected_=false;
			fd_=-1;
			return false;
		}
		recved+=error;
	}

	int return_code;
	memcpy(&return_code,buf,sizeof(int));
	reply=return_code;

	if(return_code==0)
	{
		return true;
	}
	return false;
}

bool DistributedLock::TryLock(const string &lock_key)
{
	if(is_connected_==false)
	{
		return false;
	}

	int len;
	char *buf;
	buf=GetMessage(len,(int)(TRY_LOCK),lock_key);

	int error;
	error=SendMessage(buf,len);
	if(error==false)
	{
		return false;
	}

	int reply;
	return RecvMessage(reply);
}

bool DistributedLock::TryUnlock(const string &lock_key)
{
	if(is_connected_==false)
	{
		return false;
	}

	int len;
	char *buf;
	buf=GetMessage(len,(int)(TRY_UNLOCK),lock_key);

	int error;
	error=SendMessage(buf,len);
	if(error==false)
	{
		return false;
	}

	int reply;
	return RecvMessage(reply);
}
bool DistributedLock::OwnTheLock(const string &lock_key)
{
	if(is_connected_==false)
	{
		return false;
	}

	int len;
	char *buf;
	buf=GetMessage(len,(int)(IS_OWN),lock_key);

	int error;
	error=SendMessage(buf,len);
	if(error==false)
	{
		return false;
	}

	int reply;
	return RecvMessage(reply);
}

