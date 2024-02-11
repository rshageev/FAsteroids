#pragma once

#include <variant>
#include <set>

#include <SFML/Window/Keyboard.hpp>

#include "game/GameState.h"
#include "menu/MenuState.h"

struct InputState
{
    // pressed keys
    std::set<sf::Keyboard::Key> keys;
};

using GameState = std::variant<Menu::State, Game::State>;