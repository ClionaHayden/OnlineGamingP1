#pragma once
#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>
#include <WinSock2.h>

#include "Client.h"
#include "Player.h"

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
	void setupClient();
	void sendData();

private:
	sf::RenderWindow m_window;
	Player m_player;
	Entity m_p2;
	Entity m_p3;


	Client myClient;
};

#endif // !GAME