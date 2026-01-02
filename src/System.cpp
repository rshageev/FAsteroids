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

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

System::State System::operator()()
{
    const auto visitor = overloads{
        [this](const sf::Event::KeyPressed& event) {
            input.keys.insert(event.code);
        },
        [this](const sf::Event::KeyReleased& event) {
            input.keys.erase(event.code);
        },
        [this](const sf::Event::Closed) {
            app_closed = true;
        },
        [](const auto& event) {}
    };

    while (const auto event = window->pollEvent()) {
        event->visit(visitor);
    }

    return State{ window.get(), &input, clock.restart(), app_closed };
}