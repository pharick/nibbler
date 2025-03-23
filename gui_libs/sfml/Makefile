NAME		:= sfml_libgui.so

CXX			:= clang++
CXXFLAGS	:= -Wall -Wextra -Werror -fPIC -std=c++20 -MMD -MP -I/opt/homebrew/include -I../../ -I.
LDFLAGS		:= -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system

DIR			:= ./gui_libs/sfml
SRC			:= sfml_libgui.cpp
OBJ			:= $(SRC:.cpp=.o)
DEPS		:= $(OBJ:.o=.d)

all:		$(NAME)

$(NAME):	$(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) -shared

%.o:		%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OBJ:.o=.d)

fclean:		clean
	rm -f $(NAME)

re:			fclean all

-include $(DEPS)

.PHONY: all clean fclean re
