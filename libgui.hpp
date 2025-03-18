#ifndef LIBGUI_HPP
#define LIBGUI_HPP

#include <exception>
#include <string>
#include <vector>

#include "common_types.hpp"

/* SETTINGS */

typedef struct LibGUIWindowSettings {
    int width;
    int height;
    std::string title;
} LibGUIWindowSettings;

typedef struct LibGUISettings {
    LibGUIWindowSettings window;
    int fieldWidth;
    int fieldHeight;
} LibGUISettings;

/* EXCEPTION */

class LibGuiException final : public std::exception {
public:
    explicit LibGuiException(std::string message) : message(std::move(message)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

/*************/

class ALibGUI {
public:
    explicit ALibGUI(LibGUISettings  settings) : settings(std::move(settings)) {}
    virtual ~ALibGUI() = default;

    ALibGUI(const ALibGUI& other) = delete;
    ALibGUI& operator=(const ALibGUI& other) = delete;
    ALibGUI(ALibGUI&& other) noexcept = default;
    ALibGUI& operator=(ALibGUI&& other) noexcept = default;

    virtual void render(const std::vector<Segment> &snakeSegments, const Segment &food) = 0;
    virtual const Input &handleInput() = 0;

    [[nodiscard]] const LibGUISettings &getSettings() const {
        return settings;
    }

private:
    LibGUISettings settings;
};

extern "C" {
    ALibGUI* createGuiLibrary(const LibGUISettings &settings);
    void destroyGuiLibrary(const ALibGUI* lib);
}

#endif // LIBGUI_HPP
