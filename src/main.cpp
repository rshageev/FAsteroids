#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow{ sf::VideoMode{ { 1920u, 1080u } }, "Asteroids" };
    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        window.display();
    }
}