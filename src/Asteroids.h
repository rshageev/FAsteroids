#pragma once

#include "AppState.h"

GameState StartGame();
GameState GoToMenu();

GameState Update(const GameState& state, const InputState& input, sf::Time dt);
