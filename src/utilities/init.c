#include "minishell.h"

void	term_load(void)
{
	char	*term_name;

	if (map_contains_key(g_global.env, "TERM"))
		term_name = map_get(g_global.env, "TERM");
	else
	{
		ft_puterr("Impossible to determine the terminal\n");
		exit(1);
	}
	if (tcgetattr(0, &g_global.term) == ERROR || \
	tcgetattr(0, &g_global.save) == ERROR)
	{
		ft_putendl_fd(strerror(errno), 2);
		exit(1);
	}
	g_global.term.c_lflag &= ~(ICANON | ECHO);
	g_global.term.c_cc[VMIN] = 1;
	g_global.term.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &g_global.term) == ERROR)
	{
		ft_putendl_fd(strerror(errno), 2);
		exit(1);
	}
	if (tgetent(NULL, term_name) != 1)
		exit(1);
}

int	initialize(char **envp)
{
	g_global.fd[0] = dup(0);
	g_global.fd[1] = dup(1);
	g_global.cmd_ret = 0;
	load_environment(envp);
	term_load();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
	return (ft_strlen(PROMPT));
}
