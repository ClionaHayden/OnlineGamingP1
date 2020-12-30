#pragma once
#include "Entity.h"

class Player : Entity
{
public:
	Player() = default;
	Player(Vector2f t_pos, float t_radius, Color t_color);
	void handleInput(Event& t_e);
	void draw(RenderWindow& t_win);
	void updatePos(sf::Time t_deltaTime);
private:
	float m_speed = 4.0f;
	void wrapScreen();
};