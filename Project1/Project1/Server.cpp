#include "Server.h"

Server::Server(int port, Game& t_game, bool broadcastPublically):
    m_game(t_game)
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
    std::thread PST(packetSenderThread, ref(*this));
    PST.detach();
    m_threads.push_back(&PST);
}

Server::~Server()
{
    m_terminateThreads = true;
    for (std::thread* t : m_threads) //Wait for all created threads to end...
    {
        t->join();
    }
}

bool Server::ListenForNewConnection(Server& t_server)
{
    //for (int i = 0; i < 2; i++)
    while(true)
    {
        SOCKET ConnectionNew = accept(t_server.sListen, (SOCKADDR*)&t_server.addr, &t_server.addrlen);
        if (ConnectionNew == 0)
        {
            std::cout << "Failed to accept client connection" << std::endl;
            return false;
        }
        else
        {
            std::cout << "Client connected" << std::endl;
            std::shared_ptr<Connection> newConnection(std::make_shared<Connection>(ConnectionNew));
            newConnection->m_ID = t_server.connectionCounter;
            t_server.m_connections.push_back(newConnection);
            std::thread CHT(t_server.ClientHandlerThread, newConnection->m_ID, ref(t_server));
            CHT.detach();
            t_server.m_threads.push_back(&CHT);
           /* PlayerUpdate temp;
            temp.ID = t_server.connectionCounter;
            PS::GameUpdate gu(temp);
            newConnection->m_pm.Append(gu.toPacket(PacketType::Setup));*/
            //CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);
            std::string msg = "send";
            //sendString(i, msg);
            t_server.connectionCounter++;
        }
    }
    return true;
}

void Server::setupGame()
{
    for (int i = 0; i < m_connections.size(); i++)
    {
        PlayerUpdate temp;
        temp.ID = i + 1;
        temp.pos = Vector2f(100.0f * temp.ID, 20.0f);
        if (temp.ID == 0)
        {
            temp.col = sf::Color::Red;
        }
        else if (temp.ID == 1)
        {
            temp.col = sf::Color::Blue;
        }
        else if (temp.ID == 2)
        {
            temp.col = sf::Color::Green;
        }
        PS::GameUpdate gu(temp);
        m_connections[i]->m_pm.Append(gu.toPacket(PacketType::Setup));
    }
}

bool Server::ProcessPacket(int ID, PacketType t_packetType)
{
    switch (t_packetType)
    {
    case PacketType::GameUpdate:
    {
        PlayerUpdate temp;
        if(!getPlayerUpdate(ID, temp))
        {
            std::cout << "Failed to send message from ID: " << ID << " to client ID: " << ID << std::endl;
        }
        m_game.setPlayerData(temp.ID, temp.pos, temp.col);
        PS::GameUpdate gameUpdate(temp);
        std::shared_ptr<Packet> pkt = std::make_shared<Packet>(gameUpdate.toPacket(PacketType::GameUpdate));
        {
            std::shared_lock<std::shared_mutex> lock(m_connectionManagerMutex);
            for (shared_ptr<Connection> con : m_connections) //For each connection...
            {
                if (con != m_connections.at(ID))
                {
                    con->m_pm.Append(pkt);
                    temp.ID = m_game.getPlayer().getID();
                    temp.pos = m_game.getPlayer().getPosition();
                    temp.col = m_game.getPlayer().getFillColour();
                    PS::GameUpdate gu(temp);
                    pkt = std::make_shared<Packet>(gu.toPacket(PacketType::GameUpdate));
                    con->m_pm.Append(pkt);
                    std::cout << "Processed packet message from user ID: " << ID << std::endl;
                }
            }
        }

        break;
    }
    case PacketType::SetupEntity:
    {
        PlayerUpdate temp;
        if (!getPlayerUpdate(ID, temp))
        {
            std::cout << "Failed to send message from ID: " << ID << " to client ID: " << ID << std::endl;
        }
        PS::GameUpdate gameUpdate(temp);
        std::shared_ptr<Packet> pkt = std::make_shared<Packet>(gameUpdate.toPacket(PacketType::SetupEntity));
        {
            std::shared_lock<std::shared_mutex> lock(m_connectionManagerMutex);
            for (shared_ptr<Connection> con : m_connections) //For each connection...
            {
                if (con != m_connections.at(ID))
                {
                    con->m_pm.Append(pkt);
                    std::cout << "Processed packet message from user ID: " << ID << std::endl;
                }
            }
        }
        break;
    }
    case PacketType::End:
    {
        break;
    }
    case PacketType::ChatMessage:
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
        std::cout << "Unregognised packet"  << std::endl;
        break;
    }
    return true;
}

void Server::ClientHandlerThread(int ID, Server& t_s)
{
    PacketType packettype;
    while (true)
    {
        if (t_s.m_terminateThreads) break;

        if (!t_s.getPacketType(t_s.m_connections.at(ID)->m_ID, packettype)) break;

        if (!t_s.ProcessPacket(t_s.m_connections.at(ID)->m_ID, packettype)) break;
    }
    //close connection
    t_s.connectionCounter--;
    std::cout << "Lost connection to client ID: " << t_s.m_connections.at(ID)->m_ID << std::endl;
    return;
}

void Server::packetSenderThread(Server& t_s)
{
    while (true)
    {
        if (t_s.m_terminateThreads == true)
            break;
        shared_lock<shared_mutex> lock(t_s.m_connectionManagerMutex);
        for (shared_ptr<Connection> con : t_s.m_connections) //for each connection...
        {
            if (con->m_pm.HasPendingPackets()) //If there are pending packets for this connection's packet manager
            {
                std::shared_ptr<Packet> p = con->m_pm.Retrieve(); //Retrieve packet from packet manager
                if (!t_s.sendAll(con->m_ID,(char*)(&p->m_buffer[0]), p->m_buffer.size())) //send packet to connection
                {
                    std::cout << "Failed to send packet to ID: " << con->m_ID << std::endl; //Print out if failed to send packet
                   // t_s.(con);
                    break;
                }
            }
        }
        Sleep(5);
    }
    std::cout << "Ending Packet Sender Thread..." << std::endl;
}

void Server::endGame()
{
    PlayerUpdate temp;
    PS::GameUpdate gameUpdate(temp);
    temp.IDLose = m_game.m_loserID;
    std::shared_ptr<Packet> pkt = std::make_shared<Packet>(gameUpdate.toPacket(PacketType::End));
    {
        std::shared_lock<std::shared_mutex> lock(m_connectionManagerMutex);
        for (shared_ptr<Connection> con : m_connections) //For each connection...
        {
            con->m_pm.Append(pkt);
        }
    }
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

bool Server::sendPacketType(int ID, PacketType t_packetType)
{
    if (!sendInt32_t(ID,(int32_t)t_packetType))
    {
        return false;
    }
    return true;
}

bool Server::sendString(int ID, string& t_string)
{
    if (!sendPacketType(ID, PacketType::ChatMessage))
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
        int retnCheck = send(m_connections.at(ID)->m_socket, t_data + bytesSent, t_totalBytes - bytesSent, NULL);
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

bool Server::getPacketType(int ID, PacketType& t_packetType)
{
    int packetType;
    if (!getInt32_t(ID,packetType))
    {
        return false;
    }
    t_packetType = (PacketType)packetType;
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
        int retnCheck = recv(m_connections.at(ID)->m_socket, t_data + bytesRecv, t_totalBytes - bytesRecv, NULL);
        if (retnCheck == SOCKET_ERROR)
        {
            return false;
        }
        bytesRecv += retnCheck;
    }
    return true;
}

bool Server::getPlayerUpdate(int ID, PlayerUpdate& t_update)
{
    int32_t bufLen;
    if (!getInt32_t(ID, bufLen))
        return false;
    if (bufLen == 0)
        return false;
    return recvAll(ID, (char*)&t_update, bufLen);
}
