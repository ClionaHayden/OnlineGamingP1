#pragma once
#define NOMINMAX
#include "Entity.h"

class Player : Entity
{
public:
	Player() = default;
	Player(Vector2f t_pos, float t_radius, Color t_color);
	void handleInput(Event& t_e);
	void draw(RenderWindow& t_win);
	sf::Vector2f updatePos(sf::Time t_deltaTime);
	void setPosition(Vector2f t_pos) { m_pos = t_pos; };
	Color getFillColour() { return getColor(); }
	bool getGameWin() { return getWin(); }
	void setGameWin(bool t_new) { setWin(t_new); }

	void setColour(Color t_col);
	int getID() { return m_ID; };
	void setID(int t_id) { m_ID = t_id; };
private:
	float m_speed = 4.0f;
	void wrapScreen();
};