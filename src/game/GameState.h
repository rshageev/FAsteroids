#pragma once

#include <SFML/System/Vector2.hpp>

namespace Game
{
	struct UserInput
	{
		bool turn_left = false;
		bool turn_right = false;
		bool accelerate = false;
	};

	struct Player
	{
		sf::Vector2f position;
		sf::Vector2f velocity;
		sf::Angle angle;
		float thrust = 0.0f;
	};

	struct State
	{
		Player player;
	};
}