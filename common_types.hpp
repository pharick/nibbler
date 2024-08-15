#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

typedef struct Segment {
    int x;
    int y;
} Segment;

typedef struct Input {
    bool quit;
    bool left;
    bool right;
    bool up;
    bool down;
} Input;

#endif // COMMON_TYPES_HPP
