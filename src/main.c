#include "minishell.h"
#include <stdio.h>

void	printline(t_token *token)
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
		default:
			token_name = "eoi";
	}
	printf("\033[33m%10s \033[31m\"\033[32m%s\033[31m\"\033[0m\n",
		token_name, token->buffer->string);
}

void	free_token(t_token *token)
{
	str_destroy(token->buffer);
	free(token->buffer);
	free(token);
}

t_token	*new_token(t_list *tokens, t_token_type type, t_token *cur)
{
	t_token	*token;

	if (cur && cur->token_type == type)
		return (cur);
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->buffer = str_new();
	token->token_type = type;
	token->quoted = type == T_DOUBLE_QUOTE || type == T_SINGLE_QUOTE;
	return (lst_push(tokens, token));
}

// To protect
void	parse_line(char *line)
{
	t_token	*current;
	t_list	*tokens;
	int		escaped;
	t_token	empty;

	empty.buffer = NULL;
	empty.token_type = T_EOI;
	empty.quoted = FALSE;
	tokens = lst_new((t_con)free_token);
	current = &empty;
	escaped = 0;
	while (*line)
	{
		if (!current->quoted && !escaped)
		{
			if (*line == '|')
				current = new_token(tokens, T_PIPE, current);
			else if (*line == '\"')
				current = new_token(tokens, T_DOUBLE_QUOTE, current);
			else if (*line == '\'')
				current = new_token(tokens, T_SINGLE_QUOTE, current);
			else if (*line == ' ')
				current = new_token(tokens, T_WHITESPACE, current);
			else if (*line == '<')
				current = new_token(tokens, T_REDIRECT_IN, current);
			else if (*line == '>')
				current = new_token(tokens, T_REDIRECT_OUT, current);
			else if (*line == ';')
				current = new_token(tokens, T_LAZY_AND, current);
			else if (*line == '&')
				current = new_token(tokens, T_AND, current);
			else
				current = new_token(tokens, T_WORD, current);
			if (*line == '\\')
				escaped = 2;
		}
		else if (*line == *current->buffer->string && !escaped)
		{
			str_cappend(current->buffer, *(line++));
			current = &empty;
			continue ;
		}
		str_cappend(current->buffer, *(line++));
		escaped /= 2;
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
