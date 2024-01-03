#pragma once

#include "AppState.h"

#include <SFML/System/Time.hpp>

AppState Update(const AppState& state, sf::Time dt);
