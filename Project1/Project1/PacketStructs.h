#pragma once
#define NOMINMAX
#include "Data.h"
#include "PacketType.h"
#include "Packet.h"
#include <string> //for std::string
#include <memory> //for std::shared_ptr

namespace PS //Packet Structures Namespace
{
	class ChatMessage
	{
	public:
		ChatMessage(const std::string& str);
		std::shared_ptr<Packet> toPacket(); //Converts ChatMessage to packet
	private:
		std::string m_message;
	};

	class GameUpdate
	{
	public:
		GameUpdate(const PlayerUpdate& str);
		std::shared_ptr<Packet> toPacket(PacketType t_type); //Converts ChatMessage to packet
	private:
		PlayerUpdate m_data;
	};
}