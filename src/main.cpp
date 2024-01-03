#include <SFML/Graphics.hpp>

#include "Asteroids.h"
#include "Render.h"

AppState InitGameState(sf::Vector2u windowSize)
{
    return Game::State{
        .player = {
            .position = sf::Vector2f{ windowSize } * 0.5f
        }
    };
}

bool EventLoop(sf::Window& window)
{
    for (auto event = sf::Event{}; window.pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
    return window.isOpen();
}

int main()
{
    auto window = sf::RenderWindow{ sf::VideoMode{ { 1920u, 1080u } }, "Asteroids" };
    window.setVerticalSyncEnabled(true);


    AppState state = InitGameState(window.getSize());

    sf::Clock clock;
    while (EventLoop(window))
    {
        const auto dt = clock.restart();
        auto next_state = Update(state, dt);
        state = std::move(next_state);

        Draw(state, window);
    }
}