#pragma once
#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>
#include <WinSock2.h>

#include "Player.h"

class Game
{
public:
	Game(SOCKET& soc);
	Game() = default;
	void run();

private:
	void setupAssets();
	void loadTextures();
	void processEvents();
	void update(sf::Time);
	void render();
	void sendData();

private:
	sf::RenderWindow m_window;
	SOCKET m_connection;
	Player m_player;
	Entity m_p2;
	Entity m_p3;
};

#endif // !GAME