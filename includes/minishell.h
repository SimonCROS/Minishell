#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <term.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <termios.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/errno.h>
# include <sys/ioctl.h>
# include <sys/param.h>

/*** Types ********************************************************************/

typedef struct s_global			t_global;

typedef struct s_gnl_entry		t_gnl_entry;

typedef struct s_redirect		t_redirect;
typedef struct s_command		t_command;

typedef struct s_token			t_token;
typedef enum e_token_type		t_token_type;

typedef struct s_parsing_arg	t_parsing_argument;

t_global						g_global;

/*** Definitions **************************************************************/

# define BUFF_SIZE		50
# define ERROR			-1
# define TOO_MANY_ARGS	1
# define NOT_VALID		1
# define NOT_SET		-4
# define CTRL_D			"\4"
# define PROMPT			"minishell> "
# define CONTINUE		2

/*** Tokenizer ****************************************************************/

enum e_token_type
{
	T_NONE,
	T_WORD,
	T_NUMBER,
	T_WHITESPACE,
	T_SEPARATOR,
	T_SINGLE_QUOTE,
	T_DOUBLE_QUOTE,
	T_PIPE,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_VAR
};

struct s_token
{
	char			**buffer;
	t_token_type	type;
	int				quoted;
	int				separator;
	t_list			*children;
	t_token			*parent;
};

int			is_valid_variable_char(char c, char *str);
t_token		*new_token(t_token *parent, t_token_type tp, t_token *t, int push);

/*** Parsing ******************************************************************/

struct s_parsing_arg
{
	char	**argument;
	t_token	*next;
	int		*params;
};

t_list		*parse_line(char *line);
int			validate(t_list *commands, t_list *tokens, int started, int zero);
void		read_token(t_token *token, char **container);
int			is_redirection(t_list *tokens, t_token *current);

/*** Global *******************************************************************/

struct s_global
{
	char			pwd[MAXPATHLEN];
	char			oldpwd[MAXPATHLEN];
	t_map			*env;
	int				cmd_ret;
	int				in_cmd;
	struct termios	term;
	struct termios	save;
	struct winsize	wnsze;
	char			**line;
	int				pos;
	char			*line_cpy;
	t_dentry		*history;
	int				fd[2];
};

int			fork_pipes(t_iterator *it);
int			launch_built_in(t_iterator *it);

/*** Commands *****************************************************************/

struct s_redirect
{
	int		fd;
	char	*str;
	int		append;
};

struct s_command
{
	char			**buffer;
	t_token_type	type;
	int				quoted;
	int				separator;
	t_list			*children;
	t_token			*parent;
	t_list			*args;
	t_list			*redirect_in;
	t_list			*redirect_out;
	t_token_type	next_relation;
};

char		**ft_split_first(char *s, char c);
int			gnl_init(char ***current, char **tmp_line, ssize_t *result);
int			parse(t_command *command);
int			tokenize(t_token *parent, char **line);
int			token1(t_token	**cur, char c, int escaped, t_token *parent);
int			token2(t_token	**cur, char c, char **line, t_token *parent);
t_token		null_token(void);
void		free_token(t_token *token);

/*** Modeles ******************************************************************/

int			do_execute(char *path, char **argv);
void		do_pwd(char **argv);
void		do_cd(char **argv);
void		do_echo(char **argv);
void		do_export(char **argv);
void		do_unset(char **argv);
void		do_env(char **argv);
void		do_exit(char **argv);
char		*env_compose(char *key, char *value);
void		load_environment(char **envp);
int			built_in(char **argv, int forked);
void		do_command(t_list *cmds);
int			check_export_arg(char *arg);
void		term_load(void);
int			initialize(char **envp);
void		signal_handler(int sig);
void		terminal(int prompt_size);
char		*get_path_from_env(char *path);
int			file_exists(char *filename);
int			redirect_in(t_command *cmd);
int			redirect_out(t_command *cmd);
void		wait_proc(int tmp, int count, pid_t last, int status);

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

/*** GNL **********************************************************************/

struct s_gnl_entry
{
	void	*next;
	int		fd;
	char	*content;
};

char		**ft_gnl_split(char *s, char c);
int			get_next_line(int fd, char **line);
int			gnl_init(char ***current, char **tmp_line, ssize_t *result);

/*** Others *******************************************************************/

void		free_command(t_command *command);
void		free_redirect(t_redirect *redirect);

#endif
