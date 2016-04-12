/*
 * DistributedLock.h
 *
 *  Created on: 2016��4��12��
 *      Author: mayue
 */

#ifndef DISTRIBUTEDLOCK_H_
#define DISTRIBUTEDLOCK_H_

#include <string>



class DistributedLock
{
public:
	DistributedLock(const char*ip,int port,const char*name); // ����ClientId��������Server ������
	virtual ~DistributedLock();
	bool ConnectToServer(); // ���Խ�����ָ��Server ������
	bool DisconnectFromServer();
	bool TryLock(const std::string &lockKey);
	bool TryUnlock(const std::string &lockKey);
	bool OwnTheLock(const std::string &lockKey);
	static std::string GetClientId(const char*name); // ����UUID ����ClientId
private:
	std::string client_id_;
	bool is_connected_;
	int fd_; // the descriptor used to talk to the consensus system
	std::string server_ip_;
	int server_port_;
};
#endif /* DISTRIBUTEDLOCK_H_ */
