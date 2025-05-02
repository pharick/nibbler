#include "sfml_libgui.hpp"

SFML_LibGUI::SFML_LibGUI(const LibGUISettings& settings)
    : ALibGUI(settings), windowWidth(settings.window.initWidth), windowHeight(settings.window.initHeight)
{
    window.create(
        sf::VideoMode({
            static_cast<unsigned>(windowWidth),
            static_cast<unsigned>(windowHeight)
        }),
        "Snake SFML",
        sf::Style::Default | sf::Style::Resize
    );
    window.setFramerateLimit(60);

    view = window.getDefaultView();
    window.setView(view);
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
            static_cast<float>(windowWidth) / static_cast<float>(getSettings().fieldWidth) + 1,
            static_cast<float>(windowHeight) / static_cast<float>(getSettings().fieldHeight) + 1
        )
    );
    segmentShape.setFillColor(sf::Color::Green);

    for (const auto& [x, y] : snakeSegments)
    {
        segmentShape.setPosition({
            static_cast<float>(x * windowWidth) / static_cast<float>(getSettings().fieldWidth),
            static_cast<float>(y * windowHeight) / static_cast<float>(getSettings().fieldHeight)
        });
        window.draw(segmentShape);
    }
    (void)food;
    segmentShape.setFillColor(sf::Color::Red);
    segmentShape.setPosition({
        static_cast<float>(food.x * windowWidth) / static_cast<float>(getSettings().fieldWidth),
        static_cast<float>(food.y * windowHeight) / static_cast<float>(getSettings().fieldHeight)
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
        else if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            windowWidth = static_cast<int>(resized->size.x);
            windowHeight = static_cast<int>(resized->size.y);
            window.setView(sf::View(sf::FloatRect(
                {0.0f, 0.0f},
                {static_cast<float>(windowWidth), static_cast<float>(windowHeight)}
            )));
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            switch (keyPressed->code)
            {
            case sf::Keyboard::Key::Left:
                input.left = true;
                break;
            case sf::Keyboard::Key::Right:
                input.right = true;
                break;
            case sf::Keyboard::Key::Up:
                input.up = true;
                break;
            case sf::Keyboard::Key::Down:
                input.down = true;
                break;
            case sf::Keyboard::Key::Num1:
                input.digits[1] = true;
                break;
            case sf::Keyboard::Key::Num2:
                input.digits[2] = true;
                break;
            case sf::Keyboard::Key::Num3:
                input.digits[3] = true;
                break;
            case sf::Keyboard::Key::Escape:
                input.quit = true;
            default: break;
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
