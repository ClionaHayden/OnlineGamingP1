#pragma once
#ifndef GAME
#define GAME
#define NOMINMAX

#include <SFML/Graphics.hpp>
#include <WinSock2.h>

#include "Client.h"
#include "Player.h"
#include "PacketStructs.h"
#include "PacketType.h"

class Game
{
public:
	Game();
	void run();
	void setPlayerData(int ID, Vector2f t_pos, Color t_col);
	void setID(int t_id) { m_player.setID(t_id); };
	Player& getPlayer() { return m_player; }
	Entity& getP2() { return m_p2; }
	Entity& getP3() { return m_p3; }
	int m_loserID;
	GameState gamestate;
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

	shared_ptr<Text> m_waitText;
	Font m_font;
	Client myClient;

	void setupEntities();
};

#endif // !GAME