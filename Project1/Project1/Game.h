#pragma once
#ifndef GAME
#define GAME
#define NOMINMAX

#include "Server.h"
#include "Player.h"

class Game
{
public:
	Game();
	~Game();
	void run();
	void setPlayerData(int ID, Vector2f t_pos, Color t_col);
	void setID(int t_id) { m_player.setID(t_id); };
	Player& getPlayer() { return m_player; }
	Entity& getP2() { return m_p2; }
	Entity& getP3() { return m_p3; }

	int m_loserID;
private:
	void setupAssets();
	void loadTextures();
	void processEvents();
	void update(Time);
	void render();

private:
	RenderWindow m_window;

	shared_ptr<Text> m_waitText;
	Font m_font;

	std::thread *m_lc;
	GameState gamestate = GameState::Setup;

	Player m_player;
	Entity m_p2;
	Entity m_p3;

	Server m_server;
	void setupEntities();
	void checkGameOver();
	bool circleCollision(Vector2f t_pos1, float t_r1, Vector2f t_pos2, float t_r2);
	float distanceSquared(Vector2f t_v1, Vector2f t_v2);
};

#endif // !GAME