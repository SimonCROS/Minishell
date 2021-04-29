#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <term.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <termios.h>
# include <sys/stat.h>
# include <sys/errno.h>
# include <sys/ioctl.h>
# include <sys/param.h>

/*** Types ********************************************************************/

typedef struct s_global		t_global;

typedef struct s_gnl_entry	t_gnl_entry;

typedef struct s_command	t_command;

typedef struct s_token		t_token;
typedef enum e_token_type	t_token_type;

t_global					g_global;

/*** Definitions **************************************************************/

# define BUFF_SIZE		50
# define ERROR			-1
# define CTRL_D			"\4"
# define TOO_MANY_ARGS	-2
# define NOT_VALID		-3
# define NOT_SET		-4

/*** Tokenizer ****************************************************************/

enum e_token_type
{
	T_NONE,
	T_WORD,
	T_WHITESPACE,
	T_LAZY_AND,
	T_AND,
	T_SINGLE_QUOTE,
	T_DOUBLE_QUOTE,
	T_PIPE,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_DOLLAR
};

struct s_token
{
	char			**buffer;
	t_token_type	token_type;
	int				quoted;
	int				separator;
};

/*** Parsing ******************************************************************/

t_list		*parse_line(char *line);

/*** GNL **********************************************************************/

struct s_gnl_entry
{
	void	*next;
	int		fd;
	char	*content;
};

struct s_global
{
	char			pwd[MAXPATHLEN];
	t_map			*env;
	int				cmd_ret;
	struct termios	term;
	struct termios	save;
	int				fd[2];
	struct winsize	wnsze;
	char			**line;
};

/*** Commands *****************************************************************/

struct s_command
{
	t_list			*args;
	int				append;
	t_list			*redirect_in;
	t_list			*redirect_out;
	t_token_type	parent_relation;
};

char		**ft_split_first(char *s, char c);
int			get_next_line(int fd, char **line);
int			gnl_init(char ***current, char **tmp_line, ssize_t *result);

/*** Modeles ******************************************************************/

void		do_execute(char *path, char **argv);
void		do_pwd(char **argv);
void		do_cd(char **argv);
void		do_echo(char **argv);
void		do_export(char **argv);
void		do_unset(char **argv);
void		do_env(char **argv);
void		do_exit(char **argv);
char		*env_compose(char *key, char *value);
void		load_environment(char **envp);
void		cmd_distributor(char **argv);
int			do_command(t_command *cmd);

void		ft_puterr2(char *a, char *b);
void		ft_puterr3(char *a, char *b, char *c);
void		ft_puterr4(char *a, char *b, char *c, char *d);

#endif
