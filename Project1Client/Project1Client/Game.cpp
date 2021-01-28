#include "Game.h"
#include <iostream>


Game::Game() :
	m_window(sf::VideoMode{ 600, 600 }, "Tag Client"),
	m_player(Vector2f(10.0f,10.0f),10,Color::Red),
	m_p2(Vector2f(30.0f, 10.0f), 10, Color::Green),
	m_p3(Vector2f(50.0f, 10.0f), 10, Color::Blue),
	myClient("127.0.0.1", 1111, ref(*this))
{
	setupAssets();
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
	m_waitText = make_shared<Text>();
	if (!m_font.loadFromFile("ASSETS/font.ttf"))
	{
		cout << "Unable to load font" << endl;
	}
	m_waitText->setFont(m_font);
	m_waitText->setFillColor(Color::White);
	m_waitText->setString("Waiting for players");
	m_waitText->setPosition(Vector2f(200.0f, 100.0f));
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
	if (gamestate == GameState::Close)
	{
		m_waitText->setString("Game Over");
	}
	else
	{
		m_player.updatePos(t_deltaTime);
		m_p2.update(t_deltaTime);
		m_p3.update(t_deltaTime);
		//setupEntities();
		string msg = "Hello";
		//myClient.sendString(msg);
		PlayerUpdate temp;
		temp.ID = m_player.getID();
		temp.pos = m_player.updatePos(t_deltaTime);
		temp.col = m_player.getFillColour();
		PS::GameUpdate gu(temp);
		myClient.connection->m_pm.Append(gu.toPacket(PacketType::GameUpdate));
		Sleep(10);
	}
}

void Game::render()
{
	m_window.clear(sf::Color::Black);
	if (gamestate == GameState::Close)
	{
		m_window.draw(*m_waitText);
	}
	else
	{
		m_player.draw(m_window);
		m_p2.render(m_window);
		m_p3.render(m_window);
	}
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

void Game::setPlayerData(int ID, Vector2f t_pos, Color t_col)
{
	if (ID == m_player.getID())
	{
		m_player.setPosition(t_pos);
		m_player.setColour(t_col);
	}
	else if (ID == m_p2.m_ID)
	{
		m_p2.m_pos = t_pos;
		m_p2.setFillColour(t_col);
	}
	else if (ID == m_p3.m_ID)
	{
		m_p3.m_pos = t_pos;
		m_p3.setFillColour(t_col);
	}
}

void Game::setupEntities()
{
	if (myClient.connection->m_ID == 1)
	{
		m_player.setColour(Color::Red);
		m_p2.setFillColour(Color::Blue);
		m_p3.setFillColour(Color::Green);
	}
	else if (myClient.connection->m_ID == 2)
	{
		m_player.setColour(Color::Blue);
		m_p2.setFillColour(Color::Red);
		m_p3.setFillColour(Color::Green);
	}
	else if (myClient.connection->m_ID == 0)
	{
		m_player.setColour(Color::Green);
		m_p2.setFillColour(Color::Red);
		m_p3.setFillColour(Color::Blue);
	}
}
