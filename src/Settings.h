#pragma once

#include <SFML/System/Vector2.hpp>

using namespace sf::Literals;
using namespace std::chrono_literals;

template<class T>
struct ValueRange
{
	T min{};
	T max{};
};

struct PlayerSettings
{
	float acceleration = 0.0f;
	sf::Angle turn_speed;
	sf::Time shoot_cooldown;
};

struct AsteroidSettings
{
	ValueRange<float> speed;
	ValueRange<float> radius;
};

struct BulletSettings
{
	float speed;
	sf::Time lifetime;
};

struct Settings
{
	sf::Vector2u window_size;
	sf::Vector2f field_size;
	PlayerSettings player;
	AsteroidSettings asteroid;
	BulletSettings bullet;
};

inline constexpr auto settings = Settings
{
	.window_size = { 1920u, 1080u },
	.field_size = { 1920.f, 1080.f },
	.player = {
		.acceleration = 60.0f,
		.turn_speed = 120_deg,
		.shoot_cooldown = 150ms,
	},
	.asteroid = {
		.speed = { 0.0f, 50.0f },
		.radius = { 15.0f, 20.0f },
	},
	.bullet = {
		.speed = 320.0f,
		.lifetime = 3s,
	},
};
