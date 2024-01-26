#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>

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
		.speed = { 10.0f, 50.0f },
	},
	.bullet = {
		.speed = 320.0f,
		.lifetime = 3s,
	},
};


template<std::size_t N>
consteval std::array<sf::Vector2f, N> ScaleShape(float scale, sf::Vector2f(&& arr)[N])
{
	std::array<sf::Vector2f, N> out;
	for (std::size_t idx = 0; idx < N; ++idx) {
		out[idx] = arr[idx] * scale;
	}
	return out;
}

inline constexpr auto PlayerShape = ScaleShape(15.0f, {
	{  1.0f,  0.0f },
	{ -0.5f, -0.6f },
	{ -0.2f,  0.0f },
	{ -0.5f,  0.6f },
});

inline constexpr auto BulletShape = ScaleShape(2.0f, {
	{  1.0f,  1.0f },
	{  1.0f, -1.0f },
	{ -1.0f, -1.0f },
	{ -1.0f,  1.0f },
});

inline constexpr auto AsteroidShape = ScaleShape(25.0f, {
	{  0.0f,  1.0f },
	{  0.5f,  0.9f },

	{  1.0f,  0.0f },
	{  0.6f, -0.6f },

	{  0.0f, -1.0f },
	{ -0.5f, -0.3f },

	{ -1.0f,  0.0f },
	{ -0.8f,  0.7f },
});