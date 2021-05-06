#include "minishell.h"

void	ft_puterr(char *a)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putendl_fd(a, 2);
}

void	ft_puterr2(char *a, char *b)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(a, 2);
	ft_putendl_fd(b, 2);
}

void	ft_puterr3(char *a, char *b, char *c)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(a, 2);
	ft_putstr_fd(b, 2);
	ft_putendl_fd(c, 2);
}

void	ft_puterr4(char *a, char *b, char *c, char *d)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(a, 2);
	ft_putstr_fd(b, 2);
	ft_putstr_fd(c, 2);
	ft_putendl_fd(d, 2);
}

void	execve_err(void)
{
	ft_puterr(strerror(errno));
	g_global.cmd_ret = errno;
	exit(errno);
}
