#include "minishell.h"

void	new_prompt(void)
{
	g_global.line_cpy = NULL;
	g_global.history = NULL;
	ft_putstr(PROMPT);
	tputs(save_cursor, 1, (int (*)(int))ft_putchar);
	g_global.line = str_new();
	g_global.pos = 0;
	g_global.in_cmd = 0;
	tcsetattr(0, TCSANOW, &g_global.term);
}

void	launch_command(t_dlist *history)
{
	t_list		*cmds;

	ft_putchar('\n');
	free(g_global.line_cpy);
	if (!str_is_empty(*g_global.line))
	{
		dlst_unshift(history, ft_strdup(*g_global.line));
		tcsetattr(0, TCSANOW, &g_global.save);
		cmds = parse_line(*g_global.line);
		if (!cmds)
			g_global.cmd_ret = 258;
		else
		{
			g_global.in_cmd = 1;
			do_command(cmds);
			lst_destroy(cmds);
		}
	}
	free(*g_global.line);
	free(g_global.line);
}

void	terminal(int prompt_size)
{
	int				len;
	t_dlist			*history;
	char			str[2000];

	*str = 0;
	history = dlst_new(free);
	tputs(keypad_xmit, 1, (int (*)(int))ft_putchar);
	while (TRUE)
	{
		new_prompt();
		while (1)
		{
			len = read(1, str, 100);
			str[len] = 0;
			if (key_handler(str, prompt_size, history))
				break ;
		}
		if (ft_str_equals(str, CTRL_D))
		{
			if (str_is_empty(*g_global.line) && g_global.pos == 0)
				do_exit(NULL);
			continue ;
		}
		launch_command(history);
	}
}
