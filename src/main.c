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
		ft_putendl_fd("minishell: Impossible to determine the terminal\n", 2);
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

void	term_resize(int sig)
{
	if (SIGWINCH == sig)
	{
    	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
		tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
		tputs(tgetstr("cr", NULL), 1, (int (*)(int))ft_putchar);
		tputs(clr_eos, 1, (int (*)(int))ft_putchar);
		ft_putstr(PROMPT);
		ft_putstr(*g_global.line);
	}
}

void	test_adel(int prompt_size)
{
	int				pos;
	int				len;
	t_dlist			*history;
	t_dentry		*walker;
	char			str[2000];
	char			*cpy;
	char			*clear;
	t_list			*cmds;

	history = dlst_new(free);
	clear = "clear";
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
	*str = 0;
	pos = 0;
	tputs(keypad_xmit, 1, (int (*)(int))ft_putchar);
	while (TRUE)
	{
		cpy = NULL;
		walker = NULL;
		ft_putstr(PROMPT);
		tputs(save_cursor, 1, (int (*)(int))ft_putchar);
		g_global.line = str_new();
		tcsetattr(0, TCSANOW, &g_global.term);
		while (1)
		{
			len = read(1, str, 100);
			str[len] = 0;
			signal(SIGWINCH, term_resize);
			if (ft_str_equals(str, key_up))
			{
				if (!cpy)
					cpy = ft_strdup(*g_global.line);
				walker = dlst_walk_right(walker);
				if (!walker)
					walker = history->first;
				if (walker)
				{
					free(*g_global.line);
					*g_global.line = ft_strdup(walker->value);
				}
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				ft_putstr(*g_global.line);
				pos = ft_strlen(*g_global.line);
			}
			else if (ft_str_equals(str, key_down))
			{
				walker = dlst_walk_left(walker);
				if (!walker)
				{
					if (cpy)
					{
						free(*g_global.line);
						*g_global.line = ft_strdup(cpy);
						free(cpy);
						cpy = NULL;
					}
					else
						tputs(bell, 1, (int (*)(int))ft_putchar);
				}
				else
				{
					free(*g_global.line);
					*g_global.line = ft_strdup(walker->value);
				}
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				ft_putstr(*g_global.line);
				pos = ft_strlen(*g_global.line);
			}
			else if (pos && (ft_str_equals(str, "\177") || ft_str_equals(str, key_backspace)))
			{
				if ((pos + prompt_size) % g_global.wnsze.ws_col == 0)
				{
					tputs(cursor_up, 1, (int (*)(int))ft_putchar);
					tputs(tgoto(tgetstr("ch", NULL), 0, g_global.wnsze.ws_col - 1), 1, (int (*)(int))ft_putchar);
					tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				}
				else
					tputs(cursor_left, 1, (int (*)(int))ft_putchar);
				tputs(delete_character, 1, (int (*)(int))ft_putchar);
				g_global.line = str_rmlast(g_global.line);
				if (pos > 0)
					pos--;
			}
			else if (*str > 31 && *str < 127)
			{
				ft_putchar(*str);
				str_cappend(g_global.line, *str);
				if ((pos + prompt_size) % g_global.wnsze.ws_col == g_global.wnsze.ws_col - 1)
				{
					tputs(cursor_down, 1, (int (*)(int))ft_putchar);
					tputs(tgetstr("cr", NULL), 1, (int (*)(int))ft_putchar);
				}
				pos++;
			}
			else if (*str == 12 && *(str + 1) == 0)
			{
				do_execute(clear, &clear);
				ft_putstr(PROMPT);
			}
			if (ft_str_equals(str, "\n") || (ft_str_equals(str, CTRL_D) && !pos))
				break ;
			if (ft_str_equals(str, CTRL_D))
				tputs(bell, 1, (int (*)(int))ft_putchar);
		}
		if (ft_str_equals(str, CTRL_D))
		{
			if (str_is_empty(*g_global.line) && pos == 0)
				do_exit(NULL);
			continue ;
		}
		pos = 0;
		ft_putchar('\n');
		free(cpy);
		if (!str_is_empty(*g_global.line))
			dlst_unshift(history, ft_strdup(*g_global.line));
		tcsetattr(0, TCSANOW, &g_global.save);
		cmds = parse_line(*g_global.line);
		if (!cmds)
			return ;
		do_command(cmds);
		lst_destroy(cmds);
		free(*g_global.line);
		free(g_global.line);
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	int		prompt_size;

	(void)argc;
	(void)argv;
	g_global.fd[0] = dup(0);
	g_global.fd[1] = dup(1);
	g_global.cmd_ret = 0;
	prompt_size = ft_strlen(PROMPT);
	load_environment(envp);
	term_load();
	test_adel(prompt_size);
	return (0);
}
