#include "Asteroids.h"
#include "Settings.h"
#include "game/GameLogic.h"
#include "menu/MenuLogic.h"

GameState StartGame()
{
    return Game::State{
        .player = {
            .position = settings.field_size * 0.5f
        },
        .asteroids = Game::SpawnAsteroids(8),
    };
}

GameState GoToMenu()
{
    return Menu::State{};
}

GameState Update(const GameState& state, const InputState& input, sf::Time dt)
{
    return std::visit([&, dt](const auto& st) { return Update(st, input, dt); }, state);
}