#pragma once
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <shared_mutex>

#include "PacketType.h"
#include "PacketStructs.h"
#include "PacketManager.h"

using namespace std;

class Game;

class Connection
{
public:
	Connection(SOCKET socket)
		:m_socket(socket)
	{
	}
	SOCKET m_socket;
	PacketManager m_pm; //Packet Manager for outgoing data for this connection
	int m_ID = 0;
};

class Server
{
public:
	Server(int port, Game& t_game, bool broadcastPublically = false);
	~Server();
	static bool ListenForNewConnection(Server& t_server);
	std::vector<std::shared_ptr<Connection>> m_connections;

	void setupGame();

	bool ProcessPacket(int ID, PacketType t_packetType);
	static void ClientHandlerThread(int ID, Server& t_s);
	static void packetSenderThread(Server& t_s);
	void endGame();
	int connectionCounter = 0;
private:
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	SOCKET sListen;
	shared_mutex m_connectionManagerMutex;
	bool m_terminateThreads;

	std::vector<std::thread*> m_threads;

	Game& m_game;

	//sending
	bool sendInt32_t(int ID, int32_t t_int);
	bool sendPacketType(int ID, PacketType t_packetType);
	bool sendString(int ID, string& t_string);
	bool sendAll(int ID, char* t_data, int t_totalBytes);

	//recieving
	bool getInt32_t(int ID, int32_t& t_int);
	bool getPacketType(int ID, PacketType& t_packetType);
	bool getString(int ID, string& t_string);
	bool recvAll(int ID, char* t_data, int t_totalBytes);
	bool getPlayerUpdate(int ID, PlayerUpdate& t_update);


};

#include "Game.h"