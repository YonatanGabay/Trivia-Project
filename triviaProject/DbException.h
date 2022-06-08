#pragma once

#include <iostream>
#include <exception>

using namespace std;

class DbException : public exception
{
public:
	DbException();
	~DbException();

	string what();
};

