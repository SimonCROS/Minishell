#include "minishell.h"

void	do_clear(void)
{
	char	*clear[2];

	clear[0] = "clear";
	clear[1] = NULL;
	do_execute(clear[0], clear);
	ft_putstr(PROMPT);
	tputs(save_cursor, 1, (int (*)(int))ft_putchar);
	ft_putstr(*g_global.line);
}

int	key_handler(char *str, int prompt_size, t_dlist *history)
{
	if (ft_str_equals(str, key_up))
		do_key_up(history);
	else if (ft_str_equals(str, key_down))
		do_key_down();
	else if (g_global.pos && (ft_str_equals(str, "\177") || \
	ft_str_equals(str, key_backspace)))
		do_key_backspace(prompt_size);
	else if (*str > 31 && *str < 127)
		do_key_print(str);
	else if (*str == 12 && *(str + 1) == 0)
		do_clear();
	if (ft_str_equals(str, "\n") || (ft_str_equals(str, CTRL_D) && \
	!g_global.pos))
		return (TRUE);
	if (ft_str_equals(str, CTRL_D))
		tputs(bell, 1, (int (*)(int))ft_putchar);
	return (FALSE);
}
