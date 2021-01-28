#pragma once
#define NOMINMAX
enum class PacketType
{
	ChatMessage,
	GameUpdate,
	Setup,
	SetupEntity,
	End
};