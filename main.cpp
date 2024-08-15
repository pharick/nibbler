#include <iostream>
#include <dlfcn.h>
#include <chrono>
#include <thread>

#include <snake.hpp>
#include <libgui.hpp>

typedef ALibGUI* (*CreateGuiLibrary)(const LibGUISettings&);
typedef void (*DestroyGuiLibrary)(ALibGUI*);

typedef struct GameState {
    int x;
    int y;
} GameState;

int main() {
    const auto libName = "./sdl_libgui.so";

    void* lib = dlopen(libName, RTLD_LAZY);
    if (!lib) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return 1;
    }

    const auto createGuiLibrary = reinterpret_cast<CreateGuiLibrary>(dlsym(lib, "createGuiLibrary"));
    const auto destroyGuiLibrary = reinterpret_cast<DestroyGuiLibrary>(dlsym(lib, "destroyGuiLibrary"));

    const char* dlsymError = dlerror();
    if (dlsymError) {
        std::cerr << "Failed to load symbols: " << dlsymError << std::endl;
        dlclose(lib);
        return 1;
    }

    constexpr SnakeSettings snakeSettings = {
        .fieldWidth = 40,
        .fieldHeight = 40,
        .startX = 5,
        .startY = 5,
        .startLength = 10
    };

    const LibGUISettings guiSettings = {
        .window = {
            .width = 800,
            .height = 600,
            .title = "Snake",
        },
        .fieldWidth = snakeSettings.fieldWidth,
        .fieldHeight = snakeSettings.fieldHeight
    };

    Snake snake(snakeSettings);
    ALibGUI* libGui = createGuiLibrary(guiSettings);

    bool running = true;
    while (running) {
        const Input &input = libGui->handleInput();
        std::cout << "Input: quit=" << input.quit << ", left=" << input.left << ", right=" << input.right << std::endl;
        if (input.quit) {
            running = false;
        }
        snake.move(input);
        libGui->render(snake.getSegments(), snake.getFood());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    destroyGuiLibrary(libGui);
    dlclose(lib);

    return 0;
}
