#include "Player.h"

Player::Player(Vector2f t_pos, float t_radius, Color t_color) :
	Entity(t_pos, t_radius, t_color)
{
}

void Player::handleInput(Event& t_e)
{
	if (Event::KeyPressed == t_e.type)
	{
		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			setPos(Vector2f(getPos().x - m_speed, getPos().y));
		}
		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			setPos(Vector2f(getPos().x + m_speed, getPos().y));
		}
		if (Keyboard::isKeyPressed(Keyboard::W))
		{
			setPos(Vector2f(getPos().x, getPos().y - m_speed));
		}
		if (Keyboard::isKeyPressed(Keyboard::S))
		{
			setPos(Vector2f(getPos().x, getPos().y + m_speed));
		}
	}
	wrapScreen();
}

void Player::draw(RenderWindow& t_win)
{
	render(t_win);
}

void Player::updatePos(sf::Time t_deltaTime)
{
	update(t_deltaTime);
}

void Player::setColour(Color t_col)
{
	setFillColour(t_col);
}

void Player::wrapScreen()
{
	if (getPos().x > 600)
	{
		setPos(Vector2f(0, getPos().y));
	}
	else if (getPos().x < 0)
	{
		setPos(Vector2f(600, getPos().y));
	}
	else if (getPos().y < 0)
	{
		setPos(Vector2f(getPos().x, 600));
	}
	else if (getPos().y > 600)
	{
		setPos(Vector2f(getPos().x, 0));
	}
}
