#include "Validator.h"

bool Validator::isPasswordValid(string password)
{
	bool ret = 0;

	if (password.length() >= 4 &&
		password.find(" ") == string::npos &&
		password.find_first_of("0123456789") != string::npos &&
		password.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") != string::npos &&
		password.find_first_of("abcdefghijklmnopqrstuvwxyz") != string::npos)
	{
		ret = 1;
	}

	return ret;
}

bool Validator::isUsernameValid(string username)
{
	bool ret = 0;

	if (username != "" &&
		((username[0] >= 'a' && username[0] <= 'z') || (username[0] >= 'A' && username[0] <= 'Z')) &&
		username.find(" ") == string::npos)
	{
		ret = 1;
	}

	return ret;
}