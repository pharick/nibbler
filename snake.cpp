#include "snake.hpp"

Snake::Snake(const SnakeSettings &settings) :
    settings(settings),
    direction(DIRECTION_RIGHT),
    rng(std::random_device{}()),
    distWidth(0, settings.fieldWidth - 1),
    distHeight(0, settings.fieldHeight - 1)
{
    if (settings.fieldWidth < 10 || settings.fieldHeight < 10) {
        throw SnakeException("Field size should be at least 10x10");
    }

    if (settings.startX < 0 || settings.startX >= settings.fieldWidth
        || settings.startY < 0 || settings.startY >= settings.fieldHeight) {
        throw SnakeException("Invalid start position");
    }

    if (settings.startLength > settings.fieldWidth / 2) {
        throw SnakeException("Start length cannot be greater than field width");
    }

    if (settings.startLength < 1) {
        throw SnakeException("Start length must be at least 1");
    }

    const Segment head = {settings.startX, settings.startY};
    segments.push_back(head);

    for (int i = 1; i < settings.startLength; i++) {
        Segment segment = {settings.startX - i, settings.startY};
        segments.push_back(segment);
    }

    generateFood();
}

void Snake::move(const Input &input) {
    if (input.left && direction != DIRECTION_RIGHT) {
        direction = DIRECTION_LEFT;
    } else if (input.right && direction != DIRECTION_LEFT) {
        direction = DIRECTION_RIGHT;
    } else if (input.up && direction != DIRECTION_DOWN) {
        direction = DIRECTION_UP;
    } else if (input.down && direction != DIRECTION_UP) {
        direction = DIRECTION_DOWN;
    }

    const Segment head = segments[0];
    Segment newHead = head;

    switch (direction) {
        case DIRECTION_RIGHT:
            newHead.x++;
            break;
        case DIRECTION_LEFT:
            newHead.x--;
            break;
        case DIRECTION_UP:
            newHead.y--;
            break;
        case DIRECTION_DOWN:
            newHead.y++;
            break;
    }

    if (newHead.x < 0 || newHead.x >= settings.fieldWidth
        || newHead.y < 0 || newHead.y >= settings.fieldHeight) {
        throw SnakeException("Snake hit the wall");
    }

    for (size_t i = 1; i < segments.size(); i++) {
        if (segments[i].x == newHead.x && segments[i].y == newHead.y) {
            throw SnakeException("Snake hit itself");
        }
    }

    segments.insert(segments.begin(), newHead);
    segments.pop_back();

    if (segments[0].x == food.x && segments[0].y == food.y) {
        const Segment tail = segments[segments.size() - 1];
        segments.push_back(tail);
        generateFood();
    }
}

void Snake::generateFood() {
    bool foodOnSnake;
    do {
        food.x = distWidth(rng);
        food.y = distHeight(rng);
        foodOnSnake = false;
        for (const auto &segment : segments) {
            if (segment.x == food.x && segment.y == food.y) {
                foodOnSnake = true;
                break;
            }
        }
    } while (foodOnSnake);
}

const std::vector<Segment> &Snake::getSegments() const {
    return segments;
}

const Segment &Snake::getFood() const {
    return food;
}
