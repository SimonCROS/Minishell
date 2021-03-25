#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdio.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/errno.h>
# include <sys/param.h>

/*** Definitions **************************************************************/

# define BUFF_SIZE	50
# define ERROR		-1

/*** Types ********************************************************************/

typedef struct s_gnl_entry	t_gnl_entry;
typedef struct s_global		t_global;

/*** GNL **********************************************************************/

struct s_gnl_entry
{
	void	*next;
	int		fd;
	char	*content;
};

struct s_global
{
	char	pwd[MAXPATHLEN];
	char	**envp;
};

/*** Global variables *********************************************************/

t_global	global;

char		**ft_split_first(char *s, char c);
int			get_next_line(int fd, char **line);
int			gnl_init(char ***current, char **tmp_line, ssize_t *result);

void		execute(char *path, char **argv);
void		pwd(char **argv);
void		cd(char **argv);

#endif
