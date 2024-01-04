#pragma once

#include <SFML/System/Vector2.hpp>

#include <vector>

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

	struct Asteroid
	{
		sf::Vector2f position;
		sf::Vector2f velocity;
		float radius = 0.0f;
	};

	using Asteroids = std::vector<Asteroid>;

	struct State
	{
		Player player;
		Asteroids asteroids;
	};
}