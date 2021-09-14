BIN			= bin
SRC			= src
INC			= includes
LIBFT_DIR	= libft

LIBFT		= $(LIBFT_DIR)/libft.a

SRCS		=	main.c								\
				redirection.c						\
				piper.c								\
				parsing/parser.c					\
				parsing/checker.c					\
				parsing/tokenizer.c					\
				parsing/line_parser.c				\
				parsing/tokenizer_utils.c			\
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
				utilities/free_elements.c			\
				utilities/ft_atol.c					\
				gnl/get_next_line.c					\
				gnl/get_next_line_utils.c			\
				signal.c

OBJS		= $(addprefix $(BIN)/, $(SRCS:.c=.o))

NAME		= minishell

CC			= clang
RM			= rm -f

CFLAGS		= -Wall -Wextra -Werror
INCLUDES	= -I$(INC) -I$(LIBFT_DIR)/$(INC)

HEADERS		= $(INC)/minishell.h $(LIBFT_DIR)/$(INC)/libft.h

all:		compile_lib $(NAME)

$(BIN)/%.o:	$(SRC)/%.c $(HEADERS)
			@ mkdir -p $(dir $@);
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME):	$(OBJS) libft/libft.a
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
