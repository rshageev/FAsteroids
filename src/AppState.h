#pragma once

#include <variant>
#include <set>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "game/GameState.h"
#include "menu/MenuState.h"

// All the outside state for each frame
struct FrameData
{
    // user input
    std::set<sf::Keyboard::Key> keys;

    // delta time
    sf::Time dt;
};

using AppState = std::variant<Menu::State, Game::State>;