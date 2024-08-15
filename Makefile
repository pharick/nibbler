CXX									:= clang++
CXXFLAGS							:= -Wall -Wextra -Werror -fPIC -std=c++11
LDFLAGS								:= -ldl

SDL_LIB_NAME						:= sdl_libgui.so
MAIN_NAME							:= nibbler

SDL_LIB_CXXFLAGS					:= $(shell sdl2-config --cflags)
SDL_LIB_LDFLAGS						:= $(shell sdl2-config --libs)
SDL_LIB_DIR							:= ./gui_libs/sdl
SDL_LIB_SRC							:= sdl_libgui.cpp
SDL_LIB_OBJ							:= $(SDL_LIB_SRC:.cpp=.o)

MAIN_SRC							:= main.cpp snake.cpp
MAIN_OBJ							:= $(MAIN_SRC:.cpp=.o)

all:								$(SDL_LIB_NAME) $(MAIN_NAME)

$(MAIN_NAME):						$(MAIN_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SDL_LIB_NAME):					$(addprefix $(SDL_LIB_DIR)/, $(SDL_LIB_OBJ))
	$(CXX) $(SDL_LIB_LDFLAGS) -shared -o $@ $^

$(addprefix $(SDL_LIB_DIR)/, %.o):	$(SDL_LIB_DIR)/%.cpp
	echo "lib"
	$(CXX) $(CXXFLAGS) $(SDL_LIB_CXXFLAGS) -c $< -o $@ -I.

%.o: 								%.cpp
	echo "main"
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I.

clean:
	rm -f $(addprefix $(SDL_LIB_DIR)/, $(SDL_LIB_OBJ)) $(MAIN_OBJ)

fclean:								clean
	rm -f $(SDL_LIB_NAME) $(MAIN_NAME)

re:									clean all

.PHONY: all clean fclean re
