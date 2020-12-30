#pragma once
#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>

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
};

#endif // !GAME