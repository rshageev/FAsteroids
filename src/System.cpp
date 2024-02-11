#include "System.h"

#include "Settings.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

System::System()
{
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode{ settings.window_size }, "Asteroids");
    window->setVerticalSyncEnabled(true);
}

System::System(System&&) = default;

System::~System() = default;

System::State System::operator()()
{
    for (auto event = sf::Event{}; window->pollEvent(event);) {
        switch (event.type) {
        case sf::Event::KeyPressed:
            input.keys.insert(event.key.code); break;
        case sf::Event::KeyReleased:
            input.keys.erase(event.key.code); break;
        case sf::Event::Closed:
            app_closed = true; break;
        }
    }

    return State{ window.get(), &input, clock.restart(), app_closed };
}