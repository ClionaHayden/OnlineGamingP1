#include "Client.h"

Client::Client(string IP, int port, Game& t_game):
	m_terminateThreads(false),
	m_game(t_game)
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
}

Client::~Client()
{
	m_terminateThreads = true;
	for (std::thread* t : m_threads) //Wait for all created threads to end...
	{
		t->join();
	}
}

bool Client::Connect()
{
	connection = make_shared<Connection>(socket(AF_INET, SOCK_STREAM, NULL));
	if (connect(connection->m_socket, (SOCKADDR*)&addr, addrSize) != 0)
	{
		std::cout << "Failed to accept client connection" << std::endl;
		return false;
	}
	else
	{
		std::cout << "Client connected" << std::endl;
	}
	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientThread, NULL, NULL, NULL);
	std::thread CT(clientThread, std::ref(*this));
	CT.detach();
	m_threads.push_back(&CT);

	std::thread PST(packetSenderThread, std::ref(*this));
	PST.detach();
	m_threads.push_back(&PST);
	return true;
}

bool Client::closeConnection()
{
	lock_guard<shared_mutex> lock(m_connectionManagerMutex);

	connection->m_pm.Clear();
	closesocket(connection->m_socket);
	std::cout << "Closed client: " << connection->m_ID << "." << std::endl;
	return true;
}

bool Client::processPacket(PacketType t_type)
{
	switch (t_type)
	{
	case PacketType::GameUpdate:
	{
		PlayerUpdate temp;
		getPlayerUpdate(temp);
		m_game.setPlayerData(temp.ID, temp.pos, temp.col);
		//cout << temp.pos.x << ", " << temp.pos.y << endl;
		break;
	}
	case PacketType::Setup:
	{
		PlayerUpdate temp;
		getPlayerUpdate(temp);
		connection->m_ID = temp.ID;
		m_game.setID(temp.ID);
		m_game.setPlayerData(temp.ID, temp.pos, temp.col);

		PS::GameUpdate gu(temp);
		connection->m_pm.Append(gu.toPacket(PacketType::SetupEntity));
		break;
	}
	case PacketType::SetupEntity:
	{
		PlayerUpdate temp;
		getPlayerUpdate(temp);
		if (m_game.getPlayer().getID() == 0)
		{
			m_game.getP2().m_ID = 1;
			m_game.getP3().m_ID = 2;
		}
		else if (m_game.getPlayer().getID() == 1)
		{
			m_game.getP2().m_ID = 0;
			m_game.getP3().m_ID = 2;
		}
		else if (m_game.getPlayer().getID() == 2)
		{
			m_game.getP2().m_ID = 0;
			m_game.getP3().m_ID = 1;
		}
		break;
	}
	case PacketType::End:
	{
		PlayerUpdate temp;
		getPlayerUpdate(temp);
		m_game.m_loserID = temp.IDLose;
		m_game.gamestate = GameState::Close;
	}
	case PacketType::ChatMessage:
	{
		std::string msg;
		if (!getString(msg))
			return false;
		std::cout << msg << std::endl;
		break;
	}
	default:
		std::cout << "Unregognised packet"<< std::endl;
		break;
	}
	return true;
}

void Client::clientThread(Client& t_client)
{
	PacketType packetType = PacketType::GameUpdate;
	while (true)
	{
		if (t_client.m_terminateThreads) break;

		if (!t_client.getPacketType(packetType)) break;

		if (!t_client.processPacket(packetType)) break;
	}
	cout << "Lost connection to the server" << endl;

	t_client.closeConnection();
}

void Client::packetSenderThread(Client& t_client)
{
	while (true)
	{
		if (t_client.m_terminateThreads == true)
			break;
		if (t_client.connection->m_pm.HasPendingPackets()) //If there are pending packets for this connection's packet manager
		{
			std::shared_ptr<Packet> p = t_client.connection->m_pm.Retrieve(); //Retrieve packet from packet manager
			if (!t_client.sendAll((char*)(&p->m_buffer[0]), p->m_buffer.size())) //send packet to connection
			{
				std::cout << "Failed to send packet to ID: " << t_client.connection->m_ID << std::endl; //Print out if failed to send packet
				t_client.closeConnection();
				break;
			}
		}
		Sleep(5);
	}
	std::cout << "Ending Packet Sender Thread..." << std::endl;
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

bool Client::sendPacketType(PacketType t_packetType)
{
	if (!sendInt32_t((int32_t)t_packetType))
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
		int retnCheck = send(connection->m_socket, t_data + bytesSent, t_totalBytes - bytesSent, NULL);
		if (retnCheck == SOCKET_ERROR)
		{
			return false;
		}
		bytesSent += retnCheck;
	}
	return true;
}

bool Client::getPacketType(PacketType& t_packetType)
{
	int packetType;
	if (!getInt32_t(packetType))
	{
		return false;
	}
	t_packetType = (PacketType)packetType;
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
		int retnCheck = recv(connection->m_socket, t_data + bytesRecv, t_totalBytes - bytesRecv, NULL);
		if (retnCheck == SOCKET_ERROR)
		{
			return false;
		}
		bytesRecv += retnCheck;
	}
	return true;
}

bool Client::getPlayerUpdate(PlayerUpdate& t_update)
{
	int32_t bufLen;
	if (!getInt32_t(bufLen))
		return false;
	if (bufLen == 0)
		return false;
	return recvAll((char*)&t_update, bufLen);
}

bool Client::sendString(string& t_string)
{
	if (!sendPacketType(PacketType::ChatMessage))
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

