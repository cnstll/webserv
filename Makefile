NAME = webserv

SRCS = ./core/core.cpp  \
		./core/Response.cpp \
		./core/Request.cpp \
		./cgi/CgiHandler.cpp \
		./parsing/parseConfig.cpp \
		./parsing/preParsing.cpp \
		./core/Server.cpp \
		./utils/utils.cpp

OBJS = $(SRCS:.cpp=.o)

COMP= g++  -fdiagnostics-color=always -fsanitize=address -g -std=c++98

CFLAGS = -std=c++98 -Wall -Wextra -Werror

GFLAG = #-g3 -fsanitize=address

INCLUDES= -I includes/

all : $(NAME)

$(NAME) : $(OBJS)
	$(COMP) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(COMP) -o $@ -c $^ $(CFLAGS) $(INCLUDES)

debug :
	$(COMP) $(CFLAGS) $(GFLAG) -o $(NAME) $(SRCS) 

debug_lldb :
	$(COMP) $(CFLAGS) -g -o $(NAME) $(SRCS) 

clean :
	rm -f $(OBJS)

fclean:	clean
	rm -f $(NAME)

re: fclean all
