#include "Client.h"

Client::Client(string IP, int port)
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "Winsock startup failed" << std::endl;
	}

	addr.sin_addr.s_addr = inet_addr(IP.c_str());
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	clientPtr = this;
}

Client::~Client()
{
	delete clientPtr;
}

bool Client::Connect()
{
	connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(connection, (SOCKADDR*)&addr, addrSize) != 0)
	{
		std::cout << "Failed to accept client connection" << std::endl;
		return false;
	}
	else
	{
		std::cout << "Client connected" << std::endl;
	}
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientThread, NULL, NULL, NULL);
	return true;
}

bool Client::closeConnection()
{
	if (closesocket(connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;
		string errorMsg = "Failed to close socket. Winsock error: " + to_string(WSAGetLastError());
		cout << errorMsg << endl;
		return false;
	}
	return true;
}

bool Client::processPacket(PType t_type)
{
	switch (t_type)
	{
	case P_ChatMsg:
	{
		std::string msg;
		if (!getString(msg))
			return false;
		std::cout << msg << std::endl;
		break;
	}
	default:
		std::cout << "Unregognised packet" << t_type << std::endl;
		break;
	}
	return true;
}

void Client::clientThread()
{
	PType packetType = PType::P_ChatMsg;
	while (true)
	{
		if (!clientPtr->getPacketType(packetType))
		{
			break;
		}
		if (!clientPtr->processPacket(packetType))
		{
			break;
		}
	}
	std::cout << "Lost connection to server" << std::endl;
	if (clientPtr->closeConnection())
	{
		cout << "Socket closed" << endl;
	}
	else
	{
		cout << "Socket not able to be closed" << endl;
	}
}

bool Client::sendInt32_t(int32_t t_int)
{
	t_int = htonl(t_int);
	if (!sendAll((char*)&t_int, sizeof(int32_t)))
	{
		return false;
	}
	return true;
}

bool Client::getInt32_t(int32_t& t_int)
{
	if (!recvAll((char*)&t_int, sizeof(int32_t)))
	{
		return false;
	}
	t_int = ntohl(t_int);
	return true;
}

bool Client::sendPacketType(PType t_packetType)
{
	if (!sendInt32_t(t_packetType))
	{
		return false;
	}
	return true;
}

bool Client::sendAll(char* t_data, int t_totalBytes)
{
	int bytesSent = 0;
	while (bytesSent < t_totalBytes)
	{
		int retnCheck = send(connection, t_data + bytesSent, t_totalBytes - bytesSent, NULL);
		if (retnCheck == SOCKET_ERROR)
		{
			return false;
		}
		bytesSent += retnCheck;
	}
	return true;
}

bool Client::getPacketType(PType& t_packetType)
{
	int packetType;
	if (!getInt32_t(packetType))
	{
		return false;
	}
	t_packetType = (PType)packetType;
	return true;
}

bool Client::getString(string& t_string)
{
	int32_t bufLen = t_string.size();
	if (!getInt32_t(bufLen))
		return false;
	char* buffer = new char[bufLen + 1];
	buffer[bufLen] = '\0';
	if (!recvAll(buffer, bufLen))
	{
		delete[] buffer;
		return false;
	}
	t_string = buffer;
	delete[] buffer;
	return true;
}

bool Client::recvAll(char* t_data, int t_totalBytes)
{
	int bytesRecv = 0;
	while (bytesRecv < t_totalBytes)
	{
		int retnCheck = recv(connection, t_data + bytesRecv, t_totalBytes - bytesRecv, NULL);
		if (retnCheck == SOCKET_ERROR)
		{
			return false;
		}
		bytesRecv += retnCheck;
	}
	return true;
}

bool Client::sendString(string& t_string)
{
	if (!sendPacketType(P_ChatMsg))
		return false;
	int32_t bufLen = t_string.size();
	if (!sendInt32_t(bufLen))
		return false;
	if (!sendAll((char*)t_string.c_str(), bufLen))
	{
		return false;
	}
	return true;
}

