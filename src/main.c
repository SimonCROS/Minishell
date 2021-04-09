#include <stdio.h>

#include "minishell.h"

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
		token_name, *(token->buffer));
}

void	free_token(t_token *token)
{
	free(*(token->buffer));
	free(token->buffer);
	free(token);
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
	token->buffer = str_new();
	if (!token->buffer)
		return (0);
	token->token_type = type;
	token->quoted = type == T_DOUBLE_QUOTE || type == T_SINGLE_QUOTE;
	if (!tokens)
		return (token);
	return (lst_push(tokens, token));
}

// To protect
void	tokenize(t_list	*tokens, char *line)
{
	t_token	*current;
	int		escaped;
	t_token	empty;

	empty.buffer = NULL;
	empty.token_type = T_EOI;
	empty.quoted = FALSE;
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
		else if (*line == **(current->buffer) && !escaped)
		{
			str_cappend(current->buffer, *(line++));
			current = &empty;
			continue ;
		}
		str_cappend(current->buffer, *(line++));
		escaped /= 2;
	}
}

void	parse_line(char *line)
{
	t_list	*tokens;

	tokens = lst_new((t_con)free_token);
	tokenize(tokens, line);

	// lst_find_first(list)

	lst_foreach(tokens, (t_con)printline);
	lst_destroy(tokens);
	// free(*(and.buffer));
	// free(and.buffer);
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
