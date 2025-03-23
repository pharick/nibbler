#include "opengl_libgui.hpp"

#include <iostream>

OpenGL_LibGUI::OpenGL_LibGUI(const LibGUISettings& settings) : ALibGUI{settings}, window{nullptr}, input{}
{
    if (!glfwInit())
    {
        throw LibGuiException("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(settings.window.width, settings.window.height, settings.window.title.c_str(),
                              nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw LibGuiException("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (const auto err = glewInit(); err != GLEW_OK)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw LibGuiException("Failed to initialize GLEW");
    }

    glfwSetKeyCallback(window, keyCallback);
}

OpenGL_LibGUI::~OpenGL_LibGUI()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

OpenGL_LibGUI::OpenGL_LibGUI(OpenGL_LibGUI&& other) noexcept :
    ALibGUI(std::move(other)),
    window(other.window),
    input()
{
    other.window = nullptr;
}

OpenGL_LibGUI& OpenGL_LibGUI::operator=(OpenGL_LibGUI&& other) noexcept
{
    if (this != &other)
    {
        if (window) glfwDestroyWindow(window);

        ALibGUI::operator=(std::move(other));

        window = other.window;
        other.window = nullptr;
    }
    return *this;
}

void OpenGL_LibGUI::render(const std::vector<Segment>& snakeSegments, const Segment& food)
{
    (void)snakeSegments;
    (void)food;
}

Input OpenGL_LibGUI::handleInput()
{
    glfwPollEvents();
    if (glfwWindowShouldClose(window))
    {
        std::cout << "Quit" << std::endl;
        input.quit = true;
    }
    const auto currentInput = input;
    input = {};
    return currentInput;
}

void OpenGL_LibGUI::keyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
{
    (void)scancode;
    (void)mods;
    auto& input = static_cast<OpenGL_LibGUI*>(glfwGetWindowUserPointer(window))->input;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_UP:
            input.up = true;
            break;
        case GLFW_KEY_DOWN:
            input.down = true;
            break;
        case GLFW_KEY_LEFT:
            input.left = true;
            break;
        case GLFW_KEY_RIGHT:
            input.right = true;
            break;
        case GLFW_KEY_1:
            input.digits[1] = true;
            break;
        case GLFW_KEY_2:
            input.digits[2] = true;
            break;
        case GLFW_KEY_3:
            input.digits[3] = true;
            break;
        default:
            break;
        }
    }
}

extern "C" {
ALibGUI* createGuiLibrary(const LibGUISettings& settings)
{
    return new OpenGL_LibGUI(settings);
}

void destroyGuiLibrary(const ALibGUI* lib)
{
    delete lib;
}
}
