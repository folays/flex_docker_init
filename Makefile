NAME	= flex-docker-init
CC	= gcc
RM	= rm -f

SRC	= flex-docker-init.c
OBJ	= $(SRC:.c=.o)

all	: $(NAME)

re	: fclean all

$(NAME)	: $(OBJ)
	$(CC) -o $(NAME) -static $(OBJ)

clean	:
	-$(RM) $(OBJ) *~

fclean	: clean
	-$(RM) $(NAME)
