#ifndef SFML_LIBGUI_HPP
#define SFML_LIBGUI_HPP

#include "libgui.hpp"

#include <SFML/Graphics.hpp>

class SFML_LibGUI final : public ALibGUI
{
public:
    explicit SFML_LibGUI(const LibGUISettings& settings);
    ~SFML_LibGUI() override;

    SFML_LibGUI(const SFML_LibGUI& other) = delete;
    SFML_LibGUI& operator=(const SFML_LibGUI& other) = delete;
    SFML_LibGUI(SFML_LibGUI&& other) noexcept = delete;
    SFML_LibGUI& operator=(SFML_LibGUI&& other) = delete;

    void render(const std::vector<Segment>& snakeSegments, const Segment& food) override;
    Input handleInput() override;

private:
    sf::RenderWindow window;
    sf::View view;
    int windowWidth;
    int windowHeight;
};

#endif // SFML_LIBGUI_HPP
