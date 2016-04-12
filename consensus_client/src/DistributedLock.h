/*
 * DistributedLock.h
 *
 *  Created on: 2016年4月12日
 *      Author: mayue
 */

#ifndef DISTRIBUTEDLOCK_H_
#define DISTRIBUTEDLOCK_H_

#include <string>



class DistributedLock
{
public:
	DistributedLock(const char*ip,int port,const char*name); // 生成ClientId，建立到Server 的连接
	virtual ~DistributedLock();
	bool ConnectToServer(); // 尝试建立到指定Server 的连接
	bool DisconnectFromServer();
	bool TryLock(const std::string &lockKey);
	bool TryUnlock(const std::string &lockKey);
	bool OwnTheLock(const std::string &lockKey);
	static std::string GetClientId(const char*name); // 根据UUID 生成ClientId
private:
	std::string client_id_;
	bool is_connected_;
	int fd_; // the descriptor used to talk to the consensus system
	std::string server_ip_;
	int server_port_;
};
#endif /* DISTRIBUTEDLOCK_H_ */
