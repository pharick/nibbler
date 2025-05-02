#include "opengl_libgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#define FOV 45.0f
#define Z_NEAR 1.0f
#define Z_FAR 100.0f
#define Z_DISTANCE 5.0f

constexpr float cubeVertexPositions[]{
    // Front face (z = +1) - clockwise from outside
    -1.0f, -1.0f, 1.0f,  // bottom-left
    1.0f, -1.0f, 1.0f,   // bottom-right
    1.0f, 1.0f, 1.0f,    // top-right

    -1.0f, -1.0f, 1.0f,  // bottom-left
    1.0f, 1.0f, 1.0f,    // top-right
    -1.0f, 1.0f, 1.0f,   // top-left

    // Back face (z = -1) - clockwise from outside
    1.0f, -1.0f, -1.0f,  // bottom-right
    -1.0f, -1.0f, -1.0f, // bottom-left
    -1.0f, 1.0f, -1.0f,  // top-left

    1.0f, -1.0f, -1.0f,  // bottom-right
    -1.0f, 1.0f, -1.0f,  // top-left
    1.0f, 1.0f, -1.0f,   // top-right

    // Left face (x = -1) - clockwise from outside
    -1.0f, -1.0f, -1.0f, // bottom-back
    -1.0f, -1.0f, 1.0f,  // bottom-front
    -1.0f, 1.0f, 1.0f,   // top-front

    -1.0f, -1.0f, -1.0f, // bottom-back
    -1.0f, 1.0f, 1.0f,   // top-front
    -1.0f, 1.0f, -1.0f,  // top-back

    // Right face (x = +1) - clockwise from outside
    1.0f, -1.0f, 1.0f,   // bottom-front
    1.0f, -1.0f, -1.0f,  // bottom-back
    1.0f, 1.0f, -1.0f,   // top-back

    1.0f, -1.0f, 1.0f,   // bottom-front
    1.0f, 1.0f, -1.0f,   // top-back
    1.0f, 1.0f, 1.0f,    // top-front

    // Top face (y = +1) - clockwise from outside
    -1.0f, 1.0f, 1.0f,   // front-left
    1.0f, 1.0f, 1.0f,    // front-right
    1.0f, 1.0f, -1.0f,   // back-right

    -1.0f, 1.0f, 1.0f,   // front-left
    1.0f, 1.0f, -1.0f,   // back-right
    -1.0f, 1.0f, -1.0f,  // back-left

    // Bottom face (y = -1) - clockwise from outside
    -1.0f, -1.0f, -1.0f, // back-left
    1.0f, -1.0f, -1.0f,  // back-right
    1.0f, -1.0f, 1.0f,   // front-right

    -1.0f, -1.0f, -1.0f, // back-left
    1.0f, -1.0f, 1.0f,   // front-right
    -1.0f, -1.0f, 1.0f   // front-left
};

OpenGL_LibGUI::OpenGL_LibGUI(const LibGUISettings& settings)
    : ALibGUI{settings}, windowWidth(settings.window.initWidth), windowHeight(settings.window.initHeight)
{
    if (!glfwInit())
    {
        throw LibGuiException("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(windowWidth, windowHeight, "Snake OpenGL", nullptr, nullptr);
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

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    prepareState();
}

OpenGL_LibGUI::~OpenGL_LibGUI()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

OpenGL_LibGUI::OpenGL_LibGUI(OpenGL_LibGUI&& other) noexcept :
    ALibGUI(std::move(other)),
    window(other.window),
    program(other.program),
    projectionMatrixLocation(other.projectionMatrixLocation),
    modelMatrixLocation(other.modelMatrixLocation)
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

GLuint OpenGL_LibGUI::loadShader(const GLenum shaderType, const std::string& filename)
{
    std::ifstream shaderFile(filename);
    if (!shaderFile.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 0;
    }

    std::stringstream shaderData;
    shaderData << shaderFile.rdbuf();
    shaderFile.close();
    auto shaderStr = shaderData.str();
    auto shaderStrData = shaderStr.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderStrData, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        auto infoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
        std::cerr << "Compile failure in " << filename << ": " << infoLog << std::endl;
        delete[] infoLog;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint OpenGL_LibGUI::createProgram(const std::vector<GLuint>& shaders)
{
    const auto program = glCreateProgram();
    for (const auto shader : shaders)
    {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        const auto infoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog);
        std::cerr << "Linker failure: " << infoLog << std::endl;
        delete[] infoLog;
        glDeleteProgram(program);
        return 0;
    }

    for (const auto shader : shaders)
    {
        glDetachShader(program, shader);
    }

    return program;
}

void OpenGL_LibGUI::prepareState()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    const auto vertexShader = loadShader(GL_VERTEX_SHADER, "./gui_libs/opengl/shaders/shader.vert");
    if (!vertexShader)
    {
        throw LibGuiException("Failed to load vertex shader");
    }

    const auto fragmentShader = loadShader(GL_FRAGMENT_SHADER, "./gui_libs/opengl/shaders/shader.frag");
    if (!fragmentShader)
    {
        glDeleteShader(vertexShader);
        throw LibGuiException("Failed to load fragment shader");
    }

    const std::vector shaders = {vertexShader, fragmentShader};
    program = createProgram(shaders);
    if (!program)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw LibGuiException("Failed to create shader program");
    }

    projectionMatrixLocation = glGetUniformLocation(program, "projectionMatrix");
    modelMatrixLocation = glGetUniformLocation(program, "modelMatrix");
    segmentNumberLocation = glGetUniformLocation(program, "segmentNumber");

    auto projectionMatrix = glm::perspective(glm::radians(FOV),
                                             static_cast<float>(windowWidth) /
                                             static_cast<float>(windowHeight),
                                             Z_NEAR, Z_FAR);

    glUseProgram(program);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));
    glUseProgram(0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void OpenGL_LibGUI::render(const std::vector<Segment>& snakeSegments, const Segment& food)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int n = 1;
    for (const auto& segment : snakeSegments)
    {
        renderSegment(segment, n++);
    }
    renderSegment(food, 0);

    glDisableVertexAttribArray(0);
    glUseProgram(0);

    glfwSwapBuffers(window);
}

void OpenGL_LibGUI::renderSegment(const Segment& segment, const int n) const
{
    constexpr float fov = glm::radians(FOV);
    const float aspect =
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    const float visibleHeight = 2.0f * tan(fov / 2.0f) * Z_DISTANCE;
    const float visibleWidth = visibleHeight * aspect;
    const float cellWidth = visibleWidth / static_cast<float>(getSettings().fieldWidth);
    const float cellHeight = visibleHeight / static_cast<float>(getSettings().fieldHeight);

    const float ndcX = -visibleWidth / 2.0f + cellWidth * (static_cast<float>(segment.x) + 0.5f);
    const float ndcY = visibleHeight / 2.0f - cellHeight * (static_cast<float>(segment.y) + 0.5f);

    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = translate(modelMatrix, glm::vec3(ndcX, ndcY, -Z_DISTANCE));
    modelMatrix = scale(modelMatrix, glm::vec3(cellWidth * 0.5f, cellHeight * 0.5f, cellWidth * 0.5f));

    glUniform1iv(segmentNumberLocation, 1, &n);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(modelMatrix));

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Input OpenGL_LibGUI::handleInput()
{
    glfwPollEvents();
    if (glfwWindowShouldClose(window))
    {
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
        case GLFW_KEY_ESCAPE:
            input.quit = true;
            break;
        default:
            break;
        }
    }
}

void OpenGL_LibGUI::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    OpenGL_LibGUI* instance = static_cast<OpenGL_LibGUI*>(glfwGetWindowUserPointer(window));
    instance->windowWidth = width;
    instance->windowHeight = height;
    glViewport(0, 0, width, height);
    
    auto projectionMatrix = glm::perspective(glm::radians(FOV),
                                             static_cast<float>(width) /
                                             static_cast<float>(height),
                                             Z_NEAR, Z_FAR);

    glUseProgram(instance->program);
    glUniformMatrix4fv(instance->projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));
    glUseProgram(0);
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
