#pragma once

#include "DataBase.h"
#include "map"
#include "Question.h"

class User;

class Game
{
public:
	Game(const vector<User*>& userList, int questionNo, DataBase& db);
	~Game();
	void sendFirstQuestion();
	void handleFinishGame();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* user);
	int getID();
	Question* getQuestion();
	int GetQuestionNo();
	void popQuestion();
	int getCurrQstID();
	void addTurn();
	int getTurn();
	void addCurrect(string username);
	int getCurrect(string username);

private:
	bool insertGameToDB();
	void initQuestionsFromDB();
	void sendQuestionToAllUsers();
	bool isGame(); //retunrn True if game ended
	

	vector<Question*> _questions;
	vector<User*> _players;
	int _question_no;
	int _currQuestionIndex;
	DataBase _db;
	map<string, int> _results;
	int _currentTurnAnswers;
	int _gameId;
	int answered;
};