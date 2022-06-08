#pragma once

#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

class Question
{
public:
	Question(int, string, string, string, string, string);

	string getQuestion();

	string* getAnswers();

	int getCorrectAnswerIndex();
	int getId();
	void addAns();
	int getAns();


private:
	int _id;
	int _correctAnswerIndex;
	int _ans;

	string _question;

	string _answers[4];
	string _correctAnswer;
};