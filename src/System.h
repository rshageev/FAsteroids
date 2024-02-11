#pragma once

#include "Utils.h"
#include "functional/GenerateView.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <set>
#include <memory>


namespace sf {
    class RenderWindow;
}

struct InputState
{
    std::set<sf::Keyboard::Key> keys;
};

struct System
{
public:
    struct State
    {
        sf::RenderWindow* window;
        const InputState* input;
        sf::Time dt;
        bool app_closed;
    };

    System();
    System(System&&);

    ~System();

    State operator()();

    static bool IsRunning(const State& st) { return !st.app_closed; }
private:
    std::unique_ptr<sf::RenderWindow> window;
    InputState input;
    sf::Clock clock;
    bool app_closed = false;
};

inline auto GetSystemState()
{
    return views::generate(System{}) | stdv::take_while(System::IsRunning);
}