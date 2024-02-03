#pragma once

#include "AppState.h"

AppState StartGame();
AppState GoToMenu();

AppState Update(const AppState& state, const InputState& input, sf::Time dt);
