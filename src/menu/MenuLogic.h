#pragma once

#include "AppState.h"
#include "System.h"

namespace Menu
{
	GameState Update(const State& state, const InputState& input, sf::Time dt);
}