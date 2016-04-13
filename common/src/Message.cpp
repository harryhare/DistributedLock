#include "Message.h"
#include <string>
#include <cassert>
#include <cstring>
using namespace std;



bool IsEqual(const Message &m1,const Message &m2)
{
	bool b1 = m1.total_length == m2.total_length;
	bool b2 = m1.operate == m2.operate;
	bool b3 = m1.client_id == m2.client_id;
	bool b4 = m1.lock_key == m2.lock_key;
	return (b1)&&(b2)&&(b3)&&(b4);
}

int Serialize(Message & m,char*buf)
{
	int length=0;

	//memcpy(buf+length, &(m.total_length), sizeof(int));
	length+=sizeof(int);

	memcpy(buf+length, &(m.operate), sizeof(int));
	length+=sizeof(int);

	int t=m.client_id.length();
	memcpy(buf+length, &(t), sizeof(int));
	length+=sizeof(int);
	memcpy(buf+length, m.client_id.c_str(),t);
	length+=t;

	t=m.lock_key.length();
	memcpy(buf+length, &(t), sizeof(int));
	length+=sizeof(int);
	memcpy(buf+length, m.lock_key.c_str(),t);
	length+=t;

	memcpy(buf, &length, sizeof(int));
	m.total_length=length;
	return length;
}

bool Deserialize(char*buf, Message &m)
{

	int index=0;
	memcpy(&(m.total_length),buf+index,sizeof(int));
	index+=sizeof(int);

	memcpy(&(m.operate),buf+index,sizeof(int));
	index+=sizeof(int);

	int t;
	memcpy(&t,buf+index,sizeof(int));
	index+=sizeof(int);
	if(t>1024)
	{
		return false;
	}
	char*client_id=new char[t+1];
	if(client_id==NULL)
	{
		return false;
	}
	memcpy(client_id,buf+index,t);
	index+=t;
	client_id[t]=0;
	m.client_id=string(client_id);
	delete[]client_id;

	memcpy(&t,buf+index,sizeof(int));
	index+=sizeof(int);
	if(t>1024)
	{
		return false;
	}
	char*lock_key=new char[t+1];
	if(lock_key==NULL)
	{
		return false;
	}
	memcpy(lock_key,buf+index,t);
	index+=t;
	lock_key[t]=0;
	m.lock_key=string(lock_key);
	delete[]lock_key;

	if(index!=m.total_length)
	{
		return false;
	}
	return true;
}

void TestMessage()
{
	Message m;
	m.operate=1;
	m.client_id=string("client_id");
	m.lock_key=string("key1");
	m.total_length=sizeof(int)*4+m.client_id.length()+m.lock_key.length();
	char buf[1024];
	assert(m.total_length<1024);
	Serialize(m,buf);
	Message m_test;
	Deserialize(buf,m_test);
	assert(IsEqual(m,m_test));
}

