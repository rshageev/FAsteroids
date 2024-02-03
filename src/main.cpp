#include <SFML/Graphics.hpp>

#include "Asteroids.h"
#include "Render.h"
#include "Settings.h"
#include "Utils.h"
#include "functional/GenerateView.h"

#include <functional>

FrameData UpdateFrame(FrameData frame, const auto& frame_input)
{
    const auto& [events, dt] = frame_input;

    for (const auto& event : events)
    {
        if (event.type == sf::Event::KeyPressed) {
            frame.keys.insert(event.key.code);
        }
        if (event.type == sf::Event::KeyReleased) {
            frame.keys.erase(event.key.code);
        }
    }

    frame.dt = dt;

    return frame;
}

std::vector<sf::Event> PollEvents(sf::Window& window)
{
    std::vector<sf::Event> events;
    for (auto event = sf::Event{}; window.pollEvent(event);) {
        events.push_back(event);
    }
    return events;
}

auto GetFramesDeltaTimes()
{
    return views::generate([clock = sf::Clock{}]() mutable { return clock.restart(); });
}

auto GetFramesInputEvents(sf::Window& window)
{
    auto close_not_requested = [](const auto& events) {
        return stdr::find(events, sf::Event::Closed, &sf::Event::type) == stdr::end(events);
    };

    return views::generate([&window]() { return PollEvents(window); })
        | stdv::take_while(close_not_requested);
}

struct ProgramState
{
    AppState state;
    FrameData frame;
};

ProgramState UpdateProgram(sf::RenderWindow& window, const Resources& resources, ProgramState program_state, const auto& frame_input)
{
    program_state.frame = UpdateFrame(program_state.frame, frame_input);
    program_state.state = Update(program_state.state, program_state.frame);
    Draw(window, program_state.state, resources);
    return program_state;
}

int main()
{
    auto window = sf::RenderWindow{ sf::VideoMode{ settings.window_size }, "Asteroids" };
    window.setVerticalSyncEnabled(true);

    const auto resources = LoadResources();

    auto frames = stdv::zip(
        GetFramesInputEvents(window),
        GetFramesDeltaTimes());

    [[maybe_unused]] const auto final_state = stdr::fold_left(
        frames,
        ProgramState{},
        [&](auto&& state, const auto& frame) {
            return UpdateProgram(window, resources, state, frame);
        });

    window.close();
    return 0;
}