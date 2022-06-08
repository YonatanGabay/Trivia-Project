#include "Room.h"
#include "User.h"

Room::Room(int id, User* admin, string name, int maxUsers, int  questionNo, int questionTime)
{
	_id = id;
	_admin = admin;
	joinRoom(admin);
	_name = name;
	_maxUsers = maxUsers;
	_questionTime = questionTime;
	_questionNo = questionNo;
	_users.push_back(admin);
}

Room::~Room(){}

bool Room::joinRoom(User* user)
{
	int count = 0;

	for (vector<User*>::iterator it = _users.begin(); it != _users.end(); it++)
		count++;	// counts the user number

	if (count < _maxUsers) //if there is space
	{
		_users.push_back(user); //add user to list
		user->setRoom(this);
		return true;
	}


	return false;
}

void Room::leaveRoom(User* user)
{
	this->_users.erase(find(this->_users.begin(), this->_users.end(), user));
}

void Room::closeRoom(User* user)
{
	for (int i = 0; i < this->_users.size(); i++)
	{
		this->_users[i]->clearRoom();
		::send(this->_users[i]->getSocket(), "116", 3, 0);
	}
}

vector<User*> Room::getUsers()
{
	return _users;
}

string Room::getUsersListMessage()
{
	return "userslist(transform from vector to list?)";
}

int Room::getQuestionsNo()
{
	return _questionNo;
}

int Room::getQuestionsTime()
{
	return this->_questionTime;
}

int Room::getId()
{
	return _id;
}

string Room::getName()
{
	return _name;
}

User * Room::getAdmin()
{
	return this->_admin;
}

string Room::getUsersAsString(vector<User*> users, User* user)
{
	string list = "";
	for (vector<User*>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		list += (*it)->getUsername();
	}
	return list;
}

void Room::sendMessage(string msg)
{
	for (int i = 0; i < _users.size(); i++)
	{
		sendMessage(_users[i], msg);
	}
}
void Room::sendMessage(User * user, string msg)
{
	user->send(msg);
	//throw(new exception &e);
}