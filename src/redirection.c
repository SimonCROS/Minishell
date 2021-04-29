#include "minishell.h"

int	redirect_out(t_command *cmd)
{
	int			out_fd;
	t_entry		*walk;

	if (lst_is_empty(cmd->redirect_out))
		return (FALSE);
	walk = cmd->redirect_out->first;
	while (walk)
	{
		if (walk->next)
			close(open(walk->value, O_WRONLY | O_CREAT | O_TRUNC, 0644));
		walk = walk->next;
	}
	if (cmd->append)
		out_fd = open(lst_last(cmd->redirect_out), O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		out_fd = open(lst_last(cmd->redirect_out), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (out_fd == -1)
	{
		ft_puterr2(">: ", strerror(errno));
		return (FALSE);
	}
	if (dup2(out_fd, 1) == -1)
	{
		ft_puterr2(">: ", strerror(errno));
		return (FALSE);
	}
	if (close(out_fd) == -1)
	{
		ft_puterr2(">: ", strerror(errno));
		return (FALSE);
	}
	return (TRUE);
}

int	redirect_in(t_command *cmd)
{
	int			in_fd;

	if (lst_is_empty(cmd->redirect_in))
		return (FALSE);
	in_fd = open(lst_last(cmd->redirect_in), O_RDONLY);
	if (in_fd == -1)
	{
		ft_puterr2("<: ", strerror(errno));
		return (FALSE);
	}
	if (dup2(in_fd, 0) == -1)
	{
		ft_puterr2("<: ", strerror(errno));
		return (FALSE);
	}
	if (close(in_fd) == -1)
	{
		ft_puterr2("<: ", strerror(errno));
		return (FALSE);
	}
	return (TRUE);
}

void	piper(t_command *cmd, t_iterator *it)
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
		cmd = iterator_next(it);
		if (cmd->next_relation == T_PIPE)
			piper(cmd, it);
		else
			do_redirect(cmd);
	}
	else
	{
		if (dup2(fd[1], 1) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		close(fd[0]);
		do_redirect(cmd);
		exit(0);
	}
}

int		do_redirect(t_command *cmd)
{
	if (redirect_in(cmd) || redirect_out(cmd))
		return (FALSE);
	cmd_distributor((char **)as_array(cmd->args));
	dup2(g_global.fd[0], 0);
	dup2(g_global.fd[1], 1);
	if (g_global.cmd_ret)
		return (FALSE);
	return (TRUE);
}

void	do_command(t_list *cmds)
{
	t_iterator	it;
	t_command	*cmd;

	it = iterator_new(cmds);
	while (iterator_has_next(&it))
	{
		cmd = iterator_next(&it);
		if (cmd->next_relation == T_PIPE)
			piper(cmd, &it);
		else
			do_redirect(cmd);
	}
}
