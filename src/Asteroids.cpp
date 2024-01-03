#include "Asteroids.h"

#include "game/GameLogic.h"

AppState Update(const Game::State& game, sf::Time dt)
{
    return Game::Update(game, Game::GetUserInput(), dt);
}

AppState Update(const Menu::State& menu, sf::Time dt)
{
    return menu;
}

AppState Update(const AppState& state, sf::Time dt)
{
    return std::visit([dt](const auto& st) { return Update(st, dt); }, state);
}