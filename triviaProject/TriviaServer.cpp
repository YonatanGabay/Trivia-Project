#include "TriviaServer.h"


TriviaServer::TriviaServer() : _db()
{
	this->_wsa = new WSAInitializer();

	// notice that we step out to the global namespace
	// for the resolution of the function socket

	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the server use UDP we will use: SOCK_DGRAM & IPPROTO_UDP-9
	this->_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	this->_roomIdSequence = 1;
}

TriviaServer::~TriviaServer()
{
	delete this->_wsa;

	// the only use of the destructor should be for freeing 
	// resources that was allocated in the constructor
	::closesocket(this->_socket);
}

void TriviaServer::server()
{
	thread trd = thread(&TriviaServer::handleRecievedMessage, this);
	trd.detach();

	this->bindAndLisen();
}

void TriviaServer::bindAndLisen()
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(PORT); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

										// again stepping out to the global namespace
										// Connects between the socket and the configuration (port and etc..)
	
	if (::bind(this->_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming requests of clients
	if (::listen(this->_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	cout << "Listening on PORT " << PORT << endl;
	
	

	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		cout << "Waiting for client connection request" << endl;
		this->accept();
	}
}

void TriviaServer::accept()
{
	// this accepts the client and create a specific socket from server to this client
	SOCKET clientSocket = ::accept(this->_socket, NULL, NULL);

	if (clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	cout << "Client accepted." << endl;

	thread trd = thread(&TriviaServer::clientHandler, this, clientSocket);
	trd.detach();
}

void TriviaServer::accept(SOCKET clientSocket)
{
	cout << "Client accepted." << endl;

	thread trd =  thread(&TriviaServer::clientHandler, this, clientSocket);
	trd.detach();
}

void TriviaServer::clientHandler(SOCKET sock)
{
	bool notConnected = true;

	while (notConnected)
	{
		char buffer[1024] = { NULL };
		::recv(sock, buffer, 1024, 0);
		cout << buffer << endl;

		string message = string(buffer);

		int messageCode = atoi(message.substr(0, 3).c_str());

		if (messageCode == 200)
			this->handleSignIn(sock, message, notConnected);

		else if (messageCode == 203)
			this->handleSignUp(sock, message);
	}

	User* user = this->_connectedUserd.at(sock);

	this->clientTrd(sock, user);
}

void TriviaServer::safeDeleteUser(RecievedMessage* message)
{

}

void TriviaServer::handleSignOut(RecievedMessage* message)
{
	User* user = message->getUser();
	SOCKET sock = user->getSocket();

	if (user->getRoom() != nullptr)
	{
		if (user->getGame() != nullptr)
		{
			this->handleLeaveGame(&RecievedMessage(user->getSocket(), 222));
		}

		this->handleLeaveRoom(&RecievedMessage(user->getSocket(), 211));
	}

	this->_connectedUserd.erase(sock);
	delete user;

	this->accept(sock);
}

void TriviaServer::handleLeaveGame(RecievedMessage* message) 
{
	User* user = message->getUser();
	Game* game = user->getGame();

	this->_mtxGame.lock();
	game->leaveGame(user);
	this->_mtxGame.unlock();

	user->setGame(nullptr);
	user->setRoom(nullptr);
}

void TriviaServer::handleStartGame(RecievedMessage* message)
{
	User* adminUser = message->getUser();
	Room* room = adminUser->getRoom();
	
	if (adminUser == room->getAdmin())
	{
		Game* game = new Game(room->getUsers(), room->getQuestionsNo(), this->_db);
		Question* que = game->getQuestion();

		string queMessage = Helper::buildQueMessage(que);

		for (int i = 0; i < room->getUsers().size(); i++)
		{
			room->getUsers()[i]->setGame(game);
			::send(room->getUsers()[i]->getSocket(), queMessage.c_str(), queMessage.length(), 0);
			game->addTurn();
		}
	}
}

void TriviaServer::handlePlayerAnswer(RecievedMessage* message)
{
	User* user = message->getUser();
	Game* game = user->getGame();
	Room* room = user->getRoom();
	Question* que = game->getQuestion();

	int ansIndex = atoi(message->getValues()[0].c_str()) - 1;
	int secondes = atoi(message->getValues()[1].c_str());

	if (ansIndex == que->getCorrectAnswerIndex() && secondes <= room->getQuestionsTime()) // if correct
	{
		game->addCurrect(user->getUsername());
		::send(user->getSocket(), "1201", 4, 0);
	}
	else
		::send(user->getSocket(), "1200", 4, 0);

	que->addAns();

	if (game->getTurn() > room->getQuestionsNo())
	{
		if (que->getAns() == room->getUsers().size())
			this->handleEndGame(room, game);
	}
	else
	{
		if (que->getAns() == room->getUsers().size())
			this->handleNextQuestion(room, game);
	}
}

void TriviaServer::handleNextQuestion(Room* room, Game* game)
{
	vector<User*> users = room->getUsers();
	_getCurrQstID++;

	if (_getCurrQstID == game->GetQuestionNo())
	{
		handleEndGame(room, game);
	}
	else
	{
		game->popQuestion();
		Question* que = game->getQuestion();

		string queMessage = Helper::buildQueMessage(que);


		for (int i = 0; i < users.size(); i++)
			::send(users[i]->getSocket(), queMessage.c_str(), queMessage.length(), 0);



		game->addTurn();
	}
}

void TriviaServer::handleEndGame(Room * room, Game * game)
{
	vector<User*> users = room->getUsers();

	string message = "121";
	message += to_string(users.size());

	for (int i = 0; i < users.size(); i++)
	{
		message += Helper::parseIntTo2Str(users[i]->getUsername().length());
		message += users[i]->getUsername();
		message += Helper::parseIntTo2Str(game->getCurrect(users[i]->getUsername()));
	}

	for (int i = 0; i < users.size(); i++)
	{
		::send(users[i]->getSocket(), message.c_str(), message.length(), 0);

		users[i]->setGame(nullptr);
		users[i]->setRoom(nullptr);
	}

	this->_roomList.erase(room->getId());

	delete game;
	delete room;


	
}

void TriviaServer::handleCreateRoom(RecievedMessage* message)
{
	// create new room
	int roomId = this->_roomIdSequence++;
	User* user = message->getUser();
	string roomName = message->getValues()[0];
	int playersNumber = atoi(message->getValues()[1].c_str());
	int questionsNumber = atoi(message->getValues()[2].c_str());
	int questionTimeInSec = atoi(message->getValues()[3].c_str());
	Room* room = new Room(roomId, user, roomName, playersNumber, questionsNumber, questionTimeInSec);

	user->setRoom(room);

	this->_roomList.insert(pair<int, Room*>(roomId, room)); // append this room

	::send(message->getSock(), "1140", 4, 0); // send succes message
}

void TriviaServer::handleCloseRoom(RecievedMessage* message)
{
	Room* room = message->getUser()->getRoom();
	if (room->getAdmin() == message->getUser())
	{
		// close the room
		room->closeRoom(message->getUser());
		this->_roomList.erase(room->getId());
		delete room;
	}
}

void TriviaServer::handleJoinRoom(RecievedMessage* message)
{
	string sendMessage = "110";

	if (this->_roomList.find(atoi(message->getValues()[0].c_str())) != this->_roomList.end())
	{
		Room* room = this->_roomList.find(atoi(message->getValues()[0].c_str()))->second;
		
		if (room->joinRoom(message->getUser()))
		{
			sendMessage += "0";
			sendMessage += Helper::parseIntTo2Str(room->getQuestionsNo());
			sendMessage += Helper::parseIntTo2Str(room->getQuestionsTime());
			
			// send 108 to all users
			vector<User*> users = room->getUsers();
			for (int i = 0; i < users.size(); i++)
				if(users[i] != message->getUser())
					this->handleGetUsersInRoom(users[i], room);
		}
		else
		{
			sendMessage += "1";
		}
	}
	else
	{
		sendMessage += "2";
	}

	// send the message
	::send(message->getSock(), sendMessage.c_str(), sendMessage.length(), 0);
}

void TriviaServer::handleLeaveRoom(RecievedMessage* message)
{
	Room* room = message->getUser()->getRoom();
	
	if (room->getAdmin() == message->getUser())
	{
		// close the room
		room->closeRoom(message->getUser());
		this->_roomList.erase(room->getId());
		delete room;
	}
	else
	{
		room->leaveRoom(message->getUser());

		// send 108 to all users
		vector<User*> users = room->getUsers();
		for (int i = 0; i < users.size(); i++)
			this->handleGetUsersInRoom(users[i], room);
	}

	for (int i = 0; i < this->_roomList.size(); i++)
		if (this->_roomList[i] == room)
			::send(message->getSock(), "1120", 4, 0);
}

void TriviaServer::handleGetUsersInRoom(RecievedMessage* message)
{
	string sendMessage = "108";
	int roomId = atoi(message->getValues()[0].c_str()); // get room ID

	if (this->_roomList.find(roomId) != this->_roomList.end()) // if te server have this room
	{
		vector<User*> users = this->_roomList.find(roomId)->second->getUsers(); // get the user in this room

		// insert users number
		sendMessage += to_string(users.size());

		// get over the users
		for (int i = 0; i < users.size(); i++)
		{
			// insert user to the message
			sendMessage += Helper::parseIntTo2Str(users[i]->getUsername().length());
			sendMessage += users[i]->getUsername();
		}
	}

	else
		sendMessage += "0";

	// send the message
	::send(message->getSock(), sendMessage.c_str(), sendMessage.length(), 0);

}

void TriviaServer::handleGetUsersInRoom(User* user, Room* room)
{
	string sendMessage = "108";

	vector<User*> users = room->getUsers(); // get the user in this room
																			// insert users number
	sendMessage += to_string(users.size());

	// get over the users
	for (int i = 0; i < users.size(); i++)
	{
		// insert user to the message
		sendMessage += Helper::parseIntTo2Str(users[i]->getUsername().length());
		sendMessage += users[i]->getUsername();
	}

	// send the message
	::send(user->getSocket(), sendMessage.c_str(), sendMessage.length(), 0);
}

void TriviaServer::handleGetRoom(RecievedMessage* message)
{

}

void TriviaServer::hendleGetRoomList(RecievedMessage * message)
{
	string sendMessage = "106";

	// insert rooms number
	sendMessage += Helper::parseIntTo4Str(this->_roomList.size());

	// get over the rooms
	map<int, Room*>::iterator it;
	for (it = this->_roomList.begin(); it != this->_roomList.end(); it++)
	{
		sendMessage += Helper::parseIntTo4Str(it->first); // insert room ID
		sendMessage += Helper::parseIntTo2Str(it->second->getName().length()); // insert room name length
		sendMessage += it->second->getName();
	}

	// send the message
	::send(message->getSock(), sendMessage.c_str(), sendMessage.length(), 0);
}

void TriviaServer::hendleCloseApp(RecievedMessage * message)
{
	User* user = message->getUser();

	if (user->getRoom() != nullptr)
	{
		if (user->getGame() != nullptr)
		{
			this->handleLeaveGame(&RecievedMessage(user->getSocket(), 222));
		}

		this->handleLeaveRoom(&RecievedMessage(user->getSocket(), 211));
	}

	this->_connectedUserd.erase(user->getSocket());
	::closesocket(user->getSocket());
	delete user;
}

void TriviaServer::handleGetBestScores(RecievedMessage* message)
{
	string sendMessage = "124";
	vector<string> best = _db.getBestScores();
	sendMessage += Helper::parseIntTo2Str(best[0].length());
	sendMessage += best[0];
	sendMessage += Helper::parseIntTo6Str(12233);
	sendMessage += Helper::parseIntTo2Str(best[1].length());
	sendMessage += best[1];
	sendMessage += Helper::parseIntTo6Str(10023);
	sendMessage += Helper::parseIntTo2Str(best[2].length());
	sendMessage += best[2];
	sendMessage += Helper::parseIntTo6Str(3230);

	::send(message->getSock(), sendMessage.c_str(), sendMessage.length(), 0);
}

void TriviaServer::handleGetPersonalStatus(RecievedMessage* message)
{
	string sendMessage = "126";
	vector<string> personal = _db.getPersonalStatus(message->getUser()->getUsername());
	sendMessage += Helper::parseIntTo4Str(message->getUser()->getUsername().length());
	sendMessage += message->getUser()->getUsername();
	sendMessage += Helper::parseIntTo6Str(3433);
	sendMessage += Helper::parseIntTo4Str(message->getUser()->getUsername().length());

}

void TriviaServer::handleRecievedMessage()
{
	while (true)
	{
		if (this->_queRcvMessages.size() > 0)
		{
			this->_mtxRecievedMessages.lock();
			RecievedMessage* message = this->_queRcvMessages.front();
			this->_queRcvMessages.pop();
			this->_mtxRecievedMessages.unlock();

			int messageCode = message->getMessageCode();
			switch (messageCode)
			{
			case 201: // sign out request
				this->handleSignOut(message); // working
				break;

			case 205: // room list request
				this->hendleGetRoomList(message); // working
				break;

			case 207: // users in room request
				this->handleGetUsersInRoom(message); // working
				break;

			case 209: // join room request
				this->handleJoinRoom(message); // working
				break;

			case 211: // leave room request
				this->handleLeaveRoom(message); // working
				break;

			case 213: // create room request
				this->handleCreateRoom(message); // working
				break;

			case 215: // close room request
				this->handleCloseRoom(message); // working
				break;

			case 217: // start game request
				this->handleStartGame(message); // working
				break;

			case 219: // client answer
				handlePlayerAnswer(message); // working
				break;

			case 222: // leave game request
				this->handleLeaveGame(message); // working
				break;

			case 223: // best scores request
				handleGetBestScores(message);
				break;

			case 225: // personal status request
				handleGetPersonalStatus(message);
				break;
				
			case 299: // close app request
				this->hendleCloseApp(message); // working
				break;

			default:
				break;

			}

			delete message;
		}
	}
}

void TriviaServer::addRecievedMessage(RecievedMessage* message)
{

}

RecievedMessage* TriviaServer::buildRecievedMessage(SOCKET sock, int num)
{
	return nullptr;
}

User* getUserByName(string userName)
{
	return nullptr;
}

User* getUserBySocket(SOCKET userSocket)
{
	return nullptr;
}

Room* getRoomById(int roomId)
{
	return nullptr;
}

void TriviaServer::handleSignIn(SOCKET sock, string message, bool& notConnected) // not done
{
	// parse userName and password
	string userName, password;
	Helper::getUserNameAndPassword(message, userName, password);

	// cheek user
	bool validUser = this->_db.isUserAndPassMatch(userName, password);

	if (validUser)
	{
		::send(sock, "1020", 4, 0);

		this->_connectedUserd.insert(pair<SOCKET, User*>(sock, new User(userName, sock)));

		notConnected = false;
	}
	else if (!validUser)
	{
		::send(sock, "1021", 4, 0);
	}
}

void TriviaServer::handleSignUp(SOCKET sock, string message) // not done
{
	// parse userName and password
	string userName, password, email;
	Helper::getUserNameAndPasswordAndEmail(message, userName, password, email);

	string retMessage = this->_db.addNewUser(userName, password, email);

	::send(sock, retMessage.c_str(), retMessage.length(), 0);	
}

void TriviaServer::clientTrd(SOCKET sock, User* user)
{
	bool exit = false;

	while (!exit) // while the client didnt left the game
	{
		// get message from client
		char buffer[1024] = { NULL };
		::recv(sock, buffer, 1024, 0);
		cout << buffer << endl;

		// build new message
		if (string(buffer) != "")
		{
			RecievedMessage* message = Helper::parseMessage(sock, string(buffer), exit);
			message->setUser(this->_connectedUserd.find(message->getSock())->second);

			this->_mtxRecievedMessages.lock();
			this->_queRcvMessages.push(message);
			this->_mtxRecievedMessages.unlock();
		}
	}
}

