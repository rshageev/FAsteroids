#pragma once

#include <SFML/System/Vector2.hpp>

using namespace sf::Literals;

struct PlayerSettings
{
	float acceleration = 0.0f;
	sf::Angle turn_speed;
};

struct Settings
{
	sf::Vector2u window_size;
	sf::Vector2f field_size;
	PlayerSettings player;
};

inline constexpr auto settings = Settings
{
	.window_size = { 1920u, 1080u },
	.field_size = { 1920.f, 1080.f },
	.player = {
		.acceleration = 60.0f,
		.turn_speed = 120_deg,
	},
};
