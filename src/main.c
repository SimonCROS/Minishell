#include "minishell.h"
#include <stdio.h>

void	printline(t_token *str)
{
	printf("\033[32m\"%s\"\033[0m\n", str->content->string);
}

// char	*parse_simple_quote(char **line)
// {
// 	char	*str;
// 	char	*start;

// 	(*line)++;
// 	start = *line;
// 	str = NULL;
// 	while (**line && **line != '\'')
// 		(*line)++;
// 	if (**line)
// 		str = ft_substr(start, 0, *line - start);
// 	return (str);
// }

// char	*parse_double_quote(char **line)
// {
// 	char	*str;
// 	char	*start;

// 	(*line)++;
// 	start = *line;
// 	str = NULL;
// 	while (**line && **line != '\"')
// 	{
// 		if (**line == '\\' && (*(*line + 1) == '$' || *(*line + 1) == '`'
// 				|| *(*line + 1) == '\\' || *(*line + 1) == '\"'))
// 			(*line)++;
// 		else if ((**line == '$' && (ft_isalnum(*(*line + 1))
// 					|| *(*line + 1) == '_')) || **line == '`')
// 			printf("Special here !\n");
// 		(*line)++;
// 	}
// 	if (**line)
// 		str = ft_substr(start, 0, *line - start);
// 	return (str);
// }

// char	*parse_no_quote(char **line)
// {
// 	char	*str;
// 	char	*start;

// 	start = *line;
// 	str = NULL;
// 	while (**line && **line != '\"' && **line != '\''
// 		&& **line != ' ')
// 	{
// 		if (**line == '\\' && (*(*line + 1) == '$' || *(*line + 1) == '`'
// 				|| *(*line + 1) == '\\' || *(*line + 1) == '\"'
// 				|| *(*line + 1) == ' '))
// 			(*line)++;
// 		else if ((**line == '$' && (ft_isalnum(*(*line + 1))
// 					|| *(*line + 1) == '_')) || **line == '`')
// 			printf("Special here !\n");
// 		(*line)++;
// 	}
// 	str = ft_substr(start, 0, *line - start);
// 	if (**line)
// 		(*line)--;
// 	return (str);
// }

// char	*parse_argument(char **line)
// {
// 	t_list	*argument;

// 	argument = lst_new(free);
// 	if (!argument)
// 		return (NULL);
// 	while (**line && **line != ' ')
// 	{
// 		if (**line == '\'')
// 			lst_push(argument, parse_simple_quote(line));
// 		else if (**line == '\"')
// 			lst_push(argument, parse_double_quote(line));
// 		else
// 			lst_push(argument, parse_no_quote(line));
// 		(*line)++;
// 	}
// 	while (**line == ' ')
// 		(*line)++;
// 	return ((char *)lst_reducef(argument, NULL, (t_bifun)ft_strjoin, free));
// }

// int	parse_command(t_list *commands, char **line)
// {
// 	t_list	*command;

// 	command = lst_new(free);
// 	if (!command)
// 		return (FALSE);
// 	while (**line)
// 	{
// 		lst_push(command, parse_argument(line));
// 	}
// 	lst_foreach(command, printline);
// 	// if (in_str)
// 	// {
// 	// 	printf("\033[31mMissing quote !\033[0m\n");
// 	// 	return (-1);
// 	// }
// 	lst_push(commands, command);
// 	return (TRUE);
// }

// t_list	*parse_line(char *line)
// {
// 	t_list	*commands;

// 	commands = lst_new((t_con)lst_destroy);
// 	if (!commands)
// 		return (NULL);
// 	while (*line && parse_command(commands, &line))
// 		;
// }

void	free_token(t_token *token)
{
	str_destroy(token->content);
	free(token->content);
	free(token);
}

t_token	*new_token(t_list *tokens, t_token_type type)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->content = str_new();
	token->token_type = type;
	return (lst_push(tokens, token));
}

// To protect
void	parse_line(char *line)
{
	t_list	*tokens;
	t_token	*current;

	tokens = lst_new((t_con)free_token);
	current = new_token(tokens, T_WORD);
	while (*line)
	{
		if (*line == '\"' && current->token_type != T_DOUBLE_QUOTE)
			current = new_token(tokens, T_DOUBLE_QUOTE);
		else if (*line == '\"' && current->token_type == T_DOUBLE_QUOTE)
			current = new_token(tokens, T_WORD);
		else if (*line == ' ' && current->token_type != T_DOUBLE_QUOTE
			&& current->token_type != T_WHITESPACE)
			current = new_token(tokens, T_WHITESPACE);
		else if (current->token_type != T_WORD && current->token_type != T_DOUBLE_QUOTE)
			current = new_token(tokens, T_WORD);
		str_cappend(current->content, *line);
		line++;
	}
	lst_foreach(tokens, (t_con)printline);
}

void	test(void)
{
	int			result;
	char		*buffer;

	result = 1;
	while (result)
	{
		result = get_next_line(0, &buffer);
		parse_line(buffer);
		free(buffer);
	}
}

int	main(void)
{
	test();
	return (0);
}
