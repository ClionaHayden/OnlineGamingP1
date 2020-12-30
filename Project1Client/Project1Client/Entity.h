#pragma once
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

	void setPos(Vector2f t_pos) { m_pos = t_pos; };

	void render(RenderWindow& t_win);
	void update(sf::Time t_deltaTime);
private:
	CircleShape m_circ;
	Vector2f m_pos;
	float m_radius;
	Color m_color;


	void setup();
};