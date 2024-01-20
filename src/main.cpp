#include <SFML/Graphics.hpp>

#include "Asteroids.h"
#include "Render.h"
#include "Settings.h"

FrameData UpdateFrame(FrameData frame, sf::Window& window, sf::Clock& clock)
{
    for (auto event = sf::Event{}; window.pollEvent(event);)
    {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            frame.keys.insert(event.key.code);
        }
        if (event.type == sf::Event::KeyReleased) {
            frame.keys.erase(event.key.code);
        }
    }

    frame.dt = clock.restart();

    return frame;
}

int main()
{
    auto window = sf::RenderWindow{ sf::VideoMode{ settings.window_size }, "Asteroids" };
    window.setVerticalSyncEnabled(true);

    const auto resources = LoadResources();

    auto state = AppState{};
    auto frame = FrameData{};
    sf::Clock clock;

    while (window.isOpen())
    {
        frame = UpdateFrame(frame, window, clock);
        state = Update(state, frame);
        Draw(window, state, resources);
    }

    return 0;
}