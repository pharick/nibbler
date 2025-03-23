#include "sfml_libgui.hpp"

SFML_LibGUI::SFML_LibGUI(const LibGUISettings& settings) : ALibGUI(settings)
{
    window.create(sf::VideoMode({
                      static_cast<unsigned>(settings.window.width),
                      static_cast<unsigned>(settings.window.height)
                  }), settings.window.title);
    window.setFramerateLimit(60);
    window.setView(window.getDefaultView());
}

SFML_LibGUI::~SFML_LibGUI()
{
    window.close();
}

void SFML_LibGUI::render(const std::vector<Segment>& snakeSegments, const Segment& food)
{
    window.clear(sf::Color::Black);

    sf::RectangleShape segmentShape(
        sf::Vector2f(
            static_cast<float>(getSettings().window.width) / static_cast<float>(getSettings().fieldWidth) + 1,
            static_cast<float>(getSettings().window.height) / static_cast<float>(getSettings().fieldHeight) + 1
        )
    );
    segmentShape.setFillColor(sf::Color::Green);

    for (const auto& [x, y] : snakeSegments)
    {
        segmentShape.setPosition({
            static_cast<float>(x * getSettings().window.width) / static_cast<float>(getSettings().fieldWidth),
            static_cast<float>(y * getSettings().window.height) / static_cast<float>(getSettings().fieldHeight)
        });
        window.draw(segmentShape);
    }
    (void)food;
    segmentShape.setFillColor(sf::Color::Red);
    segmentShape.setPosition({
        static_cast<float>(food.x * getSettings().window.width) / static_cast<float>(getSettings().fieldWidth),
        static_cast<float>(food.y * getSettings().window.height) / static_cast<float>(getSettings().fieldHeight)
    });
    window.draw(segmentShape);

    window.display();
}

Input SFML_LibGUI::handleInput()
{
    Input input{};

    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            input.quit = true;
        }

        if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPress->code == sf::Keyboard::Key::Left)
            {
                input.left = true;
            }
            if (keyPress->code == sf::Keyboard::Key::Right)
            {
                input.right = true;
            }
            if (keyPress->code == sf::Keyboard::Key::Up)
            {
                input.up = true;
            }
            if (keyPress->code == sf::Keyboard::Key::Down)
            {
                input.down = true;
            }
            if (keyPress->code == sf::Keyboard::Key::Num1)
            {
                input.digits[1] = true;
            }
            if (keyPress->code == sf::Keyboard::Key::Num2)
            {
                input.digits[2] = true;
            }
            if (keyPress->code == sf::Keyboard::Key::Num3)
            {
                input.digits[3] = true;
            }
        }
    }

    return input;
}

extern "C" {
ALibGUI* createGuiLibrary(const LibGUISettings& settings)
{
    return new SFML_LibGUI(settings);
}

void destroyGuiLibrary(const ALibGUI* lib)
{
    delete lib;
}
}
