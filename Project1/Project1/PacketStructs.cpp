#include "PacketStructs.h"

namespace PS
{
	ChatMessage::ChatMessage(const std::string& msg)
		:m_message(msg)
	{
	}

	std::shared_ptr<Packet> ChatMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::ChatMessage);
		p->Append(m_message.size());
		p->Append(m_message);
		return p;
	}

	GameUpdate::GameUpdate(const PlayerUpdate& str)
	{
		m_data = str;
	}

	std::shared_ptr<Packet> GameUpdate::toPacket(PacketType t_type)
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(t_type);
		p->Append(sizeof(m_data));
		p->Append(m_data);
		return p;
	}
}