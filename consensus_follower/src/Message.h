/*
 * Message.h
 *
 *  Created on: 2016��4��11��
 *      Author: mayue
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
using namespace std;
struct Message
{
	int total_length;
	int operate;	//1trylock, 2unlock, 3isown
	string client_id;
	string lock_key;

};

bool IsEqual(const Message &m1,const Message &m2);
int Serialize(const Message & m,char*buf);
bool Deserialize(char*buf, Message &m);
void TestMessage();


#endif /* MESSAGE_H_ */
