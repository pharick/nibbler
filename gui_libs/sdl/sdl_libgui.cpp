#include "sdl_libgui.hpp"

SDL_LibGUI::SDL_LibGUI(const LibGUISettings &settings) : ALibGUI(settings) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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

    if (!window) {
        SDL_Quit();
        throw LibGuiException("Failed to create window");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw LibGuiException("Failed to create renderer");
    }
}

SDL_LibGUI::~SDL_LibGUI() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDL_LibGUI::render(const std::vector<Segment> &snakeSegments, const Segment &food) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Render snake */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const Segment &segment : snakeSegments) {
        SDL_Rect rect = {
            segment.x * getSettings().window.width / getSettings().fieldWidth,
            segment.y * getSettings().window.height / getSettings().fieldHeight,
            getSettings().window.width / getSettings().fieldWidth,
            getSettings().window.height / getSettings().fieldHeight
        };
        SDL_RenderFillRect(renderer, &rect);
    }

    /* Render food */
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {
        food.x * getSettings().window.width / getSettings().fieldWidth,
        food.y * getSettings().window.height / getSettings().fieldHeight,
        getSettings().window.width / getSettings().fieldWidth,
        getSettings().window.height / getSettings().fieldHeight
    };
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}

const Input &SDL_LibGUI::handleInput() {
    static Input input;
    input.quit = false;
    input.left = false;
    input.right = false;
    input.up = false;
    input.down = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            input.quit = true;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
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
                default: break;
            }
        }
    }

    return input;
}

extern "C" {
    ALibGUI* createGuiLibrary(const LibGUISettings &settings) {
        return new SDL_LibGUI(settings);
    }

    void destroyGuiLibrary(const ALibGUI* lib) {
        delete lib;
    }
}
