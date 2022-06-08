#pragma once

#include <string>
#include <vector>
#include "WSAInitializer.h"
#include "Helper.h"
#include "Game.h"
using namespace std;

class Room;

class User
{
private:
	string _userName;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;

public:
	User(string userName, SOCKET sock);
	void send(string message);
	string getUsername();
	SOCKET getSocket();
	Room* getRoom();
	Game* getGame();
	void setGame(Game* gm);
	void clearRoom(); 
	bool createRoom(int id, string roomName, int maxUsers, int questionsNo, int questionTime);
	bool joinRoom(Room* f);
	void setRoom(Room* room);
	void leaveRoom();
	int closeRoom();
	bool leaveGame();
};