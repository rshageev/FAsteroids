#include "Render.h"
#include "Settings.h"
#include "Utils.h"

namespace
{
	sf::ConvexShape CreateConvexShape(range_of<sf::Vector2f> auto&& points, sf::Color color)
	{
		sf::ConvexShape shape(stdr::size(points));
		for (auto [idx, point] : points | stdv::enumerate) {
			shape.setPoint(idx, point);
		}
		shape.setFillColor(color);
		return shape;
	}

	sf::Font LoadFont(const std::filesystem::path& path)
	{
		sf::Font font;
		[[maybe_unused]] const bool ok = font.loadFromFile(path);
		return font;
	}

	void Draw(sf::RenderTarget& target, const sf::Drawable& drawable, sf::Vector2f pos, sf::Angle angle)
	{
		target.draw(drawable, { sf::Transform{}
			.translate(pos)
			.rotate(angle)
		});
	}

	void PrintText(sf::RenderTarget& target, const sf::Font& font, const sf::String& string, sf::Vector2f position, unsigned int size, sf::Color color = sf::Color::White)
	{
		sf::Text text(font, string, size);
		text.setPosition(position);
		text.setFillColor(color);
		target.draw(text);
	}
}

Resources LoadResources()
{
	return {
		.player = CreateConvexShape(PlayerShape, sf::Color::White),
		.asteroid = CreateConvexShape(AsteroidShape, sf::Color::Blue),
		.bullet = CreateConvexShape(BulletShape, sf::Color::White),
		.font = LoadFont("res/AstroSpace.ttf"),
	};
}

void Draw(sf::RenderWindow& out, const Game::State& state, const Resources& res)
{
	for (const auto& asteroid : state.asteroids) {
		Draw(out, res.asteroid, asteroid.position, asteroid.angle);
	}

	for (const auto& bullet : state.bullets) {
		Draw(out, res.bullet, bullet.position, sf::degrees(0.0f));
	}

	Draw(out, res.player, state.player.position, state.player.angle);
}

void Draw(sf::RenderWindow& window, const Menu::State& state, const Resources& res)
{
	PrintText(window, res.font, "Asteroids", { 100.0f, 100.0f }, 64);

	PrintText(window, res.font, "Press any key to play", { 100.0f, 250.0f }, 24, sf::Color(200, 200, 200));
}

void Draw(sf::RenderWindow& window, const GameState& state, const Resources& res)
{
	window.clear();

	std::visit([&](const auto& st) { Draw(window, st, res); }, state);

	window.display();
}