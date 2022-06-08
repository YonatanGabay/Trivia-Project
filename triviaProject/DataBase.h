#pragma once

#include <vector>
#include <string>
#include "sqlite3.h"
#include "Question.h"
#include <unordered_map>


class DataBase
{
public:
	DataBase();
	~DataBase();

	bool isUserExists(string);
	string addNewUser(string, string, string);
	bool isUserAndPassMatch(string, string);

	vector<Question*> initQuestions(int);

	vector<string> getBestScores();
	vector<string> getPersonalStatus(string);

	int insertNewGame();

	bool updateGameStatus(int name);
	bool addAnswerToPlayer(int, string, int, string, bool, int);

private:
	int static callbackCount(void*, int, char**, char**);
	int static callbackQuestions(void*, int, char**, char**);
	int static callbackBestScores(void*, int, char**, char**);
	int static callbackPersonalStatus(void*, int, char**, char**);

	int static _count;
	int static _gameId;
	sqlite3* _db;
};