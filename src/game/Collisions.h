#pragma once

#include "GameState.h"

#include <vector>

namespace Game
{
	struct CollisionsData
	{
		std::vector<bool> asteroids;
		std::vector<bool> bullets;
	};

	CollisionsData CalcAllCollisions(const State& prev_state);
}