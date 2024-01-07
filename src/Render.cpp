#include "Render.h"
#include "Settings.h"

namespace
{
	sf::ConvexShape CreateConvexShape(const auto& points, float scale = 1.0f)
	{
		sf::ConvexShape shape(std::size(points));
		size_t idx = 0;
		for (auto point : points) {
			shape.setPoint(idx++, point * scale);
		}
		return shape;
	}

	sf::ConvexShape CreatePlayerShape()
	{
		static constexpr std::array<sf::Vector2f, 4> PlayerPoints = { {
			{  1.0f,  0.0f },
			{ -0.5f, -0.6f },
			{ -0.2f,  0.0f },
			{ -0.5f,  0.6f },
		} };

		auto shape = CreateConvexShape(PlayerPoints, 15.0f);
		shape.setFillColor(sf::Color::White);
		return shape;
	}

	sf::CircleShape CreateAsteroidShape()
	{
		sf::CircleShape asteroid(1.0f, 12u);
		asteroid.setFillColor(sf::Color::Blue);
		asteroid.setOrigin({ 1.0f, 1.0f });
		return asteroid;
	}

	sf::RectangleShape CreateBulletShape()
	{
		sf::RectangleShape bullet({ 4.0f, 4.0f });
		bullet.setFillColor(sf::Color::White);
		bullet.setOrigin({ 2.0f, 2.0f });
		return bullet;
	}

	void Draw(sf::RenderTarget& target, const sf::Drawable& drawable, sf::Vector2f pos, float scale = 1.0f, sf::Angle angle = sf::degrees(0.0f))
	{
		target.draw(drawable, { sf::Transform{}
			.translate(pos)
			.rotate(angle)
			.scale({ scale, scale })
		});
	}
}

Sprites LoadResources()
{
	return {
		.player = CreatePlayerShape(),
		.asteroid = CreateAsteroidShape(),
		.bullet = CreateBulletShape(),
	};
}

void Draw(sf::RenderWindow& out, const Game::State& state, const Sprites& sprites)
{
	for (const auto& asteroid : state.asteroids) {
		Draw(out, sprites.asteroid, asteroid.position, asteroid.radius);
	}

	for (const auto& bullet : state.bullets) {
		Draw(out, sprites.bullet, bullet.position);
	}

	Draw(out, sprites.player, state.player.position, 1.0f, state.player.angle);
}

void Draw(sf::RenderWindow& out, const Menu::State& state, const Sprites& sprites)
{
}

void Draw(sf::RenderWindow& window, const AppState& state, const Sprites& sprites)
{
	window.clear();

	std::visit([&](const auto& st) { Draw(window, st, sprites); }, state);

	window.display();
}