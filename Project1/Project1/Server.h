#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

enum PType
{
	P_ChatMsg
};

class Server
{
public:
	Server(int port, bool broadcastPublically = false);
	~Server();
	bool ListenForNewConnection();
private:
	SOCKET connections[3];
	int connectionCounter = 0;
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	SOCKET sListen;


	//sending
	bool sendInt32_t(int ID, int32_t t_int);
	bool sendPacketType(int ID, PType t_packetType);
	bool sendString(int ID, string& t_string);
	bool sendAll(int ID, char* t_data, int t_totalBytes);

	//recieving
	bool getInt32_t(int ID, int32_t& t_int);
	bool getPacketType(int ID, PType& t_packetType);
	bool getString(int ID, string& t_string);
	bool recvAll(int ID, char* t_data, int t_totalBytes);

	bool ProcessPacket(int ID, PType t_packetType);
	static void ClientHandlerThread(int ID);

};

static Server* serverPtr;