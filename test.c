#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

struct command
{
	const char	**argv;
};

int	spawn_proc(int in, int out, char **cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (in != 0)
		{
			dup2 (in, 0);
			close (in);
		}
		if (out != 1)
		{
			dup2 (out, 1);
			close (out);
		}
		execvp(cmd[0], cmd);
		exit(127);
	}
	return (pid);
}

int	fork_pipes(int n, char **cmd)
{
	int		i;
	pid_t	pid;
	int		in;
	int		fd[2];

	in = 0;
	for (i = 0; i < n - 1; ++i)
	{
		pipe(fd);
		spawn_proc(in, fd[1], cmd);
		close(fd[1]);
		in = fd[0];
	}
	if (in != 0)
		dup2 (in, 0);
	return (execvp(cmd[0], cmd));
}

int	main(void)
{
	int			status;

	const char		*cat[] = {"existepas", 0};
	const char		*ls[] = {"ls", 0};

	if (!fork())
	{
		fork_pipes(300, cat);
		exit(127);
	}
	else
	{
		wait(&status);
		printf("%d     \n", WEXITSTATUS(status));
	}
	if (!fork())
	{
		fork_pipes(180, ls);
		exit(127);
	}
	else
	{
		wait(&status);
		printf("%d     \n", WEXITSTATUS(status));
	}
}
