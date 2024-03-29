#include "GameLogic.h"
#include "Settings.h"
#include "Utils.h"
#include "Collisions.h"
#include "functional/GenerateView.h"

#include <random>
#include <functional>

namespace Game
{
	template<class T>
	concept GameObject = requires (T obj) {
		{ obj.position } -> std::convertible_to<sf::Vector2f>;
		{ obj.velocity } -> std::convertible_to<sf::Vector2f>;
	};

	/* Common functions */

	auto WrapAround(float value, float min, float max) -> float
	{
		const auto rem = std::fmod(value - min, max - min);
		return min + ((value >= min) ? rem : max - rem);
	}

	auto WrapAround(sf::Vector2f position, sf::Vector2f field_size)
	{
		return sf::Vector2f{
			WrapAround(position.x, 0.0f, field_size.x),
			WrapAround(position.y, 0.0f, field_size.y)
		};
	}

	auto UpdateObjectPosition(const GameObject auto& obj, sf::Time dt)
	{
		return WrapAround(obj.position + obj.velocity * dt.asSeconds(), settings.field_size);
	}

	/* User input */

	auto TranslateUserInput(const InputState& input)
	{
		return UserInput{
			.turn_left = input.keys.contains(sf::Keyboard::Key::Left),
			.turn_right = input.keys.contains(sf::Keyboard::Key::Right),
			.accelerate = input.keys.contains(sf::Keyboard::Key::Up),
			.shoot = input.keys.contains(sf::Keyboard::Key::Space),
		};
	}
	
	/* Player */

	auto UpdatePlayerVelocity(sf::Vector2f velocity, sf::Angle angle, float thrust, sf::Time dt)
	{
		return velocity + sf::Vector2f{ thrust, angle } * dt.asSeconds();
	}

	auto UpdatePlayerAngle(sf::Angle angle, const UserInput& input, sf::Time dt)
	{
		const auto sign = (input.turn_right ? 1.0f : 0.0f) - (input.turn_left ? 1.0f : 0.0f);
		return angle + sign * settings.player.turn_speed * dt.asSeconds();
	}

	auto UpdatePlayerShootCooldown(const Player& player, const UserInput& input, sf::Time dt)
	{
		if (player.shoot_cooldown > sf::Time::Zero) {	
			return player.shoot_cooldown - dt; // on cd: update cooldown timer
		}
		if (input.shoot) {
			return settings.player.shoot_cooldown; // noton cd, shooting: start cooldown timer
		}
		return player.shoot_cooldown; // not on cd, not shooting
	}

	auto UpdatePlayer(const Player& player, const UserInput& input, sf::Time dt)
	{
		return Player{
			.position = UpdateObjectPosition(player, dt),
			.velocity = UpdatePlayerVelocity(player.velocity, player.angle, player.thrust, dt),
			.angle = UpdatePlayerAngle(player.angle, input, dt),
			.thrust = input.accelerate ? settings.player.acceleration : 0.0f,
			.shoot_cooldown = UpdatePlayerShootCooldown(player, input, dt),
		};
	}

	/* Asteroids */

	float Random(auto&& rng, float min, float max)
	{
		std::uniform_real_distribution<float> distr(min, max);
		return distr(rng);
	}

	auto SpawnAsteroids(int count) -> Asteroids
	{
		std::random_device rand_device;
		std::mt19937 rng(rand_device());

		std::uniform_real_distribution pos_x(0.0f, settings.field_size.x);
		std::uniform_real_distribution pos_y(0.0f, settings.field_size.y);
		std::uniform_real_distribution speed(settings.asteroid.speed.min, settings.asteroid.speed.max);
		std::uniform_real_distribution angle(0.0f, 360.0f);
		std::uniform_real_distribution rot_speed(settings.asteroid.rot_speed.min.asDegrees(), settings.asteroid.rot_speed.max.asDegrees());

		auto spawn_asteroid = [&]() -> Asteroid {
			return {
				.position = { pos_x(rng), pos_y(rng) },
				.velocity = { speed(rng), sf::degrees(angle(rng)) },
				.angle = sf::degrees(angle(rng)),
				.rot_speed = sf::degrees(rot_speed(rng)),
			};
		};

		return views::generate(spawn_asteroid) | stdv::take(count) | stdr::to<std::vector>();
	}

	auto UpdateAsteroids(range_of<Asteroid> auto&& asteroids, const CollisionsData& cd, sf::Time dt)
	{
		auto move_asteroid = [dt](const auto& idx_asteroid) -> Asteroid {
			const auto& [_, asteroid] = idx_asteroid;
			return {
				.position = UpdateObjectPosition(asteroid, dt),
				.velocity = asteroid.velocity,
				.angle = (asteroid.angle + asteroid.rot_speed * dt.asSeconds()).wrapSigned(),
				.rot_speed = asteroid.rot_speed,
			};
		};

		auto is_asteroid_alive = [&cd](const auto& idx_asteroid) {
			const auto& [idx, _] = idx_asteroid;
			return !cd.asteroids[idx];
		};

		return asteroids | stdv::enumerate    // process asteroids with their indices (to check collisions)
			| stdv::filter(is_asteroid_alive) // remove collided asteroids
			| stdv::transform(move_asteroid); // move remaining asteroids
	}


	/* Bullets */

	auto ShootBullet(const Player& player)
	{
		return Bullet{
			.position = player.position,
			.velocity = { settings.bullet.speed, player.angle },
			.lifetime = settings.bullet.lifetime,
		};
	}

	auto ShootBullets(range_of<Bullet> auto&& bullets, const Player& player, const UserInput& input)
	{
		Bullets out;
		out.append_range(bullets);
		if (player.shoot_cooldown <= sf::Time::Zero && input.shoot) {
			out.push_back(ShootBullet(player));
		}
		return out;
	}

	auto UpdateBullets(range_of<Bullet> auto&& bullets, const CollisionsData& cd, sf::Time dt)
	{
		auto update_bullet = [dt](const auto& idx_bullet) -> Bullet {
			const auto& [_, bullet] = idx_bullet;
			return {
				.position = UpdateObjectPosition(bullet, dt),
				.velocity = bullet.velocity,
				.lifetime = bullet.lifetime - dt,
			};
		};

		auto bullet_alive = [&cd](const auto& idx_bullet) -> bool {
			const auto& [idx, bullet] = idx_bullet;
			return !cd.bullets[idx] && bullet.lifetime > sf::Time::Zero;
		};

		return bullets | stdv::enumerate      // process bullets along with their indices (to check collisions)
			| stdv::filter(bullet_alive)      // remove collided or expired bullets
			| stdv::transform(update_bullet); // move remaining bullets and update their lifetimes
	}

	/* Whole game state */

	GameState Update(const State& prev_state, const InputState& input_state, sf::Time dt)
	{
		const auto input = TranslateUserInput(input_state);

		const auto collisions = CalcAllCollisions(prev_state);

		return State
		{
			.player = UpdatePlayer(prev_state.player, input, dt),
			.asteroids = UpdateAsteroids(prev_state.asteroids, collisions, dt) | stdr::to<std::vector>(),
			.bullets = ShootBullets(
				UpdateBullets(prev_state.bullets, collisions, dt),
				prev_state.player, input),
		};
	}
}
