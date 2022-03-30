NAME = webserv

SRCS = ./core/core_experiment.cpp  \
		./core/Response.cpp \
		./core/Request.cpp \
		./cgi/env.cpp \
		./core/utils.cpp

OBJS = $(SRCS:.cpp=.o)

COMP=g++

CFLAGS =  -g #-std=c++98 #-Wall -Wextra -Werror

GFLAG = #-g3 -fsanitize=address

INCLUDES= -I includes/

all : $(NAME)

$(NAME) : $(OBJS)
	$(COMP) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(COMP) -o $@ -c $^ $(CFLAGS) $(INCLUDES)

clean :
	rm -f $(OBJS)

fclean:	clean
	rm -f $(NAME)

re: fclean all
