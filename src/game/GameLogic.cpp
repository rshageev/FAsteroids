#include "GameLogic.h"
#include "Settings.h"

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

	template<class R, class V>
	concept range_of = stdr::range<R> && std::convertible_to<stdr::range_value_t<R>, V>;


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

	/* Collisions */

	struct CollisionsData
	{
		std::vector<bool> asteroids;
		std::vector<bool> bullets;
	};

	
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
			return player.shoot_cooldown - dt; // on cd: update cooldown timer
		}
		if (input.shoot) {
			return settings.player.shoot_cooldown; // noton cd, shooting: start cooldown timer
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

	auto UpdateAsteroids(range_of<Asteroid> auto&& asteroids, const CollisionsData& cd, sf::Time dt)
	{
		auto move_asteroid = [dt](const auto& idx_asteroid) -> Asteroid {
			const auto& [_, asteroid] = idx_asteroid;
			return {
				.position = UpdateObjectPosition(asteroid, dt),
				.velocity = asteroid.velocity,
				.radius = asteroid.radius,
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

	/* Collisions */

	bool Collides(const Asteroid& asteroid, const Bullet& bullet)
	{
		return (asteroid.position - bullet.position).length() <= asteroid.radius;
	}

	CollisionsData CalcAsteroidBulletCollisions(const State& prev_state)
	{
		// process all [asteroid, bullet] pairs along with their indices
		auto ab_pairs = stdv::cartesian_product(prev_state.asteroids | stdv::enumerate, prev_state.bullets | stdv::enumerate);

		auto cd = CollisionsData{
			.asteroids = std::vector<bool>(prev_state.asteroids.size(), false),
			.bullets = std::vector<bool>(prev_state.bullets.size(), false),
		};

		auto add_collision = [](CollisionsData cd, const auto& ab) {
			// ab is a tuple of tuples, so we need to destructure the whole thing
			const auto& [ia, ib] = ab;
			const auto& [aidx, asteroid] = ia;
			const auto& [bidx, bullet] = ib;

			if (Collides(asteroid, bullet)) {
				cd.asteroids[aidx] = true;
				cd.bullets[bidx] = true;
			}
			return cd;
		};

		return stdr::fold_left(ab_pairs, std::move(cd), add_collision);
	}

	/* Whole game state */

	AppState Update(const State& prev_state, const FrameData& frame)
	{
		const auto input = GetUserInput(frame);

		const auto collisions = CalcAsteroidBulletCollisions(prev_state);

		return State
		{
			.player = UpdatePlayer(prev_state.player, input, frame.dt),
			.asteroids = UpdateAsteroids(prev_state.asteroids, collisions, frame.dt) | stdr::to<std::vector>(),
			.bullets = ShootBullets(
				UpdateBullets(prev_state.bullets, collisions, frame.dt),
				prev_state.player, input),
		};
	}
}
