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
# define TOO_MANY_ARGS	-2
# define NOT_VALID		-3
# define NOT_SET		-4
# define OLDPWD			0
# define PWD			1
# define CTRL_D			"\4"
# define PROMPT			"minishell> "

/*** Tokenizer ****************************************************************/

enum e_token_type
{
	T_NONE,
	T_WORD,
	T_WHITESPACE,
	T_SEPARATOR,
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
	char			oldpwd[MAXPATHLEN];
	t_map			*env;
	int				cmd_ret;
	int				in_cmd;
	struct termios	term;
	struct termios	save;
	int				fd[2];
	struct winsize	wnsze;
	char			**line;
	int				pos;
	char			*line_cpy;
	t_dentry		*history;
};

// to rm
void	printcommand(t_command *command);

/*** Commands *****************************************************************/

struct s_command
{
	t_list			*args;
	t_list			*tokens;
	int				append;
	t_list			*redirect_in;
	t_list			*redirect_out;
	t_token_type	next_relation;
};

char		**ft_split_first(char *s, char c);
int			get_next_line(int fd, char **line);
int			gnl_init(char ***current, char **tmp_line, ssize_t *result);
int			parse(t_command *command);

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
void		do_command(t_list *cmds);
int			do_redirect(t_command *cmd);
int			check_export_arg(char *arg);
void		term_load(void);
int			initialize(char **envp);
void		signal_handler(int sig);
void		terminal(int prompt_size);
int			file_exists(char *filename);

void		execve_err(void);
void		ft_puterr(char *a);
void		ft_puterr2(char *a, char *b);
void		ft_puterr3(char *a, char *b, char *c);
void		ft_puterr4(char *a, char *b, char *c, char *d);

int			key_handler(char *str, int prompt_size, t_dlist *history);
void		do_clear(void);
void		do_key_print(char *str);
void		do_key_backspace(int prompt_size);
void		do_key_down(void);
void		do_key_up(t_dlist *history);

#endif
