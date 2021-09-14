#include "minishell.h"

static void	signal_resize(void)
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
}

void	signal_handler(int sig)
{
	if (sig == SIGWINCH)
		signal_resize();
}
