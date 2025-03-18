#ifndef SDL_LIBGUI_HPP
#define SDL_LIBGUI_HPP

#include "libgui.hpp"

#include <SDL.h>

class SDL_LibGUI final : public ALibGUI
{
public:
    explicit SDL_LibGUI(const LibGUISettings& settings);
    ~SDL_LibGUI() override;

    SDL_LibGUI(const SDL_LibGUI& other) = delete;
    SDL_LibGUI& operator=(const SDL_LibGUI& other) = delete;
    SDL_LibGUI(SDL_LibGUI&& other) noexcept;
    SDL_LibGUI& operator=(SDL_LibGUI&& other) noexcept;

    void render(const std::vector<Segment>& snakeSegments, const Segment& food) override;
    const Input& handleInput() override;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif // SDL_LIBGUI_HPP
