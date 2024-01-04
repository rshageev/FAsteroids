#pragma once

#include "../AppState.h"

#include <SFML/System/Time.hpp>

namespace Game
{
	UserInput GetUserInput();

	Asteroids SpawnAsteroids(int count);

	AppState Update(const State& state, UserInput input, sf::Time dt);
}