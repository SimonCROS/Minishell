BIN			= bin
SRC			= src
INC			= includes
LIBFT_DIR	= libft

LIBFT		= $(LIBFT_DIR)/libft.a

SRCS		=	main.c								\
				gnl/get_next_line.c					\
				gnl/get_next_line_utils.c			\
				exec_command.c						\
				commands.c							\
				environ.c							\
				redirection.c

OBJS		= $(addprefix $(BIN)/, $(SRCS:.c=.o))

NAME		= minishell

CC			= gcc
RM			= rm -f

CFLAGS		= -Wall -Wextra #-fsanitize=address # -Werror -g
INCLUDES	= -I$(INC) -I$(LIBFT_DIR)/$(INC)

HEADERS		= includes/minishell.h

all:		$(NAME)

$(BIN)/%.o:	$(SRC)/%.c $(HEADERS)
			@ mkdir -p $(dir $@);
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME):	compile_lib $(OBJS)
			@ $(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME) -ltermcap

compile_lib:
			@ $(MAKE) -C $(LIBFT_DIR)

re_lib:
			@ $(MAKE) -C $(LIBFT_DIR) re

clean:
			@ $(MAKE) -C $(LIBFT_DIR) clean
			@ echo "Cleaning Minishell binaries\n"
			@ $(RM) $(OBJS)
			@ find . -type d -empty -delete

fclean:
			@ $(MAKE) -C $(LIBFT_DIR) fclean
			@ echo "Cleaning Minishell\n"
			@ $(RM) $(OBJS) $(NAME)
			@ find . -type d -empty -delete

re:			fclean all

.PHONY:		all clean fclean re compile_lib re_lib
