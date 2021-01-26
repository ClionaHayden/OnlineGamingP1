#include "Game.h"
#include <iostream>


Game::Game() :
	m_window(sf::VideoMode{ 600, 600 }, "Tag"),
	m_server(1111)
{
	m_server.ListenForNewConnection();
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / 60.f);
	while (m_window.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}
		render();
	}
}

void Game::setupAssets()
{
	
}

void Game::loadTextures()
{

}

void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		
	}
}

void Game::update(sf::Time t_deltaTime)
{
	
}

void Game::render()
{
	m_window.clear(sf::Color::Black);
	m_window.display();
}