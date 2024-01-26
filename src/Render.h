#pragma once

#include "AppState.h"

#include <SFML/Graphics.hpp>

struct Resources
{
	sf::ConvexShape player;
	sf::ConvexShape asteroid;
	sf::ConvexShape bullet;
	sf::Font font;
};

Resources LoadResources();

void Draw(sf::RenderWindow& out, const AppState& state, const Resources& res);