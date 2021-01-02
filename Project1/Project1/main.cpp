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
#include <WS2tcpip.h>


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
 
SOCKET connections[3];
int connectionCounter = 0;

enum PType
{
	P_ChatMsg
};

bool ProcessPacket(int ID, PType t_packetType)
{
	switch (t_packetType)
	{
	case P_ChatMsg:
	{
		int bufferLen;
		recv(connections[ID], (char*)&bufferLen, sizeof(bufferLen), NULL);
		char* buffer = new char[bufferLen];
		recv(connections[ID], buffer, bufferLen, NULL);
		for (int i = 0; i < connectionCounter; i++)
		{
			if (i = ID)
				continue;
			PType chatPacket = P_ChatMsg;
			send(connections[i], (char*)&chatPacket, sizeof(PType), NULL);
			send(connections[i], (char*)&bufferLen, sizeof(bufferLen), NULL);
			send(connections[i], buffer, bufferLen, NULL);
		}
		delete[] buffer;
		break;
	}
	default:
		std::cout << "Unregognised packet" << t_packetType << std::endl;
		break;
	}
	return true;
}

void ClientHandlerThread(int ID)
{
	PType packetType;
	while (true)
	{
		recv(connections[ID], (char*)&packetType, sizeof(PType), NULL);
		if (!ProcessPacket(ID, packetType))
		{
			break;
		}
	}
	closesocket(connections[ID]);
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

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 3; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
		if (newConnection == 0)
		{
			std::cout << "Failed to accept client connection" << std::endl;
		}
		else
		{
			std::cout << "Client connected" << std::endl;
			connections[i] = newConnection;
			connectionCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);
			PType chatPacket = P_ChatMsg;
			send(connections[i], (char*)&chatPacket, sizeof(PType), NULL);
			std::string msg = "send";
			int msgLen = msg.size();
			send(newConnection, (char*)&msgLen, sizeof(int32_t), NULL);
			send(newConnection, msg.c_str(), msgLen, NULL);
		}
	}
	Game game;
	game.run();

	return 1;
}