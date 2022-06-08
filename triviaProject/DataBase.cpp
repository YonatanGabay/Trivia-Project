#include "DataBase.h"
#include "DbException.h"
#include "Validator.h"

int DataBase::_count = 0;
int DataBase::_gameId = 0;

DataBase::DataBase()
{
	int rc;

	rc = sqlite3_open("trivia.db", &_db);

	if (rc)
	{
		sqlite3_close(_db);

		DbException ex;
		throw ex;
	}
}

DataBase::~DataBase()
{
	sqlite3_close(_db);
}

bool DataBase::isUserExists(string username)
{
	bool ret = 0;
	int rc = 0;
	char* zErrMsg = 0;
	string command = "SELECT * FROM t_users WHERE username = '" + username + "';";

	_count = 0;
	rc = sqlite3_exec(_db, command.c_str(), callbackCount, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;
	}
	else if (_count > 0)
	{
		ret = 1;
	}

	return ret;
}

string DataBase::addNewUser(string username, string password, string email)
{
	bool ret = 0;

	if (!isUserExists(username))
	{
		if (Validator::isUsernameValid(username))
		{
			if (Validator::isPasswordValid(password))
			{
				int rc = 0;
				char* zErrMsg = 0;
				string command = "INSERT INTO t_users(username, password, email) VALUES('" + username + "', '" + password + "', '" + email + "');";

				rc = sqlite3_exec(_db, command.c_str(), NULL, 0, &zErrMsg);

				if (rc != SQLITE_OK)
				{
					cout << "SQL error: " << zErrMsg << endl;
					return string("1044");
				}
			}
			else
			{
				return string("1041");
			}
		}
		else
		{
			return string("1043");
		}
	}
	else
	{
		return string("1042");
	}

	return string("1040");
}

bool DataBase::isUserAndPassMatch(string username, string password)
{
	bool ret = 0;
	int rc = 0;
	char* zErrMsg = 0;
	string command = "SELECT * FROM t_users WHERE username = '" + username + "' AND password = '" + password + "';";

	_count = 0;
	rc = sqlite3_exec(_db, command.c_str(), callbackCount, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;
	}
	else if (_count > 0)
	{
		ret = 1;
	}

	return ret;
}

vector<Question*> DataBase::initQuestions(int questionNo)
{
	int rc = 0;
	char* zErrMsg = 0;
	vector<Question*> questions;
	string command = "SELECT * FROM t_questions ORDER BY RANDOM() LIMIT " + to_string(questionNo) + ";";

	rc = sqlite3_exec(_db, command.c_str(), callbackQuestions, &questions, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;
	}

	return questions;
}

vector<string> DataBase::getBestScores()
{
	int rc = 0;
	char* zErrMsg = 0;
	vector<string> users;
	string command = "SELECT username, SUM(is_correct) FROM t_players_answers GROUP BY username ORDER BY SUM(is_correct) DESC LIMIT 3;";

	rc = sqlite3_exec(_db, command.c_str(), callbackBestScores, &users, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;
	}


	return users;
}

vector<string> DataBase::getPersonalStatus(string username)
{
	int rc = 0;
	char* zErrMsg = 0;
	vector<string> status;
	string command = "SELECT SUM(game_id), SUM(is_correct), SUM(is_correct == 0), AVG(answer_time) FROM t_players_answers WHERE username='" + username + "';";

	rc = sqlite3_exec(_db, command.c_str(), callbackPersonalStatus, &status, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;
	}

	return status;
}

int DataBase::insertNewGame()
{
	_gameId++;
	int rc = 0;
	char* zErrMsg = 0;
	string command = "INSERT INTO t_games(status, start_time, end_time) VALUES(0, datetime('now'));";

	rc = sqlite3_exec(_db, command.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;
		_gameId--;
	}

	return _gameId;
}

bool DataBase::updateGameStatus(int gameId)
{
	bool ret = 1;
	int rc = 0;
	char* zErrMsg = 0;
	string command = "UPDATE t_games SET end_time=datetime('now'), status=1 WHERE game_id=";
	command += gameId;
	command += ";";

	rc = sqlite3_exec(_db, command.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;

		ret = 0;
	}

	return ret;
}

bool DataBase::addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime)
{
	bool ret = 1;
	int rc = 0;
	char* zErrMsg = 0;
	string command = "INSERT INTO t_players_answers(game_id, username, question_id, player_answer, is_correct, answer_time) VALUES(" + to_string(gameId) +", '" + username + "', "  + to_string(questionId) + ", '" + answer + "', " + to_string(isCorrect) + ", " + to_string(answerTime) + ");";

	rc = sqlite3_exec(_db, command.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		cout << "SQL error: " << zErrMsg << endl;
		ret = 0;
	}

	return ret;
}

int DataBase::callbackCount(void* NotUsed, int argc, char** argv, char** azCol)
{
	_count++;

	return 0;
}

int DataBase::callbackQuestions(void* questions, int argc, char** argv, char** azCol)
{
	vector<Question*>* vec = (vector<Question*>*) questions;
	int id = stoi(argv[0]);

	vec->push_back(new Question(id, argv[1], argv[2], argv[3], argv[4], argv[5]));

	return 0;
}

int DataBase::callbackBestScores(void* users, int argc, char** argv, char** azCol)
{
	vector<string>* vec = (vector<string>*) users;

	vec->push_back(argv[0]);
	vec->push_back(argv[2]);

	return 0;
}

int DataBase::callbackPersonalStatus(void* status, int argc, char** argv, char** azCol)
{
	vector<string>* vec = (vector<string>*) status;

	vec->push_back(argv[0]);
	vec->push_back(argv[1]);

	return 0;
}