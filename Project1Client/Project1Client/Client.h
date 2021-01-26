#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>

using namespace std;

enum PType
{
	P_ChatMsg
};

class Client
{
public:
	Client(string IP, int port);
	~Client();
	bool Connect();
	bool closeConnection();
	bool sendString(string& t_string);
private:
	SOCKET connection;
	SOCKADDR_IN addr;
	int addrSize = sizeof(addr);

	bool processPacket(PType t_type);
	static void clientThread();

	//sending
	bool sendInt32_t(int32_t t_int);
	bool sendPacketType(PType t_packetType);
	bool sendAll(char* t_data, int t_totalBytes);

	//recieving
	bool getInt32_t(int32_t& t_int);
	bool getPacketType(PType& t_packetType);
	bool getString(string& t_string);
	bool recvAll(char* t_data, int t_totalBytes);
};

static Client* clientPtr;