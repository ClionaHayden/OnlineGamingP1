#include "Server.h"

Server::Server(int port, bool broadcastPublically)
{
    connectionCounter = 0;
    WSAData wsaData;
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaData) != 0)
    {
        std::cout << "Winsock startup failed" << std::endl;
    }

    if (broadcastPublically)
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    sListen = socket(AF_INET, SOCK_STREAM, NULL);
    if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        string errorMsg = "Failed to close socket. Winsock error: ";
        cout << errorMsg  << "Error" << (MB_OK | MB_ICONERROR) << endl;
    }
    if (listen(sListen, SOMAXCONN) == SOCKET_ERROR)
    {
        string errorMsg = "Failed to close socket. Winsock error: ";
        cout << errorMsg << "Error" << (MB_OK | MB_ICONERROR) << endl;
    }

    serverPtr = this;
}

Server::~Server()
{
    delete serverPtr;
}

bool Server::ListenForNewConnection()
{
    SOCKET newConnection;
    for (int i = 0; i < 3; i++)
    {
        newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
        if (newConnection == 0)
        {
            std::cout << "Failed to accept client connection" << std::endl;
            return false;
        }
        else
        {
            std::cout << "Client connected" << std::endl;
            connections[i] = newConnection;
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);
            std::string msg = "send";
            sendString(i, msg);
            connectionCounter++;
            return true;
        }
    }
}

bool Server::ProcessPacket(int ID, PType t_packetType)
{
    switch (t_packetType)
    {
    case P_ChatMsg:
    {
        std::string msg;
        if (!getString(ID, msg))
            return false;
        for (int i = 0; i < connectionCounter; i++)
        {
            if (i = ID)
                continue;
            if (!sendString(i, msg))
            {
                std::cout << "Failed to send message from ID: " << ID << " to client ID: " << i << std::endl;
            }
            std::cout << "Processed chat message from user ID: " << ID << std::endl;
            break;
        }
        break;
    }
    default:
        std::cout << "Unregognised packet" << t_packetType << std::endl;
        break;
    }
    return true;
}

void Server::ClientHandlerThread(int ID)
{
    PType packetType = PType::P_ChatMsg;
    while (true)
    {
        if (!serverPtr->getPacketType(ID, packetType))
            break;
        if (!serverPtr->ProcessPacket(ID, packetType))
        {
            break;
        }
    }
    std::cout << "Lost connection to server" << std::endl;
    closesocket(serverPtr->connections[ID]);
}

bool Server::sendInt32_t(int ID, int32_t t_int)
{
    t_int = htonl(t_int);
    if (!sendAll(ID, (char*)&t_int, sizeof(int32_t)))
    {
        return false;
    }
    return true;
}

bool Server::sendPacketType(int ID, PType t_packetType)
{
    if (!sendInt32_t(ID,t_packetType))
    {
        return false;
    }
    return true;
}

bool Server::sendString(int ID, string& t_string)
{
    if (!sendPacketType(ID, P_ChatMsg))
        return false;
    int32_t bufLen = t_string.size();
    if (!sendInt32_t(ID, bufLen))
        return false;
    if (!sendAll(ID, (char*)t_string.c_str(), bufLen))
    {
        return false;
    }
    return true;
}

bool Server::sendAll(int ID, char* t_data, int t_totalBytes)
{
    int bytesSent = 0;
    while (bytesSent < t_totalBytes)
    {
        int retnCheck = send(connections[ID], t_data + bytesSent, t_totalBytes - bytesSent, NULL);
        if (retnCheck == SOCKET_ERROR)
        {
            return false;
        }
        bytesSent += retnCheck;
    }
    return true;
}

bool Server::getInt32_t(int ID, int32_t& t_int)
{
    if (!recvAll(ID, (char*)&t_int, sizeof(int32_t)))
    {
        return false;
    }
    t_int = ntohl(t_int);
    return true;
}

bool Server::getPacketType(int ID, PType& t_packetType)
{
    int packetType;
    if (!getInt32_t(ID,packetType))
    {
        return false;
    }
    t_packetType = (PType)packetType;
    return true;
}

bool Server::getString(int ID, string& t_string)
{
    int32_t bufLen = t_string.size();
    if (!getInt32_t(ID, bufLen))
        return false;
    char* buffer = new char[bufLen + 1];
    buffer[bufLen] = '\0';
    if (!recvAll(ID, buffer, bufLen))
    {
        delete[] buffer;
        return false;
    }
    t_string = buffer;
    delete[] buffer;
    return true;
}

bool Server::recvAll(int ID, char* t_data, int t_totalBytes)
{
    int bytesRecv = 0;
    while (bytesRecv < t_totalBytes)
    {
        int retnCheck = recv(connections[ID], t_data + bytesRecv, t_totalBytes - bytesRecv, NULL);
        if (retnCheck == SOCKET_ERROR)
        {
            return false;
        }
        bytesRecv += retnCheck;
    }
    return true;
}
