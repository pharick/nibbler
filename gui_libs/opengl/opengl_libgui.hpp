#ifndef OPENGL_LIBGUI_HPP
#define OPENGL_LIBGUI_HPP

#include "libgui.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class OpenGL_LibGUI final : public ALibGUI
{
public:
    explicit OpenGL_LibGUI(const LibGUISettings& settings);
    ~OpenGL_LibGUI() override;

    OpenGL_LibGUI(const OpenGL_LibGUI& other) = delete;
    OpenGL_LibGUI& operator=(const OpenGL_LibGUI& other) = delete;
    OpenGL_LibGUI(OpenGL_LibGUI&& other) noexcept;
    OpenGL_LibGUI& operator=(OpenGL_LibGUI&& other) noexcept;

    void render(const std::vector<Segment>& snakeSegments, const Segment& food) override;
    Input handleInput() override;

private:
    GLFWwindow* window;
    Input input;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif // OPENGL_LIBGUI_HPP
