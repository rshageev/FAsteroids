#include <SFML/Graphics/RenderWindow.hpp>

#include "Asteroids.h"
#include "Render.h"
#include "Settings.h"
#include "Utils.h"
#include "functional/GenerateView.h"

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

    System()
    {
        window = std::make_unique<sf::RenderWindow>(sf::VideoMode{ settings.window_size }, "Asteroids");
        window->setVerticalSyncEnabled(true);
    }

    auto operator()()
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

    static bool IsRunning(const State& st) { return !st.app_closed; }
private:
    std::unique_ptr<sf::RenderWindow> window;
    InputState input;
    sf::Clock clock;
    bool app_closed = false;
};

auto GetSystemState()
{
    return views::generate(System{}) | stdv::take_while(System::IsRunning);
}

struct ProgramState
{
    std::unique_ptr<Resources> resources;
    AppState state;
    int return_code = 0;
};

ProgramState InitApp()
{
    return {
        .resources = std::make_unique<Resources>(LoadResources()),
    };
}

ProgramState UpdateApp(ProgramState&& app, System::State system)
{
    app.state = Update(app.state, *system.input, system.dt);
    Draw(*system.window, app.state, *app.resources);
    return app;
}

int main()
{
    return stdr::fold_left(GetSystemState(), InitApp(), UpdateApp).return_code;
}