#pragma once

#include "AppState.h"

#include <SFML/System/Time.hpp>

AppState StartGame();

AppState Update(const AppState& state, const FrameData& frame);
