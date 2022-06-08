#pragma once
#include <iostream>
#include <map>
#include <queue>
#include <mutex>
#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#include "sqlite3.h"
#include "Helper.h"
#include "WSAInitializer.h"
#include "RecievedMessage.h"
#include "User.h"
#include "Game.h"
#include "RecievedMessage.h"
#include "Room.h"

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8820

using namespace std;


class TriviaServer
{
private:
	SOCKET _socket;
	WSAInitializer* _wsa;
	map<SOCKET, User*> _connectedUserd;
	DataBase _db;
	map<int, Room*> _roomList;
	int _getCurrQstID = 0;

	mutex _mtxRecievedMessages;
	queue<RecievedMessage*> _queRcvMessages;

	mutex _mtxGame;

	int _roomIdSequence; // need to be static

	void bindAndLisen();
	void accept();
	void accept(SOCKET clientSocket);
	void clientHandler(SOCKET sock);
	void safeDeleteUser(RecievedMessage* message);

	void handleSignIn(SOCKET sock, string message, bool& notConnected);
	void handleSignUp(SOCKET sock, string message);
	void handleSignOut(RecievedMessage* message);
	void clientTrd(SOCKET sock, User* user);

	void handleLeaveGame(RecievedMessage* message);
	void handleStartGame(RecievedMessage* message);
	void handlePlayerAnswer(RecievedMessage* message);
	void handleNextQuestion(Room* room, Game* game);
	void handleEndGame(Room* room, Game* game);
	
	void handleCreateRoom(RecievedMessage* message);
	void handleCloseRoom(RecievedMessage* message);
	void handleJoinRoom(RecievedMessage* message);
	void handleLeaveRoom(RecievedMessage* message);
	void handleGetUsersInRoom(RecievedMessage* message);
	void handleGetUsersInRoom(User* user, Room* room);
	void handleGetRoom(RecievedMessage* message);
	void hendleGetRoomList(RecievedMessage* message);
	void hendleCloseApp(RecievedMessage* message);

	void handleGetBestScores(RecievedMessage* message);
	void handleGetPersonalStatus(RecievedMessage* message);

	void handleRecievedMessage();
	void addRecievedMessage(RecievedMessage* message);
	RecievedMessage* buildRecievedMessage(SOCKET sock, int num);

	User* getUserByName(string userName);
	User* getUserBySocket(SOCKET userSocket);
	Room* getRoomById(int roomId);


public:
	TriviaServer();
	~TriviaServer();

	void server();
};

