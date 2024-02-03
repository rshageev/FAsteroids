#include "MenuLogic.h"
#include "Asteroids.h"

namespace Menu
{
	AppState Update(const State& state, const InputState& input, sf::Time)
	{
		return input.keys.empty() ? state : StartGame();
	}
}