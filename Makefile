BIN			= bin
SRC			= src
INC			= includes
LIBFT_DIR	= libft

LIBFT		= $(LIBFT_DIR)/libft.a

SRCS		=	main.c								\
				scros.c								\
				redirection.c						\
				parsing/tokenizer.c					\
				parsing/debuger.c					\
				commands/cd.c						\
				commands/echo.c						\
				commands/env.c						\
				commands/execve.c					\
				commands/exit.c						\
				commands/export.c					\
				commands/pwd.c						\
				commands/unset.c					\
				termcap/key_handler.c				\
				termcap/key_pressed.c				\
				termcap/terminal.c					\
				utilities/environ.c					\
				utilities/init.c					\
				utilities/put_error.c				\
				gnl/get_next_line.c					\
				gnl/get_next_line_utils.c			\
				signal.c

OBJS		= $(addprefix $(BIN)/, $(SRCS:.c=.o))

NAME		= minishell

CC			= gcc
RM			= rm -f

CFLAGS		= #-Wall -Wextra -Werror -fsanitize=address -g3
INCLUDES	= -I$(INC) -I$(LIBFT_DIR)/$(INC)

HEADERS		= includes/minishell.h

all:		$(NAME)

$(BIN)/%.o:	$(SRC)/%.c $(HEADERS)
			@ mkdir -p $(dir $@);
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME):	compile_lib $(OBJS)
			$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME) -ltermcap

compile_lib:
			$(MAKE) -C $(LIBFT_DIR)

re_lib:
			$(MAKE) -C $(LIBFT_DIR) re

clean:
			$(MAKE) -C $(LIBFT_DIR) clean
			$(RM) $(OBJS)
			@ find . -type d -empty -delete

fclean:
			$(MAKE) -C $(LIBFT_DIR) fclean
			$(RM) $(OBJS) $(NAME)
			@ find . -type d -empty -delete

re:			fclean all

.PHONY:		all clean fclean re compile_lib re_lib
