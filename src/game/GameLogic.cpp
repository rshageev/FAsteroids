#include "GameLogic.h"
#include "../Settings.h"

#include <SFML/Window/Keyboard.hpp>

namespace Game
{
	/* Common functions */

	constexpr float WrapValue(float value, float min, float max)
	{
		const auto w = max - min;
		while (value < min) value += w;
		while (value > max) value -= w;
		return value;
	}

	constexpr sf::Vector2f WrapPosition(sf::Vector2f position, sf::Vector2f field_size)
	{
		return sf::Vector2f{
			WrapValue(position.x, 0.0f, field_size.x),
			WrapValue(position.y, 0.0f, field_size.y)
		};
	}


	/* User input */

	UserInput GetUserInput()
	{
		return {
			.turn_left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left),
			.turn_right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right),
			.accelerate = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up),
		};
	}
	
	/* Player */

	sf::Vector2f UpdatePlayerPosition(sf::Vector2f position, sf::Vector2f velocity, sf::Time dt)
	{
		return WrapPosition(position + velocity * dt.asSeconds(), settings.field_size);
	}

	sf::Vector2f UpdatePlayerVelocity(sf::Vector2f velocity, sf::Angle angle, float thrust, sf::Time dt)
	{
		return velocity + sf::Vector2f{ thrust, angle } * dt.asSeconds();
	}

	sf::Angle UpdatePlayerAngle(sf::Angle angle, const UserInput& input, sf::Time dt)
	{
		const auto da = settings.player.turn_speed * dt.asSeconds();
		if (input.turn_left) angle -= da;
		if (input.turn_right) angle += da;
		return angle;
	}

	Player UpdatePlayer(const Player& player, const UserInput& input, sf::Time dt)
	{
		return {
			.position = UpdatePlayerPosition(player.position, player.velocity, dt),
			.velocity = UpdatePlayerVelocity(player.velocity, player.angle, player.thrust, dt),
			.angle = UpdatePlayerAngle(player.angle, input, dt),
			.thrust = input.accelerate ? settings.player.acceleration : 0.0f,
		};
	}

	/* Whole game state */

	AppState Update(const State& prev_state, UserInput input, sf::Time dt)
	{
		return State
		{
			.player = UpdatePlayer(prev_state.player, input, dt)
		};
	}
}
