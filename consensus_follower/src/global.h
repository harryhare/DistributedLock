#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <map>
#include <string>
#include <pthread.h>

extern const int LEADER_PORT_CMD;
extern const int LEADER_PORT_SYN;
extern const int LEADER_PORT_INIT;
extern const char LEADER_IP[];
extern const int SERVER_PORT;
extern const char SERVER_IP[];
extern const int MAX_PENDING_CLIENT;//listen

extern std::map<std::string,std::string> key_map;
extern pthread_mutex_t map_mutex;
extern int fd_leader;//multi thread
extern int fd_leader_syn;
extern int fd_leader_init;
extern pthread_mutex_t fd_leader_mutex;

#endif
