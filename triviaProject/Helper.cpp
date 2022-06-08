#include "Helper.h"

using namespace std;

// recieves the type code of the message from socket (first byte)
// and returns the code. if no message found in the socket returns 0 (which means the client disconnected)
int Helper::getMessageTypeCode(SOCKET sc)
{
	char* s = getPartFromSocket(sc, 3);
	std::string msg(s);

	if (msg == "")
		return 0;

	int res = std::atoi(s);
	delete s;
	return  res;
}

int Helper::getMessageTypeCode(string message)
{
	if (message == "")
		return 0;

	return std::atoi(message.substr(0, 3).c_str());
}

// send data to socket
// this is private function
void Helper::sendData(SOCKET sc, std::string message)
{
	const char* data = message.c_str();

	if (send(sc, data, message.size(), 0) == INVALID_SOCKET)
	{
		throw std::exception("Error while sending message to client");
	}
}

int Helper::getIntPartFromSocket(SOCKET sc, int bytesNum)
{
	char* s = getPartFromSocket(sc, bytesNum, 0);
	return atoi(s);
}

string Helper::getStringPartFromSocket(SOCKET sc, int bytesNum)
{
	char* s = getPartFromSocket(sc, bytesNum, 0);
	string res(s);
	return res;
}

// recieve data from socket according byteSize
// this is private function
char* Helper::getPartFromSocket(SOCKET sc, int bytesNum)
{
	return getPartFromSocket(sc, bytesNum, 0);
}

char* Helper::getPartFromSocket(SOCKET sc, int bytesNum, int flags)
{
	if (bytesNum == 0)
	{
		return "";
	}

	char* data = new char[bytesNum + 1];
	int res = recv(sc, data, bytesNum, flags);

	if (res == INVALID_SOCKET)
	{
		std::string s = "Error while recieving from socket: ";
		s += std::to_string(sc);
		throw std::exception(s.c_str());
	}

	data[bytesNum] = 0;
	return data;
}

string Helper::getPaddedNumber(int num, int digits)
{
	std::ostringstream ostr;
	ostr << std::setw(digits) << std::setfill('0') << num;
	return ostr.str();

}

vector<string> Helper::getValuesFromMessage(string message) // didnt cheked yet
{
	int messageCode;
	vector<string> values;

	messageCode = Helper::getMessageTypeCode(message); // get message code
	message = message.substr(3, message.length() - 3); // remove message code

	if (messageCode == 207 || messageCode == 209)
	{
		values.push_back(message);
	}

	else if (messageCode == 213)
	{
		// room name
		int len = atoi(message.substr(0, 2).c_str()); // get room name len
		message = message.substr(2, message.length() - 2); // remove room name len
		values.push_back(message.substr(0, len)); // insert the room name
		message = message.substr(len, message.length() - len); // remove the room name

		// num players
		values.push_back(message.substr(0, 1)); // insert the new value
		message = message.substr(1, message.length() - 1); // remove the new value

		// num questions
		values.push_back(message.substr(0, 2)); // insert the new value
		message = message.substr(2, message.length() - 2); // remove the new value

		// seconds to answer
		values.push_back(message.substr(0, 2)); // insert the new value
		message = message.substr(2, message.length() - 2); // remove the new value
	}

	else if (messageCode == 219)
	{
		// answer num
		values.push_back(message.substr(0, 1)); // insert the new value
		message = message.substr(1, message.length() - 1); // remove the new value

		// seconds to answer
		values.push_back(message.substr(0, 2)); // insert the new value
		message = message.substr(2, message.length() - 2); // remove the new value
	}

	return values;
}

RecievedMessage* Helper::parseMessage(SOCKET sock, string message, bool& exit)
{
	RecievedMessage* newMessage = nullptr;
	int messageCode = Helper::getMessageTypeCode(message);
	vector<string> values = Helper::getValuesFromMessage(message);

	if (messageCode == 201 /*signOut*/ || messageCode == 299 /*closeApp*/)
		exit = true;

	if (values.size() == 0)
		newMessage = new RecievedMessage(sock, messageCode);
	else
		newMessage = new RecievedMessage(sock, messageCode, values);

	return newMessage;
}

void Helper::getUserNameAndPassword(string message, string & userName, string & password)
{
	// remove message code
	message = message.substr(3, message.length() - 3);

	// get userName length and remove it
	int userNameLength = atoi(message.substr(0, 2).c_str());
	message = message.substr(2, message.length() - 2);

	// get userName and remove it
	userName = message.substr(0, userNameLength);
	message = message.substr(userNameLength, message.length() - userNameLength);

	// get password length and remove it
	int passwordLength = atoi(message.substr(0, 2).c_str());
	message = message.substr(2, message.length() - 2);

	// get password and remove it
	password = message.substr(0, passwordLength);
	message = message.substr(passwordLength, message.length() - passwordLength);
}

void Helper::getUserNameAndPasswordAndEmail(string message, string & userName, string & password, string & email)
{
	// remove message code
	message = message.substr(3, message.length() - 3);

	// get userName length and remove it
	int userNameLength = atoi(message.substr(0, 2).c_str());
	message = message.substr(2, message.length() - 2);

	// get userName and remove it
	userName = message.substr(0, userNameLength);
	message = message.substr(userNameLength, message.length() - userNameLength);

	// get password length and remove it
	int passwordLength = atoi(message.substr(0, 2).c_str());
	message = message.substr(2, message.length() - 2);

	// get password and remove it
	password = message.substr(0, passwordLength);
	message = message.substr(passwordLength, message.length() - passwordLength);

	// get email length and remove it
	int emailLength = atoi(message.substr(0, 2).c_str());
	message = message.substr(2, message.length() - 2);

	// get password and remove it
	email = message.substr(0, emailLength);
	message = message.substr(emailLength, message.length() - emailLength);
}

string Helper::parseIntTo4Str(int val)
{
	string res = "";

	if (val < 10)
		res = string("000") + to_string(val);

	else if (val < 100)
		res = string("00") + to_string(val);

	else if (val < 1000)
		res = string("0") + to_string(val);

	else
		res = to_string(val);

	return res;
}

string Helper::parseIntTo6Str(int val)
{
	string res = "";

	if (val < 10)
		res = string("00000") + to_string(val);

	else if (val < 100)
		res = string("0000") + to_string(val);

	else if (val < 1000)
		res = string("000") + to_string(val);

	else if (val < 10000)
		res = string("00") + to_string(val);

	else if (val < 100000)
		res = string("0") + to_string(val);

	else
		res = to_string(val);

	return res;
}

string Helper::buildQueMessage(Question * que)
{
	string queMessage = "118";
	queMessage += Helper::parseIntTo3Str(que->getQuestion().length()) + que->getQuestion();
	queMessage += Helper::parseIntTo3Str(que->getAnswers()[0].length()) + que->getAnswers()[0];
	queMessage += Helper::parseIntTo3Str(que->getAnswers()[1].length()) + que->getAnswers()[1];
	queMessage += Helper::parseIntTo3Str(que->getAnswers()[2].length()) + que->getAnswers()[2];
	queMessage += Helper::parseIntTo3Str(que->getAnswers()[3].length()) + que->getAnswers()[3];

	return queMessage;
}

string Helper::parseIntTo2Str(int val)
{
	string res = "";

	if (val < 10)
		res = string("0") + to_string(val);

	else
		res = to_string(val);

	return res;
}

string Helper::parseIntTo3Str(int val)
{
	string res = "";

	if (val < 10)
		res = string("00") + to_string(val);

	else if (val < 100)
		res = string("0") + to_string(val);

	else
		res = to_string(val);

	return res;
}
