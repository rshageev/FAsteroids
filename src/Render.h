#pragma once

#include "AppState.h"

#include <SFML/Graphics.hpp>

struct Resources
{
	sf::ConvexShape player;
	sf::CircleShape asteroid;
	sf::RectangleShape bullet;
	sf::Font font;
};

Resources LoadResources();

void Draw(sf::RenderWindow& out, const AppState& state, const Resources& res);