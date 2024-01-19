#include "Asteroids.h"
#include "Settings.h"
#include "game/GameLogic.h"


AppState StartGame()
{
    return Game::State{
        .player = {
            .position = settings.field_size * 0.5f
        },
        .asteroids = Game::SpawnAsteroids(8),
    };
}

AppState Update(const Menu::State& menu, const FrameData& frame)
{
    return menu;
}

AppState Update(const AppState& state, const FrameData& frame)
{
    return std::visit([&](const auto& st) { return Update(st, frame); }, state);
}