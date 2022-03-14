NAME = webserv

SRCS = ./core/core.c  \

OBJS = $(SRCS:.c=.o)

UNAME_S := $(shell uname -s)
   ifeq ($(UNAME_S),Linux)
        CC=clang
    endif
    ifeq ($(UNAME_S),Darwin)
        CC= clang
    endif

CFLAGS = #-Wall -Wextra -Werror

GFLAG = #-g3 -fsanitize=address

INCLUDES= -I includes/

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS) $(INCLUDES)

debug :
	$(CC) $(CFLAGS) $(GFLAG) -o $(NAME) $(SRCS) 

debug_lldb :
	$(CC) $(CFLAGS) -g -o $(NAME) $(SRCS) 

clean :
	rm -f $(OBJS)

fclean:	clean
	rm -f $(NAME)

re: fclean all
