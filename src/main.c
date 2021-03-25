#include "minishell.h"

#include <stdio.h>
void	printline(void *str)
{
	printf("\033[32m\"%s\"\033[0m\n", (char *)str);
}

char	*parse_simple_quote(char **line)
{
	char	*str;
	char	*start;

	(*line)++;
	start = *line;
	str = NULL;
	while (**line && **line != '\'')
		(*line)++;
	if (**line)
		str = ft_substr(start, 0, *line - start);
	return (str);
}

char	*parse_argument(char **line)
{
	t_list	*argument;

	argument = lst_new(free);
	if (!argument)
		return (NULL);
	while (**line)
	{
		if (**line == '\'')
			lst_push(argument, parse_simple_quote(line));
		(*line)++;
	}
	return ((char *)lst_reducef(argument, NULL, ft_strjoin, free));
}

int	parse_command(t_list *commands, char **line)
{
	t_list	*command;

	command = lst_new(free);
	if (!command)
		return (FALSE);
	while (**line)
	{
		lst_push(command, parse_argument(line));
	}
	lst_foreach(command, printline);
	// if (in_str)
	// {
	// 	printf("\033[31mMissing quote !\033[0m\n");
	// 	return (-1);
	// }
	lst_push(commands, command);
	return (TRUE);
}

t_list	*parse_line(char *line)
{
	t_list	*commands;

	commands = lst_new((t_con)lst_destroy);
	if (!commands)
		return (NULL);
	while (*line && parse_command(commands, &line))
		;
}

void	parseLine(char *line)
{
	parse_line(line);
	free(line);
}

void	test(void)
{
	int			result;
	char		*buffer;
	char		comp;

	result = 1;
	while (result)
	{
		result = get_next_line(0, &buffer);
		parseLine(buffer);
	}
}

int	main(int argc, char const *argv[])
{
	test();
	return (0);
}
