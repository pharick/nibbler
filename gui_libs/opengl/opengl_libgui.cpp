#include "sdl_libgui.hpp"

SDL_LibGUI::SDL_LibGUI(const LibGUISettings& settings) : ALibGUI(settings)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw LibGuiException("Failed to initialize SDL");
    }

    window = SDL_CreateWindow(
        settings.window.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        settings.window.width,
        settings.window.height,
        SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        SDL_Quit();
        throw LibGuiException("Failed to create window");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw LibGuiException("Failed to create renderer");
    }
}

SDL_LibGUI::~SDL_LibGUI()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_LibGUI::SDL_LibGUI(SDL_LibGUI&& other) noexcept :
    ALibGUI(std::move(other)),
    window(other.window),
    renderer(other.renderer)
{
    other.window = nullptr;
    other.renderer = nullptr;
}

SDL_LibGUI& SDL_LibGUI::operator=(SDL_LibGUI&& other) noexcept
{
    if (this != &other)
    {
        if (window) SDL_DestroyWindow(window);
        if (renderer) SDL_DestroyRenderer(renderer);

        ALibGUI::operator=(std::move(other)); // Assuming ALibGUI has a move assignment operator

        window = other.window;
        renderer = other.renderer;

        other.window = nullptr;
        other.renderer = nullptr;
    }
    return *this;
}

void SDL_LibGUI::render(const std::vector<Segment>& snakeSegments, const Segment& food)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Render snake */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const auto& [x, y] : snakeSegments)
    {
        SDL_Rect rect = {
            x * getSettings().window.width / getSettings().fieldWidth,
            y * getSettings().window.height / getSettings().fieldHeight,
            getSettings().window.width / getSettings().fieldWidth + 1,
            getSettings().window.height / getSettings().fieldHeight + 1
        };
        SDL_RenderFillRect(renderer, &rect);
    }

    /* Render food */
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    const SDL_Rect rect = {
        food.x * getSettings().window.width / getSettings().fieldWidth,
        food.y * getSettings().window.height / getSettings().fieldHeight,
        getSettings().window.width / getSettings().fieldWidth,
        getSettings().window.height / getSettings().fieldHeight
    };
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}

const Input& SDL_LibGUI::handleInput()
{
    static Input input;
    input = {};

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            input.quit = true;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT:
                input.left = true;
                break;
            case SDLK_RIGHT:
                input.right = true;
                break;
            case SDLK_UP:
                input.up = true;
                break;
            case SDLK_DOWN:
                input.down = true;
                break;
            case SDLK_1:
                input.digits[1] = true;
                break;
            case SDLK_2:
                input.digits[2] = true;
                break;
            case SDLK_3:
                input.digits[3] = true;
                break;
            default: break;
            }
        }
    }

    return input;
}

extern "C" {
ALibGUI* createGuiLibrary(const LibGUISettings& settings)
{
    return new SDL_LibGUI(settings);
}

void destroyGuiLibrary(const ALibGUI* lib)
{
    delete lib;
}
}
