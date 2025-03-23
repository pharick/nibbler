#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <utility>
#include <vector>
#include <exception>
#include <random>

#include "common_types.hpp"

/* SETTINGS */

typedef struct SnakeSettings {
    int fieldWidth;
    int fieldHeight;
    int startX;
    int startY;
    int startLength;
} SnakeSettings;

/* EXCEPTION */

class SnakeException final : public std::exception {
public:
    explicit SnakeException(std::string message) : message(std::move(message)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

/*************/

enum Direction {
    DIRECTION_RIGHT,
    DIRECTION_LEFT,
    DIRECTION_UP,
    DIRECTION_DOWN
};

class Snake {
public:
    explicit Snake(const SnakeSettings &settings);
    ~Snake() = default;

    Snake(const Snake& other) = default;
    Snake& operator=(const Snake& other) = default;
    Snake(Snake&& other) noexcept = default;
    Snake& operator=(Snake&& other) noexcept = default;

    void reset();
    void move(const Input &input);
    [[nodiscard]] const std::vector<Segment> &getSegments() const;
    [[nodiscard]] const Segment &getFood() const;

private:
    SnakeSettings settings;
    std::vector<Segment> segments;
    Segment food{};
    Direction direction{DIRECTION_RIGHT};

    std::mt19937 rng;
    std::uniform_int_distribution<> distWidth;
    std::uniform_int_distribution<> distHeight;

    void generateFood();
};

#endif // SNAKE_HPP
