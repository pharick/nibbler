NAME		:= nibbler

CXX			:= clang++
CXXFLAGS	:= -Wall -Wextra -Werror -fPIC -std=c++20 -MMD -MP -I.
LDFLAGS		:= -ldl

SRC			:= main.cpp snake.cpp
OBJ			:= $(SRC:.cpp=.o)
DEPS		:= $(OBJ:.o=.d)

GUI_LIB_DIRS	:= ./gui_libs/sdl ./gui_libs/sfml

all:		$(NAME)

$(NAME):	$(OBJ)
	for dir in $(GUI_LIB_DIRS); do \
		$(MAKE) -C $$dir; \
	done

	$(CXX) -o $@ $^ $(LDFLAGS)

%.o:		%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	for dir in $(GUI_LIB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done

	rm -f $(OBJ) $(OBJ:.o=.d)

fclean:		clean
	for dir in $(GUI_LIB_DIRS); do \
		$(MAKE) -C $$dir fclean; \
	done

	rm -f $(NAME)

re:			clean all

-include $(DEPS)

.PHONY: all clean fclean re
