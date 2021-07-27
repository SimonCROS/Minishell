#include "minishell.h"

void	signal_resize(void)
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
}

void	signal_interupt(void)
{
	ft_putchar('\n');
	if (!g_global.in_cmd)
	{
		ft_putstr(PROMPT);
		g_global.cmd_ret = 130;
	}
	else
		g_global.cmd_ret = 1;
	free(*g_global.line);
	free(g_global.line);
	g_global.line_cpy = NULL;
	g_global.history = NULL;
	tputs(save_cursor, 1, (int (*)(int))ft_putchar);
	g_global.line = str_new();
	g_global.pos = 0;
	g_global.in_cmd = 0;
	tcsetattr(0, TCSANOW, &g_global.term);
}

void	signal_quit(void)
{
	if (g_global.in_cmd)
	{
		ft_putendl("Quit: 3");
		g_global.cmd_ret = 131;
	}
}

void	signal_handler(int sig)
{
	if (sig == SIGWINCH)
		signal_resize();
	else if (sig == SIGQUIT)
		signal_quit();
	else if (sig == SIGINT)
		signal_interupt();
}
