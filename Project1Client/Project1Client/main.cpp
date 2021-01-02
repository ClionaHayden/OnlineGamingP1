/// <summary>
/// Three player tag game using winsock
/// 
/// 
/// @author Cliona Hayden
/// @date May 21017
/// </summary>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <iostream>
#include <WinSock2.h>


#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#pragma comment(lib,"ws2_32.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#pragma comment(lib,"ws2_32.lib") 
#endif 

#include "Game.h"

/// <summary>
/// main enrtry point
/// </summary>
/// <returns>zero</returns>

SOCKET connection;
enum PType
{
	P_ChatMsg
};

bool ProcessPacket(PType t_packetType)
{
	switch (t_packetType)
	{
	case P_ChatMsg:
	{
		int bufferLen;
		recv(connection, (char*)&bufferLen, sizeof(bufferLen), NULL);
		char* buffer = new char[bufferLen + 1];
		buffer[bufferLen] = '\0';
		recv(connection, buffer, bufferLen, NULL);
		std::cout << buffer << std::endl;
		delete[] buffer;
		break;
	}
	default:
		std::cout << "Unregognised packet" << t_packetType << std::endl;
		break;
	}
	return true;
}

void ClientHandlerThread()
{
	PType packetType;
	while (true)
	{
		recv(connection, (char*)&packetType, sizeof(PType), NULL);
		if (!ProcessPacket(packetType))
		{
			break;
		}
	}
	closesocket(connection);
}
 
int main()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "Winsock startup failed" << std::endl;
	}

	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//InetPton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(connection, (SOCKADDR*)&addr, addrlen) != 0)
	{
		std::cout << "Failed to accept client connection" << std::endl;
	}
	else
	{
		std::cout << "Client connected" << std::endl;
	}
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, NULL, NULL, NULL);
	
	while (true)
	{
		std::string msg = "send back";
		int bln = msg.size();
		PType packetType = P_ChatMsg;
		send(connection, (char*)&packetType, sizeof(PType), NULL);
		send(connection, (char*)&bln, sizeof(int), NULL);
		send(connection, msg.c_str(), bln, NULL);
		Sleep(10);
	}


	Game game(connection);
	game.run();

	return 1;
}