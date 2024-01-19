#include "GameLogic.h"
#include "../Settings.h"

#include <random>
#include <ranges>

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace Game
{
	template<class T>
	concept GameObject = requires (T obj) {
		{ obj.position } -> std::convertible_to<sf::Vector2f>;
		{ obj.velocity } -> std::convertible_to<sf::Vector2f>;
	};


	/* Common functions */

	float WrapAround(float value, float min, float max)
	{
		const auto rem = std::fmod(value - min, max - min);
		return min + ((value >= min) ? rem : max - rem);
	}

	sf::Vector2f WrapAround(sf::Vector2f position, sf::Vector2f field_size)
	{
		return {
			WrapAround(position.x, 0.0f, field_size.x),
			WrapAround(position.y, 0.0f, field_size.y)
		};
	}

	sf::Vector2f UpdateObjectPosition(const GameObject auto& obj, sf::Time dt)
	{
		return WrapAround(obj.position + obj.velocity * dt.asSeconds(), settings.field_size);
	}

	/* User input */

	UserInput GetUserInput(const FrameData& frame)
	{
		return {
			.turn_left = frame.keys.contains(sf::Keyboard::Key::Left),
			.turn_right = frame.keys.contains(sf::Keyboard::Key::Right),
			.accelerate = frame.keys.contains(sf::Keyboard::Key::Up),
			.shoot = frame.keys.contains(sf::Keyboard::Key::Space),
		};
	}
	
	/* Player */

	sf::Vector2f UpdatePlayerVelocity(sf::Vector2f velocity, sf::Angle angle, float thrust, sf::Time dt)
	{
		return velocity + sf::Vector2f{ thrust, angle } * dt.asSeconds();
	}

	sf::Angle UpdatePlayerAngle(sf::Angle angle, const UserInput& input, sf::Time dt)
	{
		const auto sign = (input.turn_right ? 1.0f : 0.0f) - (input.turn_left ? 1.0f : 0.0f);
		return angle + sign * settings.player.turn_speed * dt.asSeconds();
	}

	sf::Time UpdatePlayerShootCooldown(const Player& player, const UserInput& input, sf::Time dt)
	{
		if (player.shoot_cooldown > sf::Time::Zero) {	
			return player.shoot_cooldown - dt; // on cooldown
		}
		else if (input.shoot) {
			return settings.player.shoot_cooldown; // set cooldown when shooting
		}
		return player.shoot_cooldown; // not on cd, not shooting
	}

	Player UpdatePlayer(const Player& player, const UserInput& input, sf::Time dt)
	{
		return {
			.position = UpdateObjectPosition(player, dt),
			.velocity = UpdatePlayerVelocity(player.velocity, player.angle, player.thrust, dt),
			.angle = UpdatePlayerAngle(player.angle, input, dt),
			.thrust = input.accelerate ? settings.player.acceleration : 0.0f,
			.shoot_cooldown = UpdatePlayerShootCooldown(player, input, dt),
		};
	}

	/* Asteroids */

	Asteroids SpawnAsteroids(int count)
	{
		std::random_device rand_device;
		std::mt19937 rng(rand_device());

		std::uniform_real_distribution pos_x(0.0f, settings.field_size.x);
		std::uniform_real_distribution pos_y(0.0f, settings.field_size.y);
		std::uniform_real_distribution speed(settings.asteroid.speed.min, settings.asteroid.speed.max);
		std::uniform_real_distribution angle(0.0f, 360.0f);
		std::uniform_real_distribution radius(settings.asteroid.radius.min, settings.asteroid.radius.max);

		auto spawn_asteroid = [&](int) -> Asteroid {
			return {
				.position = { pos_x(rng), pos_y(rng) },
				.velocity = { speed(rng), sf::degrees(angle(rng)) },
				.radius = radius(rng),
			};
		};

		return stdv::iota(0, count) | stdv::transform(spawn_asteroid) | stdr::to<std::vector>();
	}

	auto MoveAsteroids(const Asteroids& asteroids, sf::Time dt)
	{
		auto move_asteroid = [dt](const Asteroid& asteroid) -> Asteroid {
			return {
				.position = UpdateObjectPosition(asteroid, dt),
				.velocity = asteroid.velocity,
				.radius = asteroid.radius,
			};
		};

		return asteroids | stdv::transform(move_asteroid) ;
	}

	/* Bullets */
	Bullet ShootBullet(const Player& player)
	{
		return {
			.position = player.position,
			.velocity = { settings.bullet.speed, player.angle },
			.lifetime = settings.bullet.lifetime,
		};
	}

	Bullets ShootBullets(auto&& bullets, const Player& player, const UserInput& input)
	{
		Bullets out;
		out.append_range(bullets);
		if (player.shoot_cooldown <= sf::Time::Zero && input.shoot) {
			out.push_back(ShootBullet(player));
		}
		return out;
	}

	auto UpdateBullets(const auto& bullets, sf::Time dt)
	{
		auto update_bullet = [dt](const Bullet& bullet) -> Bullet {
			return {
				.position = UpdateObjectPosition(bullet, dt),
				.velocity = bullet.velocity,
				.lifetime = bullet.lifetime - dt,
			};
		};

		auto bullet_alive = [](const Bullet& bullet) -> bool {
			return bullet.lifetime > sf::Time::Zero;
		};

		return bullets | stdv::filter(bullet_alive) | stdv::transform(update_bullet);
	}

	/* Whole game state */

	AppState Update(const State& prev_state, const FrameData& frame)
	{
		const auto input = GetUserInput(frame);

		return State
		{
			.player = UpdatePlayer(prev_state.player, input, frame.dt),
			.asteroids = MoveAsteroids(prev_state.asteroids, frame.dt) | stdr::to<std::vector>(),
			.bullets = ShootBullets(UpdateBullets(prev_state.bullets, frame.dt), prev_state.player, input),
		};
	}
}
