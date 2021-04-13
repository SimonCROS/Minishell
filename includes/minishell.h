#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"

/*** Definitions **************************************************************/

# define BUFF_SIZE 50

/*** Types ********************************************************************/

typedef struct s_gnl_entry	t_gnl_entry;

typedef struct s_token		t_token;
typedef enum e_token_type	t_token_type;

typedef struct s_command	t_command;

/*** Tokenizer ****************************************************************/

enum e_token_type
{
	T_WORD,
	T_WHITESPACE,
	T_LAZY_AND,
	T_AND,
	T_SINGLE_QUOTE,
	T_DOUBLE_QUOTE,
	T_PIPE,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_DOLLAR,
	T_NONE
};

struct s_token
{
	char			**buffer;
	t_token_type	token_type;
	int				quoted;
	int				separator;
};

/*** GNL **********************************************************************/

struct s_gnl_entry
{
	void	*next;
	int		fd;
	char	*content;
};

char		**ft_split_first(char *s, char c);
int			get_next_line(int fd, char **line);
int			gnl_init(char ***current, char **tmp_line, ssize_t *result);

/*** Modeles ******************************************************************/

struct s_command
{
	t_list			*args;
	char			*redirect_in;
	char			*redirect_out;
	t_token_type	parent_relation;
};

#endif
