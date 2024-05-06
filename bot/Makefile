NAME			:= bitchbot

CXX				:= c++

CXXFLAGS		:= -Wall -Wextra -Werror -std=c++98

SRCS_MAIN		:= bot main

SRCS			:= $(addsuffix .cpp, $(SRCS_MAIN))

OBJS			:= $(SRCS:.cpp=.o)


$(NAME):	$(OBJS)
			@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
			@make clean

all:		$(NAME)

clean:
			@rm -rf $(OBJS)

fclean:		clean
			@rm -rf $(NAME)

re: 		fclean all
