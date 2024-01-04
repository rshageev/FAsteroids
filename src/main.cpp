#include <SFML/Graphics.hpp>

#include "Asteroids.h"
#include "Render.h"
#include "Settings.h"

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
    auto window = sf::RenderWindow{ sf::VideoMode{ settings.window_size }, "Asteroids" };
    window.setVerticalSyncEnabled(true);

    auto state = StartGame();

    sf::Clock clock;
    while (EventLoop(window))
    {
        state = Update(state, clock.restart());
        Draw(state, window);
    }

    return 0;
}