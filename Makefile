NAME			:= ircserv

CXX				:= c++

CXXFLAGS		:= -Wall -Wextra -Werror -std=c++98

SRCS_F			:= sources

SRCS_CMDS		:= Commands cap join kick mode nick part ping privmsg quit topic unknown who whois invite
SRCS_MAIN		:= main Server Helper

SRCS			:= $(addsuffix .cpp, $(addprefix /cmds/, $(SRCS_CMDS))) \
				   $(addsuffix .cpp, $(addprefix /main/, $(SRCS_MAIN))) \

OBJS			:= $(addprefix $(SRCS_F), $(SRCS:.cpp=.o))


$(NAME):	$(OBJS)
			@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
			@make clean

all:		$(NAME)

clean:
			@rm -rf $(OBJS)

fclean:		clean
			@rm -rf $(NAME)

re: 		fclean all

test:		re
			./$(NAME) 3000 1234