#include "minishell.h"

void	redirect_out(t_command *cmd)
{
	int			out_fd;
	int			pid;
	t_iterator	it;

	if (lst_is_empty(cmd->redirect_out))
		return ;
	it = iterator_new(cmd->redirect_out);
	while (iterator_has_next(&it))
		close(open(((t_entry *)iterator_next(&it))->value, O_WRONLY | O_CREAT));
	// pid = fork();
	// wait(NULL);
	// if (pid == 0)
	// {
		out_fd = open(lst_last(cmd->redirect_out), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		if (dup2(out_fd, 1) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		if (close(out_fd) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
	// 	cmd_distributor((char **)as_array(cmd->args));
	// 	exit(0);
	// }
}

void	append(t_command *cmd)
{
	int		out_fd;
	int		pid;

	if (lst_is_empty(cmd->redirect_out))
		return ;
	// pid = fork();
	// wait(NULL);
	// if (pid == 0)
	// {
		out_fd = open(lst_last(cmd->redirect_out), O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (out_fd == -1)
		{
			ft_puterr2(">>: ", strerror(errno));
			return ;
		}
		if (dup2(out_fd, 1) == -1)
		{
			ft_puterr2(">>: ", strerror(errno));
			return ;
		}
		if (close(out_fd) == -1)
		{
			ft_puterr2(">>: ", strerror(errno));
			return ;
		}
	// 	cmd_distributor((char **)as_array(cmd->args));
	// 	exit(0);
	// }
}

void	redirect_in(t_command *cmd)
{
	int			in_fd;
	int			pid;

	if (lst_is_empty(cmd->redirect_in))
		return ;
	// pid = fork();
	// wait(NULL);
	// if (pid == 0)
	// {
		in_fd = open(lst_first(cmd->redirect_in), O_RDONLY);
		if (in_fd == -1)
		{
			ft_puterr2("<: ", strerror(errno));
			return ;
		}
		if (dup2(in_fd, 0) == -1)
		{
			ft_puterr2("<: ", strerror(errno));
			return ;
		}
		if (close(in_fd) == -1)
		{
			ft_puterr2("<: ", strerror(errno));
			return ;
		}
	// 	cmd_distributor((char **)as_array(cmd->args));
	// 	exit(0);
	// }
}

void	piper(t_command *cmd)
{
	int	fd[2];
	int	pid;

	if (pipe(fd) == -1)
	{
		ft_puterr2("pipe: ", strerror(errno));
		return ;
	}
	pid = fork();
	if (pid != 0)
	{
		wait(NULL);
		if (dup2(fd[0], 0) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		close(fd[1]);
		cmd_distributor((char **)as_array(cmd->args));
	}
	else
	{
		if (dup2(fd[1], 1) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		close(fd[0]);
		cmd_distributor((char **)as_array(cmd->args));
		exit(0);
	}
}

void	do_command(t_command *cmd)
{
	// int			pid;

	// pid = fork();
	// wait(NULL);
	// if (pid == 0)
	// {
	// 	redirect_in(cmd);
	// 	redirect_out(cmd);
	// 	cmd_distributor((char **)as_array(cmd->args));
	// 	exit(0);
	// }
	cmd_distributor((char **)as_array(cmd->args));
}
