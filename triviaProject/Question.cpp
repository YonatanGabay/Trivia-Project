#include "Question.h"

using namespace std;

#define FOUR 4

Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4)
{
	int arr[FOUR] = { 0 };
	int randNum = 0;
	bool flag = 1;

	string answers[4] = { correctAnswer, answer2, answer3, answer4 };

	_id = id;
	_correctAnswer = correctAnswer;
	_question = question;
	this->_ans = 0;

	srand(time(NULL));

	for (int i = 0; i < FOUR;)
	{
		randNum = rand() % 4;

		for (int j = 0; j < i && flag; ++j)
		{
			if (arr[j] == randNum)
			{
				flag = 0;
			}
		}
		
		if (flag)
		{
			_answers[randNum] = answers[i];
			arr[i] = randNum;
			++i;
		}

		flag = 1;
	}

	for (int i = 0; i < FOUR && flag; ++i)
	{
		if (_answers[i] == correctAnswer)
		{
			_correctAnswerIndex = i;
			flag = 0;
		}
	}
}

string Question::getQuestion()
{
	return _question;
}

string* Question::getAnswers()
{
	return _answers;
}

int Question::getCorrectAnswerIndex()
{
	return _correctAnswerIndex;
}

int Question::getId()
{
	return _id;
}

void Question::addAns()
{
	this->_ans++;
}

int Question::getAns()
{
	return this->_ans;
}
