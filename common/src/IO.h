/*
 * IO.h
 *
 *  Created on: 2016Äê4ÔÂ18ÈÕ
 *      Author: mayue
 */

#ifndef IO_H_
#define IO_H_


#include <unistd.h>

bool RecvWhole(int fd, char*buf, size_t&len);
bool SendWhole(int fd, const char*buf, size_t len);

#endif


