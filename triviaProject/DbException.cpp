#include "DbException.h"



DbException::DbException() : exception()
{
}


DbException::~DbException()
{
}

string DbException::what()
{
	return "Can't open the data base.";
}