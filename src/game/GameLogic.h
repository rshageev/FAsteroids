#pragma once

#include "AppState.h"

namespace Game
{
	Asteroids SpawnAsteroids(int count);

	GameState Update(const State& state, const InputState& input, sf::Time dt);
}