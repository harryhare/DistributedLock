#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "Message.h"
#include <map>
#include <queue>
#include <string>
#include <pthread.h>

extern int LEADER_PORT_CMD;
extern int LEADER_PORT_SYN;
extern int LEADER_PORT_INIT;
extern char LEADER_IP[];
extern int SERVER_PORT;
extern char SERVER_IP[];
extern int MAX_PENDING_CLIENT;//listen

extern std::map<std::string,std::string> key_map;
extern std::queue<MessageE> syn_queue;
extern pthread_mutex_t map_mutex;
extern pthread_mutex_t fd_leader_mutex;
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t queue_cond;
extern int fd_leader;//multi thread
extern int fd_leader_syn;
extern int fd_leader_init;

#endif
