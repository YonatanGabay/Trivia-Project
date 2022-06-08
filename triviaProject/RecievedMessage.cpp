#include "RecievedMessage.h"
#include "User.h"

RecievedMessage::~RecievedMessage(){}

RecievedMessage::RecievedMessage(SOCKET sock, int messageCode)
{
	_sock = sock;
	_messageCode = messageCode;
}


RecievedMessage::RecievedMessage(SOCKET sock, int messageCode, vector<string> values)
{
	_sock = sock;
	_messageCode = messageCode;
	_values = values;
}

SOCKET RecievedMessage::getSock()
{
	return _sock;
}

User* RecievedMessage::getUser()
{
	return _user;
}

void RecievedMessage::setUser(User* user)
{
	_user = user;
}

int RecievedMessage::getMessageCode()
{
	return _messageCode;
}

vector<string>& RecievedMessage::getValues()
{
	return _values;
}