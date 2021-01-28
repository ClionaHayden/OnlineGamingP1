#pragma once
#define NOMINMAX
#include <string>
enum class PacketType
{
	ChatMessage,
	GameUpdate,
	Setup,
	SetupEntity,
	End
};