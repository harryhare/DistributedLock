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

struct MessageE
{
	int operate;
	int extend;
	string client_id;
	string lock_key;

};

enum OPERATE
{
	NONE,
	TRY_LOCK,
	TRY_UNLOCK,
	IS_OWN,
};


bool IsEqual(const Message &m1,const Message &m2);
bool IsEqual(const MessageE &m1,const MessageE &m2);
int Serialize(char*buf,Message & m);
bool Deserialize(Message &m,char*buf);
int SerializeE(char * buf, const MessageE & m);
bool DeserializeE(MessageE &m, char * buf);


void TestMessage();
void TestMessageE();



#endif /* MESSAGE_H_ */
