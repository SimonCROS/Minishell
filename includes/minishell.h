#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"

/*** Definitions **************************************************************/

# define BUFF_SIZE 50

/*** Types ********************************************************************/

typedef struct s_gnl_entry	t_gnl_entry;

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

#endif
