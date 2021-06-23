#include "minishell.h"

int	redirect_out(t_command *cmd)
{
	int			out_fd;
	t_entry		*walk;

	if (lst_is_empty(cmd->redirect_out))
		return (TRUE);
	walk = cmd->redirect_out->first;
	while (walk)
	{
		if (cmd->append)
			out_fd = open(((t_redirect *)walk->value)->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			out_fd = open(((t_redirect *)walk->value)->str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1)
		{
			ft_puterr3(((t_redirect *)walk->value)->str, ": ", strerror(errno));
			g_global.cmd_ret = errno;
			return (FALSE);
		}
		if (walk->next)
			close(out_fd);
		else
			dup2(out_fd, ((t_redirect *)walk->value)->fd);
		walk = walk->next;
	}
	close(out_fd);
	return (TRUE);
}

int	redirect_in(t_command *cmd)
{
	int			in_fd;
	t_entry		*walk;

	if (lst_is_empty(cmd->redirect_in))
		return (TRUE);
	walk = cmd->redirect_in->first;
	while (walk)
	{
		in_fd = open(((t_redirect *)walk->value)->str, O_RDWR);
		if (in_fd == -1)
		{
			ft_puterr3(((t_redirect *)walk->value)->str, ": ", strerror(errno));
			g_global.cmd_ret = errno;
			return (FALSE);
		}
		if (walk->next)
			close(in_fd);
		walk = walk->next;
	}
	dup2(in_fd, 0);
	close(in_fd);
	return (TRUE);
}

void	piper(t_command *cmd, t_iterator *it)
{
	int	fd[2];
	int	pid;

	pipe(fd);
	pid = fork();
	if (pid != 0)
	{
		wait(NULL);
		dup2(fd[0], 0);
		close(fd[1]);
		cmd = iterator_next(it);
		parse(cmd);
		if (cmd->next_relation == T_PIPE)
			piper(cmd, it);
		else
			do_redirect(cmd);
	}
	else
	{
		dup2(fd[1], 1);
		close(fd[0]);
		do_redirect(cmd);
		exit(0);
	}
}

int	do_redirect(t_command *cmd)
{
	if (redirect_in(cmd) && redirect_out(cmd))
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
		parse(cmd);
		if (cmd->next_relation == T_PIPE)
			piper(cmd, &it);
		else
			do_redirect(cmd);
	}
}
