#include "Game.h"
#include <iostream>


Game::Game() :
	m_window(sf::VideoMode{ 600, 600 }, "Tag Host"),
	m_player(Vector2f(10.0f, 70.0f), 10, Color::Red),
	m_p2(Vector2f(30.0f, 110.0f), 10, Color::Green),
	m_p3(Vector2f(50.0f, 180.0f), 10, Color::Blue),
	m_server(1111, *this)
{
	setupAssets();
	std::thread lc(m_server.ListenForNewConnection, ref(m_server));
	lc.detach();
	m_lc = &lc;
	//m_server.ListenForNewConnection();
}

Game::~Game()
{
	delete m_lc;
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
	if (m_server.m_connections.size() == 2 && gamestate == GameState::Setup)
	{
		gamestate = GameState::Initialise;
	}
	/*else if(m_server.m_connections.size() == 2 && gamestate == GameState::Close)
	{
		gamestate = GameState::Setup;
	}*/
	if (gamestate == GameState::Initialise)
	{
		m_server.setupGame();
		m_player.setID(0);
		m_p2.m_ID = 1;
		m_p3.m_ID = 2;
		gamestate = GameState::Play;
	}
	if (gamestate == GameState::Play)
	{
		m_player.updatePos(t_deltaTime);
		m_p2.update(t_deltaTime);
		m_p3.update(t_deltaTime);
		checkGameOver();
	}
	if (gamestate == GameState::Close)
	{
		m_server.endGame();
		m_waitText->setString("Game Over");
	}
}

void Game::render()
{
	m_window.clear(sf::Color::Black);
	if (gamestate == GameState::Setup || gamestate == GameState::Close)
	{
		m_window.draw(*m_waitText);
	}
	else if (gamestate == GameState::Play)
	{
		m_player.draw(m_window);
		m_p2.render(m_window);
		m_p3.render(m_window);
	}
	m_window.display();
}

void Game::setupEntities()
{
	m_player.setColour(Color::Green);
	m_p2.setFillColour(Color::Red);
	m_p3.setFillColour(Color::Blue);
}

void Game::checkGameOver()
{
	if (m_player.getFillColour() == Color::Red || 
		m_player.getFillColour() == Color::Green)
	{
		if (m_p2.getColor() == Color::Blue)
		{
			if (circleCollision(m_player.getPosition(), 10, m_p2.getPos(), 10) ||
				circleCollision(m_p3.getPos(), 10, m_p2.getPos(), 10))
			{
				m_p3.setWin(true);
				m_player.setGameWin(true);
				m_loserID = m_p2.m_ID;
				gamestate = GameState::Close;
			}
		}
		else if (m_p3.getColor() == Color::Blue)
		{
			if (circleCollision(m_player.getPosition(), 10, m_p3.getPos(), 10) ||
				circleCollision(m_p3.getPos(), 10, m_p2.getPos(), 10))
			{
				m_p2.setWin(true);
				m_player.setGameWin(true);
				m_loserID = m_p3.m_ID;
				gamestate = GameState::Close;
			}
		}
	}
	if (m_player.getFillColour() == Color::Blue)
	{
		if (circleCollision(m_player.getPosition(), 10, m_p2.getPos(), 10) ||
			circleCollision(m_player.getPosition(), 10, m_p3.getPos(), 10))
		{
			m_p2.setWin(true);
			m_p3.setWin(true);
			m_loserID = m_player.getID();
			gamestate = GameState::Close;
		}
	}
}

bool Game::circleCollision(Vector2f t_pos1, float t_r1, Vector2f t_pos2, float t_r2)
{
	if (distanceSquared(t_pos1, t_pos2) <= ((t_r1 + t_r2) * (t_r1 + t_r2)))
	{
		return true;
	}
	return false;
}

float Game::distanceSquared(Vector2f t_v1, Vector2f t_v2)
{
	return ((t_v2.x - t_v1.x) * (t_v2.x - t_v1.x)) + 
		((t_v2.y - t_v1.y) * (t_v2.y - t_v1.y));
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