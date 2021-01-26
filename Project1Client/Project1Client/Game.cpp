#include "Game.h"
#include <iostream>


Game::Game() :
	m_window(sf::VideoMode{ 600, 600 }, "Tag"),
	m_player(Vector2f(10.0f,10.0f),10,Color::Red),
	m_p2(Vector2f(30.0f, 10.0f), 10, Color::Green),
	m_p3(Vector2f(50.0f, 10.0f), 10, Color::Blue),
	myClient("127.0.0.1", 1111)
{
	setupClient();
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
		m_player.handleInput(event);
	}
}

void Game::update(sf::Time t_deltaTime)
{
	m_player.updatePos(t_deltaTime);
	m_p2.update(t_deltaTime);
	m_p3.update(t_deltaTime);

	string msg = "Hello";
	myClient.sendString(msg);
	Sleep(10);
}

void Game::render()
{
	m_window.clear(sf::Color::Black);
	m_player.draw(m_window);
	m_p2.render(m_window);
	m_p3.render(m_window);
	m_window.display();
}

void Game::setupClient()
{
	if (!myClient.Connect())
	{
		cout << "Failed to connect to server" << endl;
	}
}

void Game::sendData()
{
}
