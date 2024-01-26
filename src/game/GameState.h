#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

#include <vector>

namespace Game
{
	struct UserInput
	{
		bool turn_left = false;
		bool turn_right = false;
		bool accelerate = false;
		bool shoot = false;
	};

	struct Player
	{
		sf::Vector2f position;
		sf::Vector2f velocity;
		sf::Angle angle;
		float thrust = 0.0f;
		sf::Time shoot_cooldown;
	};

	struct Asteroid
	{
		sf::Vector2f position;
		sf::Vector2f velocity;
		sf::Angle angle;
		sf::Angle rot_speed;
	};

	struct Bullet
	{
		sf::Vector2f position;
		sf::Vector2f velocity;
		sf::Time lifetime;
	};

	using Asteroids = std::vector<Asteroid>;
	using Bullets = std::vector<Bullet>;

	struct State
	{
		Player player;
		Asteroids asteroids;
		Bullets bullets;
	};
}