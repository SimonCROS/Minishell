#include <stdio.h>

#include "minishell.h"

void	print(char *str)
{
	printf("|%s| ", str);
}

void	printcommand(t_command *command)
{
	lst_foreach(command->args, (t_con)print);
	printf("\n");
}

void	printtoken(t_token *token)
{
	char	*token_name;

	switch (token->token_type)
	{
		case T_WORD:
			token_name = "word";
			break;
		case T_WHITESPACE:
			token_name = "space";
			break;
		case T_LAZY_AND:
			token_name = "lazy and";
			break;
		case T_AND:
			token_name = "and";
			break;
		case T_SINGLE_QUOTE:
			token_name = "sin quotes";
			break;
		case T_DOUBLE_QUOTE:
			token_name = "quotes";
			break;
		case T_PIPE:
			token_name = "pipe";
			break;
		case T_REDIRECT_IN:
			token_name = "in";
			break;
		case T_REDIRECT_OUT:
			token_name = "out";
			break;
		case T_DOLLAR:
			token_name = "dollar";
			break;
		default:
			token_name = "eoi";
	}
	printf("\033[33m%10s \033[31m\"\033[32m%s\033[31m\"\033[0m\n",
		token_name, *(token->buffer));
}

void	free_token(t_token *token)
{
	if (token)
	{
		if (token->buffer)
			free(*(token->buffer));
		free(token->buffer);
	}
	free(token);
}

void	free_command(t_command *command)
{
	if (command)
		lst_destroy(command->args);
	free(command);
}

int	token_equals(t_token *t1, t_token *t2)
{
	if (!t1 || !t2 || !t1->buffer || !t2->buffer)
		return (FALSE);
	return (t1->quoted == t2->quoted && t1->token_type == t2->token_type
		&& ft_str_equals(*(t1->buffer), *(t2->buffer)));
}

t_token	*new_token(t_list *tokens, t_token_type type, t_token *cur)
{
	t_token	*token;

	if (cur && cur->token_type == type)
		return (cur);
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->token_type = type;
	token->quoted = type == T_DOUBLE_QUOTE || type == T_SINGLE_QUOTE;
	token->separator = type == T_AND || type == T_LAZY_AND || type == T_PIPE;
	token->buffer = str_new();
	if (!token->buffer)
	{
		free_token(token);
		return (NULL);
	}
	if (!tokens)
		return (token);
	if (!lst_push(tokens, token))
	{
		free_token(token);
		return (NULL);
	}
	return (token);
}

t_command	*new_command(t_list *commands, t_token_type parent_relation)
{
	t_command	*command;

	command = malloc(sizeof(t_command));
	if (!command)
		return (NULL);
	command->args = lst_new(free);
	if (!command->args)
	{
		free_command(command);
		return (NULL);
	}
	command->redirect_in = NULL;
	command->redirect_out = NULL;
	command->parent_relation = parent_relation;
	if (!commands)
		return (command);
	if (!lst_push(commands, command))
	{
		free_command(command);
		return (NULL);
	}
	return (command);
}

t_token	null_token(void)
{
	return ((t_token){NULL, T_NONE, FALSE, FALSE});
}

// To protect
int	tokenize(t_list *tokens, char *line)
{
	t_token	*current;
	int		escaped;
	t_token	empty;
	int		i;

	empty = null_token();
	current = &empty;
	escaped = 0;
	i = -1;
	while (line[++i])
	{
		if (line[i] == '\\' && !escaped)
		{
			if (!current->quoted)
				current = new_token(tokens, T_WORD, current);
			escaped = 1;
			continue ;
		}
		if (!current->quoted && !escaped)
		{
			if (line[i] == '|')
				current = new_token(tokens, T_PIPE, current);
			else if (line[i] == '\"')
				current = new_token(tokens, T_DOUBLE_QUOTE, current);
			else if (line[i] == '\'')
				current = new_token(tokens, T_SINGLE_QUOTE, current);
			else if (line[i] == ' ' || line[i] == '\t')
				current = new_token(tokens, T_WHITESPACE, current);
			else if (line[i] == '<')
				current = new_token(tokens, T_REDIRECT_IN, current);
			else if (line[i] == '>')
				current = new_token(tokens, T_REDIRECT_OUT, current);
			else if (line[i] == ';')
				current = new_token(tokens, T_LAZY_AND, current);
			else if (line[i] == '&')
				current = new_token(tokens, T_AND, current);
			else if (line[i] == '$')
				current = new_token(tokens, T_DOLLAR, current);
			else if (current->token_type != T_DOLLAR)
				current = new_token(tokens, T_WORD, current);
		}
		else if (line[i] == **(current->buffer) && !escaped)
		{
			str_cappend(current->buffer, line[i]);
			current = &empty;
			continue ;
		}
		if (current->token_type == T_NONE)
			current = new_token(tokens, T_WORD, current);
		str_cappend(current->buffer, line[i]);
		escaped = 0;
	}
	return (!escaped);
}

int	is_valid(t_token *token)
{
	if (token->separator)
	{
		if (token->token_type == T_AND && ft_strlen(*(token->buffer)) != 2)
			return (FALSE);
		else if ((token->token_type == T_LAZY_AND
				|| token->token_type == T_PIPE)
			&& ft_strlen(*(token->buffer)) != 1)
			return (FALSE);
	}
	if (token->quoted)
	{
		if (ft_strlen(*(token->buffer)) < 2 || (*(token->buffer))[0] !=
			(*(token->buffer))[ft_strlen(*(token->buffer)) - 1])
			return (FALSE);
	}
	return (TRUE);
}

int	parse_text_token(t_command *command, t_token *token)
{
	
	return (TRUE);
}

int	parse(t_list *commands, t_list *tokens)
{
	char		*argument;
	t_token		*prev;
	t_token		*current;
	t_command	*command;
	t_iterator	tokens_iterator;
	int			space;

	prev = NULL;
	space = 0;
	command = new_command(commands, T_LAZY_AND);
	tokens_iterator = iterator_new(tokens);
	argument = NULL;
	while (iterator_has_next(&tokens_iterator))
	{
		current = (t_token *)iterator_next(&tokens_iterator);
		if (current->token_type == T_WHITESPACE)
		{
			space = 1;
			continue ;
		}
		if (!is_valid(current))
			return (FALSE);
		else if (current->separator && !prev)
			return (FALSE);
		else if (current->separator && prev->separator)
			return (FALSE);
		if ((current->separator || space) && argument)
		{
			lst_push(command->args, argument);
			argument = NULL;
		}
		if (current->separator)
			command = new_command(commands, current->token_type);
		else
			str_append(&argument, *(current->buffer));
		prev = current;
		space = 0;
	}
	if (argument)
		lst_push(command->args, argument);
	lst_foreach(tokens, (t_con)printtoken);
	printf("---------------\n");
	lst_foreach(commands, (t_con)printcommand);
	return (TRUE);
}

void	parse_line(char *line)
{
	t_list	*tokens;
	t_list	*commands;

	tokens = lst_new((t_con)free_token);
	commands = lst_new((t_con)free_command);

	if (!tokenize(tokens, line) || !parse(commands, tokens))
		printf("Ah !\n");

	lst_destroy(tokens);
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

// a"b'c'd"e'f"g"h'i|<>\|\<\>&&\&\&;\;
