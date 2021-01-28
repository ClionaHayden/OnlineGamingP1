#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <shared_mutex>
#include <WS2tcpip.h>

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


class Client
{
public:
	Client(string IP, int port, Game& t_game);
	~Client();
	bool Connect();
	bool closeConnection();
	bool sendString(string& t_string);
	std::shared_ptr<Connection> connection;
private:
	SOCKADDR_IN addr;
	int addrSize = sizeof(addr);
	bool m_terminateThreads;
	Game& m_game;

	std::vector<std::thread*> m_threads;
	shared_mutex m_connectionManagerMutex;

	bool processPacket(PacketType t_type);
	static void clientThread(Client& t_client);
	static void packetSenderThread(Client& t_client);

	//sending
	bool sendInt32_t(int32_t t_int);
	bool sendPacketType(PacketType t_packetType);
	bool sendAll(char* t_data, int t_totalBytes);

	//recieving
	bool getInt32_t(int32_t& t_int);
	bool getPacketType(PacketType& t_packetType);
	bool getString(string& t_string);
	bool recvAll(char* t_data, int t_totalBytes);
	bool getPlayerUpdate(PlayerUpdate& t_update);
};

#include "Game.h"