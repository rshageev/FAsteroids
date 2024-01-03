#include "GameLogic.h"

#include <SFML/Window/Keyboard.hpp>

namespace Game
{
	UserInput GetUserInput()
	{
		return {
			.turn_left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left),
			.turn_right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right),
			.accelerate = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up),
		};
	}

	sf::Angle UpdatePlayerAngle(sf::Angle angle, const UserInput& input, sf::Time dt)
	{
		const auto da = sf::degrees(120.0f) * dt.asSeconds();
		if (input.turn_left) angle -= da;
		if (input.turn_right) angle += da;
		return angle;
	}

	AppState Update(const State& prev_state, UserInput input, sf::Time dt)
	{
		return State
		{
			.player {
				.position = prev_state.player.position,
				.velocity = prev_state.player.velocity,
				.angle = UpdatePlayerAngle(prev_state.player.angle, input, dt),
			},
		};
	}
}
