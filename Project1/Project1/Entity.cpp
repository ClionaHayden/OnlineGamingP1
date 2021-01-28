#include "Entity.h"

Entity::Entity(Vector2f t_pos, float t_radius, Color t_color) :
	m_pos(t_pos),
	m_radius(t_radius),
	m_color(t_color),
	m_win(false)
{
	setup();
}

void Entity::render(RenderWindow& t_win)
{
	t_win.draw(m_circ);
}

sf::Vector2f Entity::update(sf::Time t_deltaTime)
{
	m_circ.setPosition(m_pos);
	m_circ.setFillColor(m_color);
	return m_pos;
}

void Entity::setup()
{
	m_circ.setPosition(m_pos);
	m_circ.setFillColor(m_color);
	m_circ.setRadius(m_radius);
}

void Entity::setFillColour(Color t_col)
{
	m_color = t_col;
	m_circ.setFillColor(m_color);
}
