#pragma once
#pragma once
#define NOMINMAX
#include <SFML/Graphics.hpp>

using namespace sf;
class Entity
{
public:
	Entity() = default;
	Entity(Vector2f t_pos, float t_radius, Color t_color);

	Vector2f getPos() { return m_pos; };
	float getRadius() { return m_radius; };
	Color getColor() { return m_color; };

	bool getWin() { return m_win; }
	void setWin(bool t_new) { m_win = t_new; }

	void setFillColour(Color t_col);

	void setPos(Vector2f t_pos) { m_pos = t_pos; };

	void render(RenderWindow & t_win);
	sf::Vector2f update(sf::Time t_deltaTime);
	int m_ID;
	Vector2f m_pos;
private:
	CircleShape m_circ;
	float m_radius;
	Color m_color;
	bool m_win;

	void setup();
};