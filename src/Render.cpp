#include "Render.h"

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

	class Renderer
	{
	public:
		Renderer()
		{
			player = CreatePlayerShape();
		}

		void DrawPlayer(sf::RenderWindow& window, sf::Vector2f pos, sf::Angle angle)
		{
			player.setPosition(pos);
			player.setRotation(angle);
			window.draw(player);
		}

	private:
		sf::ConvexShape player;
	};

	Renderer render;
}


void Draw(const Game::State& state, sf::RenderWindow& window)
{
	render.DrawPlayer(window, state.player.position, state.player.angle);
}

void Draw(const Menu::State& state, sf::RenderWindow& window)
{
}

void Draw(const AppState& state, sf::RenderWindow& window)
{
	window.clear();

	std::visit([&](const auto& st) { Draw(st, window); }, state);

	window.display();
}