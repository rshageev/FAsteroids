#include "Collisions.h"
#include "Utils.h"
#include "Settings.h"

namespace Game
{
	constexpr bool LineIntersectsSegment(sf::Vector2f line_origin, sf::Vector2f line_dir, sf::Vector2f p1, sf::Vector2f p2)
	{
		const auto cp1 = line_dir.cross(p1 - line_origin);
		const auto cp2 = line_dir.cross(p2 - line_origin);

		// try to count intersection with p2, but not with p1
		return (cp1 > 0.0f && cp2 <= 0.0f) || (cp1 < 0.0f && cp2 >= 0.0f);
	}

	constexpr bool SegmentsIntersect(sf::Vector2f p11, sf::Vector2f p12, sf::Vector2f p21, sf::Vector2f p22)
	{
		return LineIntersectsSegment(p11, p12 - p11, p21, p22)
			&& LineIntersectsSegment(p21, p22 - p21, p11, p12);
	}

	constexpr bool IsPointInPolygon(sf::Vector2f point, range_of<sf::Vector2f> auto&& polygon)
	{
		const auto max_x = stdr::fold_left(polygon, point.x, [](float x, sf::Vector2f pt) { return std::max(x, pt.x); });
		const auto ray_end = sf::Vector2f{ max_x + 1.0f, point.y };

		auto intersects_ray = [point, ray_end](const auto& edge) {
			return SegmentsIntersect(point, ray_end, std::get<0>(edge), std::get<1>(edge));
		};
		auto edges = stdv::repeat(polygon) | stdv::join | stdv::pairwise | stdv::take(stdr::size(polygon));

		return stdr::count_if(edges, intersects_ray) % 2 == 1;
	}

	bool Collides(const Asteroid& asteroid, const Bullet& bullet)
	{
		auto asteroid_pts = AsteroidShape | stdv::transform(translate_rotate{ asteroid.position, asteroid.angle });

		return IsPointInPolygon(bullet.position, asteroid_pts);
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