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

#include <unistd.h>

#include "Message.h"
#include "DistributedLock.h"

using namespace std;

const int LEADER_PORT_CALL	= 8101;
const int LEADER_PORT_SYN	= 8102;
const int LEADER_PORT_INIT	= 8103;
const char LEADER_IP[]	= "127.0.0.1";
const int SERVER_PORT 	= 9101;

//const char SERVER_IP[] 	= "47.88.34.145";
const char SERVER_IP[] 	= "127.0.0.1";


void test()
{
	char name[]="username1";
	char lock[]="lock1";
	int ret;
	DistributedLock lock1(SERVER_IP,SERVER_PORT,name);
	ret=lock1.ConnectToServer();
	ret=lock1.TryLock(string(lock));
	ret=lock1.TryUnlock(string(lock));
	ret=lock1.OwnTheLock(string(lock));
	ret=lock1.DisconnectFromServer();
}



int main() {
	cout << "test" << endl;
	test();
	return 0;
}
