#include "MenuLogic.h"
#include "Asteroids.h"

namespace Menu
{
	AppState Update(const State& state, const FrameData& frame)
	{
		return frame.keys.empty() ? state : StartGame();
	}
}