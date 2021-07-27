#include "minishell.h"

static void	term_load2(t_list *cmds, char *line, char *term_name)
{
	if (tcgetattr(0, &g_global.term) == ERROR || \
	tcgetattr(0, &g_global.save) == ERROR)
	{
		if (get_next_line(0, &line) <= 0)
			exit(1);
		cmds = parse_line(line);
		free(line);
		if (!cmds)
			exit(2);
		g_global.in_cmd = 1;
		do_command(cmds);
		lst_destroy(cmds);
		exit((256 + g_global.cmd_ret) % 256);
	}
	g_global.term.c_lflag &= ~(ICANON | ECHO);
	g_global.term.c_cc[VMIN] = 1;
	g_global.term.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &g_global.term) == ERROR)
		exit(1);
	if (tgetent(NULL, term_name) != 1)
		exit(1);
}

void	term_load(void)
{
	t_list	*cmds;
	char	*line;
	char	*term_name;

	if (map_contains_key(g_global.env, "TERM"))
		term_name = map_get(g_global.env, "TERM");
	else
	{
		ft_puterr("Impossible to determine the terminal\n");
		exit(1);
	}
	term_load2(cmds, line, term_name);
}

int	initialize(char **envp)
{
	g_global.cmd_ret = 0;
	load_environment(envp);
	term_load();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
	return (ft_strlen(PROMPT));
}
