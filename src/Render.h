#pragma once

#include "AppState.h"

#include <SFML/Graphics.hpp>

struct Sprites
{
	sf::ConvexShape player;
	sf::CircleShape asteroid;
	sf::RectangleShape bullet;
	sf::Font font;
};

Sprites LoadResources();

void Draw(sf::RenderWindow& out, const AppState& state, const Sprites& sprites);