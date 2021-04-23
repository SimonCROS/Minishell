#include "minishell.h"

void	cmd_distributor(char **argv)
{
	if (*argv == NULL)
		return ;
	else if (!ft_strcmp(*argv, "echo"))
		do_echo(argv + 1);
	else if (!ft_strcmp(*argv, "cd"))
		do_cd(argv + 1);
	else if (!ft_strcmp(*argv, "pwd"))
		do_pwd(argv + 1);
	else if (!ft_strcmp(*argv, "export"))
		do_export(argv + 1);
	else if (!ft_strcmp(*argv, "unset"))
		do_unset(argv + 1);
	else if (!ft_strcmp(*argv, "env"))
		do_env(argv + 1);
	else if (!ft_strcmp(*argv, "exit"))
		do_exit(argv + 1);
	else
		do_execute(*argv, argv);
}

void	term_load(void)
{
	char			*term_name;
	
	if (map_contains_key(g_global.env, "TERM"))
		term_name = map_get(g_global.env, "TERM");
	else
	{
		ft_putendl_fd("Impossible to determine the terminal\n", 2);
		exit(1);
	}
	if (tcgetattr(0, &g_global.term) == ERROR || tcgetattr(0, &g_global.save) == ERROR)
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

char	*tgetval(char *id)
{
	char	*res;

	res = tgetstr(id, NULL);
	res[2] = '0';
	return (res);
}

void	test_adel(void)
{
	int			pos;
	int			len;
	t_dlist		*history;
	t_dentry	*walker;
	char		str[2000];
	char		**line;
	char		*cpy;
	t_command	*cmd;

	history = dlst_new(free);
	*str = 0;
	pos = 0;
	tputs(keypad_xmit, 1, (int (*)(int))ft_putchar);
	while (TRUE)
	{
		cpy = NULL;
		walker = NULL;
		ft_putstr("\033[32mMinishell> \033[0m");
		tputs(save_cursor, 1, (int (*)(int))ft_putchar);
		line = str_new();
		tcsetattr(0, TCSANOW, &g_global.term);
		while (1)
		{
			len = read(1, str, 100);
			str[len] = 0;
			if (ft_str_equals(str, key_up))
			{
				if (!cpy)
					cpy = ft_strdup(*line);
				walker = dlst_walk_right(walker);
				if (!walker)
					walker = history->first;
				if (walker)
				{
					free(*line);
					*line = ft_strdup(walker->value);
				}
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				ft_putstr(*line);
				pos = ft_strlen(*line);
			}
			else if (ft_str_equals(str, key_down))
			{
				walker = dlst_walk_left(walker);
				if (!walker)
				{
					if (cpy)
					{
						free(*line);
						*line = ft_strdup(cpy);
						free(cpy);
						cpy = NULL;
					}
					else
						tputs(bell, 1, (int (*)(int))ft_putchar);
				}
				else
				{
					free(*line);
					*line = ft_strdup(walker->value);
				}
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				ft_putstr(*line);
				pos = ft_strlen(*line);
			}
			else if (pos && (ft_str_equals(str, (char[2]){ 127, 0 }) || ft_str_equals(str, key_backspace)))
			{
				tputs(cursor_left, 1, (int (*)(int))ft_putchar);
				tputs(delete_character, 1, (int (*)(int))ft_putchar);
				line = str_rmlast(line);
				if (pos > 0)
					pos--;
			}
			else if (*str > 31 && *str < 127)
			{
				ft_putchar(*str);
				str_cappend(line, *str);
				pos++;
			}
			if (ft_str_equals(str, "\n") || (ft_str_equals(str, CTRL_D) && !pos))
				break ;
			if (ft_str_equals(str, CTRL_D))
				tputs(bell, 1, (int (*)(int))ft_putchar);
		}
		if (ft_str_equals(str, CTRL_D))
		{
			if (str_is_empty(*line) && pos == 0)
				do_exit(NULL);
			continue ;
		}
		pos = 0;
		ft_putchar('\n');
		free(cpy);
		if (!str_is_empty(*line))
			dlst_unshift(history, ft_strdup(*line));
		tcsetattr(0, TCSANOW, &g_global.save);
		cmd = lst_first(parse_line(*line));
		if (cmd)
			do_command(cmd);
		lst_clear(cmd->args);
		free(*line);
		free(line);
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
	load_environment(envp);
	term_load();
	test_adel();
	return (0);
}
