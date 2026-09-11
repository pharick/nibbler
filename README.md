# nibbler

A Snake game in C++20 whose rendering lives in three interchangeable dynamic libraries (SDL2, SFML 3, OpenGL) that are loaded with `dlopen` and can be swapped while the game is running.

The executable contains only the game logic and the library loader. Each renderer is a separately compiled shared object that implements one small abstract interface and exposes two C factory symbols. Pressing `1`, `2` or `3` during play unloads the current renderer and loads another one; the snake, the food and the board carry on where they were.

## Highlights

- **Renderer interface behind a C ABI.** `libgui.hpp` declares the abstract class `ALibGUI` (`render(segments, food)`, `handleInput()`) plus two `extern "C"` functions, `createGuiLibrary` and `destroyGuiLibrary`. The executable only ever touches the abstract class and those two symbols, so no mangled C++ symbol crosses the library boundary and each renderer is built in complete isolation.
- **Hot-swapping without losing state.** `switchLibrary` in `main.cpp` destroys the current renderer through the library's own `destroyGuiLibrary`, `dlclose`s the handle, then `dlopen`/`dlsym`s the next library. The `Snake` object is untouched, so a switch is invisible to the game.
- **Game logic knows nothing about graphics.** `Snake` operates on a `std::vector<Segment>` of grid cells and talks to renderers only through two POD structs (`Segment`, `Input`) in `common_types.hpp`. Hitting a wall or the snake's own body throws `SnakeException`; the main loop catches it and resets the board.
- **Food placement** uses `std::mt19937` seeded from `std::random_device` with one `uniform_int_distribution` per axis, re-rolling until the chosen cell is not occupied by the snake.
- **OpenGL renderer on the 3.3 core profile.** Every cell is a unit cube (36 vertices in one VBO) placed with GLM model and perspective matrices so the field exactly fills the view at a fixed camera distance; the fragment shader colours each cube from its segment index with a sine gradient, so the snake renders as a rainbow. Back-face culling is on and the projection is rebuilt in the framebuffer-resize callback.
- **SDL2 and SFML renderers** draw scaled rectangles and recompute the cell size from the current window size on resize events, so the whole field stays visible after the window is resized.
- **Build hygiene.** Every target compiles with `-Wall -Wextra -Werror -std=c++20 -fPIC`, header dependencies are generated with `-MMD -MP`, and the top-level Makefile builds the three `.so` files before linking the executable.
- **Input validation.** Field dimensions must be positive integers that fit in an `int`; the `Snake` constructor additionally rejects fields smaller than the starting length (4) and out-of-range start positions.

## How it works

```
./nibbler <w> <h>
   │
   ├─ main.cpp     dlopen("./gui_libs/<name>/<name>_libgui.so")
   │               dlsym("createGuiLibrary") → ALibGUI*
   │
   │   every 80 ms:
   │     Input in = gui->handleInput();      // quit / arrows / digit keys
   │     if digit pressed → switchLibrary()  // destroy, dlclose, dlopen next
   │     snake.move(in);                     // throws SnakeException on collision → reset
   │     gui->render(snake.getSegments(), snake.getFood());
   │
   └─ snake.cpp    Snake: direction, segments, food, RNG
```

| File | Role |
|------|------|
| `main.cpp` | argument validation, `dlopen`/`dlsym` loader, runtime library switch, fixed-tick game loop |
| `snake.hpp/.cpp` | `Snake` class: movement with reversal lockout, wall and self collision, growth on food, food generation |
| `libgui.hpp` | `ALibGUI` abstract renderer, `LibGUISettings`, `LibGuiException`, C factory declarations |
| `common_types.hpp` | `Segment` (grid cell) and `Input` (key state) shared by both sides |
| `gui_libs/sdl/` | SDL2 renderer (`SDL_Renderer`, filled rects, `SDL_PollEvent`) |
| `gui_libs/sfml/` | SFML 3 renderer (`sf::RenderWindow`, `RectangleShape`, `std::optional` event API, view reset on resize) |
| `gui_libs/opengl/` | GLFW + GLEW + GLM renderer, GLSL 3.30 shaders in `shaders/` |

Each `gui_libs/*/` directory has its own Makefile producing `<name>_libgui.so` and defines the same two `extern "C"` functions, which is all the loader needs. The renderer classes are non-copyable and move-only, mirroring `ALibGUI`.

## Controls

| Key | Action |
|-----|--------|
| Arrow keys | change direction (reversing into the body is ignored) |
| `1` / `2` / `3` | switch to the SDL2 / SFML / OpenGL renderer |
| `Esc` or closing the window | quit |

## Building and running

Requirements, as encoded in the Makefiles:

- `clang++` with C++20 support;
- SDL2 with `sdl2-config` on `PATH`;
- SFML 3, GLFW, GLEW and GLM installed under `/opt/homebrew` (the SFML and OpenGL sub-Makefiles hard-code `-I/opt/homebrew/include -L/opt/homebrew/lib`);
- the OpenGL library links with `-framework OpenGL`, so the build as written targets macOS with Homebrew.

```sh
make            # builds gui_libs/{sdl,sfml,opengl}/*.so, then ./nibbler
./nibbler 30 20 # field width and height in cells
make clean      # objects and .d files
make fclean     # also the binary and the .so files
make re
```

Run from the repository root: the library paths (`./gui_libs/.../*.so`) and the shader paths (`./gui_libs/opengl/shaders/`) are resolved relative to the working directory. The window opens at 1024x768 and can be resized.

## Limitations / notes

- The Homebrew include and library paths and the `-framework OpenGL` flag are hard-coded; building on Linux means editing the SFML and OpenGL sub-Makefiles.
- The list of libraries and the `1`–`3` key mapping are fixed in `main.cpp` and in each renderer.
- Fixed 80 ms tick; no score, no speed-up, no pause. Game over is reported on stderr and the board resets immediately.
- No automated tests; one commit.

## Context

Structured after the École 42 *nibbler* subject: a Snake game whose graphics must be provided by several independently compiled dynamic libraries that can be switched at runtime. Written in C++20 with `clang++`.
