#pragma once

#include "user.h"

class Room
{
public:
	Room(int id, User* admin, string name, int maxUsers, int questionNo, int questionTime);
	~Room();
	bool joinRoom(User* user);
	void leaveRoom(User* user);
	void closeRoom(User* user);
	vector<User*> getUsers();
	string getUsersListMessage();
	int getQuestionsNo();
	int getQuestionsTime();
	int getId();
	string getName();
	User* getAdmin();

private:
	vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionNo;
	string _name;
	int _id;

	string getUsersAsString(vector<User*> users, User* user);
	void sendMessage(string msg);
	void sendMessage(User* user, string msg);
};

