#pragma once

#include <variant>
#include <set>

#include <SFML/Window/Keyboard.hpp>

#include "game/GameState.h"
#include "menu/MenuState.h"

using GameState = std::variant<Menu::State, Game::State>;