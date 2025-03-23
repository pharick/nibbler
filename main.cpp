#include <iostream>
#include <chrono>
#include <thread>
#include <dlfcn.h>

#include <snake.hpp>
#include <libgui.hpp>

typedef ALibGUI* (*CreateGuiLibrary)(const LibGUISettings&);
typedef void (*DestroyGuiLibrary)(ALibGUI*);

constexpr SnakeSettings snakeSettings = {
    .fieldWidth = 40,
    .fieldHeight = 40,
    .startX = 5,
    .startY = 5,
    .startLength = 10
};

const LibGUISettings guiSettings = {
    .window = {
        .width = 1024,
        .height = 768,
        .title = "Snake",
    },
    .fieldWidth = snakeSettings.fieldWidth,
    .fieldHeight = snakeSettings.fieldHeight
};

const std::vector guiLibFilenames{
    "./gui_libs/opengl/opengl_libgui.so",
    "./gui_libs/sdl/sdl_libgui.so",
    "./gui_libs/sfml/sfml_libgui.so",
};
auto currentGuiLibFilename = *guiLibFilenames.begin();

ALibGUI* loadGuiLibrary(const std::string& filename, const LibGUISettings& settings,
                        void*& libHandle, CreateGuiLibrary& createGuiLibrary, DestroyGuiLibrary& destroyGuiLibrary)
{
    libHandle = dlopen(filename.c_str(), RTLD_LAZY);
    if (!libHandle)
    {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return nullptr;
    }

    createGuiLibrary = reinterpret_cast<CreateGuiLibrary>(dlsym(libHandle, "createGuiLibrary"));
    destroyGuiLibrary = reinterpret_cast<DestroyGuiLibrary>(dlsym(libHandle, "destroyGuiLibrary"));
    if (const char* dlsymError = dlerror())
    {
        std::cerr << "Failed to load library " << filename << ": " << dlsymError << std::endl;
        dlclose(libHandle);
        return nullptr;
    }

    return createGuiLibrary(settings);
}

bool switchLibrary(const bool* dp, const Input& input, ALibGUI*& libGui, void*& libHandle,
                   CreateGuiLibrary& createGuiLibrary, DestroyGuiLibrary& destroyGuiLibrary)
{
    if (const size_t d = dp - input.digits - 1; d >= guiLibFilenames.size())
    {
        std::cerr << "Invalid library index: " << d << std::endl;
    }
    else if (currentGuiLibFilename == guiLibFilenames[d])
    {
        std::cerr << "Already using library: " << currentGuiLibFilename << std::endl;
    }
    else
    {
        currentGuiLibFilename = guiLibFilenames[d];
        std::cout << "Switching to library: " << currentGuiLibFilename << std::endl;

        destroyGuiLibrary(libGui);
        dlclose(libHandle);

        libGui = loadGuiLibrary(currentGuiLibFilename, guiSettings, libHandle, createGuiLibrary, destroyGuiLibrary);
        if (!libGui)
        {
            std::cerr << "Failed to load library: " << currentGuiLibFilename << std::endl;
            return false;
        }
    }
    return true;
}

int main()
{
    void* libHandle;
    CreateGuiLibrary createGuiLibrary;
    DestroyGuiLibrary destroyGuiLibrary;
    auto libGui = loadGuiLibrary(currentGuiLibFilename, guiSettings, libHandle,
                                 createGuiLibrary, destroyGuiLibrary);
    if (!libGui)
    {
        std::cerr << "Failed to load library: " << currentGuiLibFilename << std::endl;
        return EXIT_FAILURE;
    }

    Snake *snake = nullptr;
    try
    {
        snake = new Snake(snakeSettings);
    }
    catch (const SnakeException& e)
    {
        std::cerr << "Failed to create snake: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    bool running = true;
    while (running)
    {
        const Input& input = libGui->handleInput();

        if (input.quit)
        {
            running = false;
        }

        if (const auto dp = std::find(input.digits, input.digits + 10, true); dp != input.digits + 10)
        {
            if (!switchLibrary(dp, input, libGui, libHandle, createGuiLibrary, destroyGuiLibrary))
            {
                return EXIT_FAILURE;
            }
        }

        try
        {
            snake->move(input);
        }
        catch (const SnakeException& e)
        {
            std::cerr << "Game over: " << e.what() << std::endl;
            snake->reset();
        }

        libGui->render(snake->getSegments(), snake->getFood());
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }

    destroyGuiLibrary(libGui);
    dlclose(libHandle);
    delete snake;

    return EXIT_SUCCESS;
}
