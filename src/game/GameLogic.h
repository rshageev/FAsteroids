#pragma once

#include "../AppState.h"

namespace Game
{
	Asteroids SpawnAsteroids(int count);

	AppState Update(const State& state, const FrameData& frame);
}