#pragma once

#include <variant>

#include "game/GameState.h"
#include "menu/MenuState.h"

using AppState = std::variant<Menu::State, Game::State>;