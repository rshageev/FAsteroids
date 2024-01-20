#include "Collisions.h"
#include "Utils.h"

namespace Game
{
	bool Collides(const Asteroid& asteroid, const Bullet& bullet)
	{
		return (asteroid.position - bullet.position).lengthSq() <= asteroid.radius * asteroid.radius;
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

	CollisionsData CalcAllCollisions(const State& prev_state)
	{
		return CalcAsteroidBulletCollisions(prev_state);
	}
}