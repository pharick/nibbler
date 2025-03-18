NAME		:= sdl_libgui.so

CXX			:= clang++
CXXFLAGS	:= -Wall -Wextra -Werror -fPIC -std=c++20 -MMD -MP -I../../ $(shell sdl2-config --cflags)
LDFLAGS		:= $(shell sdl2-config --libs)

SRC			:= sdl_libgui.cpp
OBJ			:= $(SRC:.cpp=.o)
DEPS		:= $(OBJ:.o=.d)

all:		$(NAME)

$(NAME):	$(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) -shared

%.o:		%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) -I. $(CXXFLAGS)

clean:
	rm -f $(OBJ) $(OBJ:.o=.d)

fclean:		clean
	rm -f $(NAME)

re:			fclean all

-include $(DEPS)

.PHONY: all clean fclean re
