#pragma once
#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>
#include "Server.h"

class Game
{
public:
	Game();
	void run();

private:
	void setupAssets();
	void loadTextures();
	void processEvents();
	void update(sf::Time);
	void render();

private:
	sf::RenderWindow m_window;

	Server m_server;
};

#endif // !GAME