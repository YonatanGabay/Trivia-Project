#include"game.h"
#include "User.h"

Game::Game(const vector<User*>& userList, int questionNo, DataBase& db)
{
	_players = userList;

	for (size_t i = 0; i < this->_players.size(); i++)
		this->_results.insert(pair<string, int>(this->_players[i]->getUsername(), 0));

	
	_currQuestionIndex = 0;
	_currentTurnAnswers = 0;
	_question_no = questionNo;
	_db = db;
	initQuestionsFromDB();
}

Game::~Game()
{
	for (vector<Question*>::iterator it = _questions.begin(); it != _questions.end(); it++)
	{
		delete (*it);
	}
	_questions.clear();
	for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
	{
		delete (*it);
	}
	_players.clear();
}

void Game::sendFirstQuestion()
{
	sendQuestionToAllUsers();
}

void Game::handleFinishGame()
{
	_db.updateGameStatus(_gameId);
}

bool Game::handleNextTurn()
{
	if (!_players.empty())
	{
		for (int i = _players.size(); i > 0; i--)
		{
			if (_currentTurnAnswers == _players.size())
			{
				if (isGame())
				{
					handleFinishGame();
				}
				else
				{
					_currQuestionIndex++;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		handleFinishGame();
	}
	return isGame();
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	_currentTurnAnswers++;

	if (_questions[_currQuestionIndex]->getCorrectAnswerIndex() == answerNo)
	{
		_db.addAnswerToPlayer(this->getID(), user->getUsername(), _currQuestionIndex, (_questions[_currQuestionIndex]->getAnswers())[answerNo], 1, time);
	}
	else if (answerNo == 5)
	{
		_db.addAnswerToPlayer(this->getID(), user->getUsername(), _currQuestionIndex, "", 0, time);
	}
	else
	{
		_db.addAnswerToPlayer(this->getID(), user->getUsername(), _currQuestionIndex, (_questions[_currQuestionIndex]->getAnswers())[answerNo], 0, time);
	}
	
	return isGame();

}

bool Game::leaveGame(User* user)
{
	size_t idx = find(_players.begin(), _players.end(), user) - _players.begin();
	if (idx < _players.size())
	{
		_players.erase(_players.begin() + idx);
		handleNextTurn();
	}
	return isGame();
}

int Game::getID()
{
	return _gameId;
}

bool Game::insertGameToDB()
{
	_gameId = _db.insertNewGame();
	return 0; //change this return (for not error)
}

void Game::initQuestionsFromDB()
{
	_questions = _db.initQuestions(_question_no);
}

void Game::sendQuestionToAllUsers()
{
}

bool Game::isGame()
{
	if (_questions.begin() + _currQuestionIndex + 1 != _questions.end())
	{
		return TRUE;
	}
	return FALSE; //F - end , T - still
}

Question* Game::getQuestion()
{
	Question* que = this->_questions[this->_questions.size() - 1];
	return que;
}

int Game::GetQuestionNo()
{
	return _question_no;
}

void Game::popQuestion()
{
	this->_questions.pop_back();
}

int Game::getCurrQstID()
{
	return _currQuestionIndex;
}

void Game::addTurn()
{
	this->_currentTurnAnswers++;
}

int Game::getTurn()
{
	return this->_currentTurnAnswers;
}

void Game::addCurrect(string username)
{
	map<string, int>::iterator it = this->_results.find(username);
	if (it != this->_results.end())
		it->second++;
}

int Game::getCurrect(string username)
{
	map<string, int>::iterator it = this->_results.find(username);
	if (it != this->_results.end())
		return it->second;

	return 0;
}
