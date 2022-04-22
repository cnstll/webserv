NAME = webserv

SRCS = ./core/Response.cpp \
		./core/Request.cpp \
		./cgi/CgiHandler.cpp \
		./parsing/parseConfig.cpp \
		./parsing/preParsing.cpp \
		./core/Server.cpp \
		./core/core.cpp  \
		./utils/utils.cpp

OBJS = $(SRCS:.cpp=.o)

COMP= g++ -g -fdiagnostics-color=always #-fsanitize=address

CFLAGS = -std=c++98 -Wall -Wextra -Werror

all : $(NAME)

$(NAME) : $(OBJS)
	$(COMP) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(COMP) -o $@ -c $^ $(CFLAGS)

clean :
	rm -f $(OBJS)

fclean:	clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean re fclean