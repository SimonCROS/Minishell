#include "minishell.h"

int	redirect_out(t_command *cmd)
{
	int			out_fd;
	t_entry		*walk;
	t_redirect	*redirect;

	if (lst_is_empty(cmd->redirect_out))
		return (TRUE);
	walk = cmd->redirect_out->first;
	while (walk)
	{
		redirect = ((t_redirect *)walk->value);
		if (redirect->append)
			out_fd = open(redirect->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			out_fd = open(redirect->str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1)
		{
			ft_puterr3(redirect->str, ": ", strerror(errno));
			g_global.cmd_ret = errno;
			return (FALSE);
		}
		if (walk->next)
			close(out_fd);
		else
			dup2(out_fd, redirect->fd);
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

#include <stdio.h>
#include <sys/wait.h>

pid_t	spawn_proc(int in, int out, t_command *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (in != 0)
		{
			dup2(in, 0);
			close(in);
		}
		if (out != 1)
		{
			dup2(out, 1);
			close(out);
		}
		parse(cmd);
		if (redirect_in(cmd) && redirect_out(cmd))
			cmd_distributor((char **)as_array(cmd->args));
	}
	return (pid);
}

int	prepare_command(t_command *command, int in)
{
	int		fd[2];
	pid_t	pid;

	pipe(fd);
	pid = spawn_proc(in, fd[1], command);
	if (command->next_relation != T_PIPE)
		waitpid(pid, NULL, 0);
	close(fd[1]);
	in = fd[0];
}

// wait(&status);
// g_global.cmd_ret = WEXITSTATUS(status);

void	piper(t_command *command, t_iterator *it)
{
	int	in;

	in = prepare_command(command, 0);
	while (command->next_relation == T_PIPE)
	{
		command = iterator_next(it);
		in = prepare_command(command, in);
	}
}

void	do_command(t_list *cmds)
{
	t_iterator	it;
	t_command	*cmd;

	it = iterator_new(cmds);
	while (iterator_has_next(&it))
	{
		cmd = iterator_next(&it);
		if (!parse(cmd))
		{
			g_global.cmd_ret = 2;
			continue ;
		}
		piper(cmd, &it);
	}
}
