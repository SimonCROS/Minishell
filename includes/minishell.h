#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"

/*** Definitions **************************************************************/

# define BUFF_SIZE 50

/*** Types ********************************************************************/

typedef struct s_gnl_entry	t_gnl_entry;

typedef enum e_command_type	t_command_type;
typedef struct s_command	t_command;

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

enum e_command_type
{
	END,
	COMMAND,
	TO_FOUND,
	PIPE,
	REDIRECT_IN,
	REDIRECT_OUT,
};

struct s_command
{
	t_list			args;
	char			*redirect_in;
	char			*redirect_out;
	t_command		parent;
	t_command_type	next_command_type;
};

#endif
