#include "Asteroids.h"
#include "Render.h"
#include "System.h"

struct AppState
{
    std::unique_ptr<Resources> resources;
    GameState state;
};

AppState InitApp()
{
    return {
        .resources = std::make_unique<Resources>(LoadResources()),
        .state = GoToMenu(),
    };
}

AppState UpdateApp(AppState&& app, System::State system)
{
    app.state = Update(app.state, *system.input, system.dt);
    Draw(*system.window, app.state, *app.resources);
    return app;
}

int ReturnCode(const AppState&) { return 0; }

int main()
{
    return ReturnCode(stdr::fold_left(GetSystemState(), InitApp(), UpdateApp));
}