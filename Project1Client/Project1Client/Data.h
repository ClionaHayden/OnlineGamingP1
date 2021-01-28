#pragma once
#define NOMINMAX
#include <SFML/Graphics.hpp>
using namespace sf;

struct PlayerUpdate
{
	int ID;
	Vector2f pos;
	Color col;
	int IDLose;
};
enum class GameState
{
	Setup,
	Initialise,
	Play,
	Close
};