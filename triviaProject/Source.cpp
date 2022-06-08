#include <iostream>
#include "TriviaServer.h"


using namespace std;



void main()
{
	TriviaServer* server = new TriviaServer();

	server->server();
	delete server;

	system("pause");
}

