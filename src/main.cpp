#include <SFML/Graphics.hpp>

#include "Asteroids.h"
#include "Render.h"
#include "Settings.h"
#include "Utils.h"
#include "functional/GenerateView.h"

#include <functional>

InputState UpdateInputState(InputState input, const auto& system_events)
{
    for (const auto& event : system_events) {
        if (event.type == sf::Event::KeyPressed) {
            input.keys.insert(event.key.code);
        }
        if (event.type == sf::Event::KeyReleased) {
            input.keys.erase(event.key.code);
        }
    }
    return input;
}

std::vector<sf::Event> PollEvents(sf::Window& window)
{
    std::vector<sf::Event> events;
    for (auto event = sf::Event{}; window.pollEvent(event);) {
        events.push_back(event);
    }
    return events;
}

auto GetDeltaTimes()
{
    return views::generate([clock = sf::Clock{}]() mutable { return clock.restart(); });
}

auto GetSystemEvents(sf::Window& window)
{
    auto close_not_requested = [](const auto& events) {
        return !stdr::contains(events, sf::Event::Closed, &sf::Event::type);
    };

    return views::generate([&window]() { return PollEvents(window); })
        | stdv::take_while(close_not_requested);
}

struct ProgramState
{
    std::unique_ptr<Resources> resources;
    AppState state;
    InputState input;
    int ret_code = 0;
};

ProgramState InitProgram()
{
    return {
        .resources = std::make_unique<Resources>(LoadResources()),
    };
}

ProgramState UpdateProgram(sf::RenderWindow& window, ProgramState&& ps, const auto& frame_input)
{
    const auto& [events, dt] = frame_input;
    ps.input = UpdateInputState(ps.input, events);
    ps.state = Update(ps.state, ps.input, dt);
    Draw(window, ps.state, *ps.resources);
    return ps;
}

int main()
{
    auto window = sf::RenderWindow{ sf::VideoMode{ settings.window_size }, "Asteroids" };
    window.setVerticalSyncEnabled(true);

    return stdr::fold_left(
        stdv::zip(GetSystemEvents(window), GetDeltaTimes()),
        InitProgram(),
        [&](auto&& state, const auto& frame) {
            return UpdateProgram(window, std::move(state), frame);
        }
    ).ret_code;
}