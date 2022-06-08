#include "User.h"
#include "Room.h"

User::User(string userName, SOCKET sock)
{
	_userName = userName;
	_sock = sock;
	_currRoom = nullptr;
	_currGame = nullptr;
}

void User::send(string message)
{
	Helper::sendData(_sock, message);
}

string User::getUsername()
{
	return _userName;
}

SOCKET User::getSocket()
{
	return _sock;
}

Room* User::getRoom()
{
	return _currRoom;
}

Game* User::getGame()
{
	return _currGame;
}

void User::setGame(Game* gm)
{
	_currGame = gm;
}


void User::clearRoom()
{
	_currRoom = nullptr;
}

bool User::createRoom(int id, string roomName, int maxUsers, int questionsNo, int questionTime)
{
	if (_currRoom)
	{
		this->send("114"); //code 114
		return FALSE;
	}
	Room newRoom = Room(id, this, roomName, maxUsers, questionsNo, questionTime);
	_currRoom = &newRoom;
	this->send("114"); //code 114
	return TRUE;
}

bool User::joinRoom(Room* f)
{
	if (_currRoom)
	{
		return FALSE;
	}
	else
	{
		_currRoom->joinRoom(this);
	}
}

void User::setRoom(Room * room)
{
	this->_currRoom = room;
}

void User::leaveRoom()
{
	if (_currRoom)
	{
		_currRoom->leaveRoom(this);
		clearRoom();
	}
}

int User::closeRoom()
{
	return 1;
}

bool User::leaveGame()
{
	if (_currGame)
	{
		_currGame->leaveGame(this);
		clearRoom();
		return FALSE; //F - game over , T - still live;
	}
}
