#include "minishell.h"
#include <stdio.h>

void	printline(t_token *token)
{
	printf("\033[33m%d \033[31m\"\033[32m%s\033[31m\"\033[0m\n",
		token->token_type, token->buffer->string);
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
	t_list	*tokens;
	t_token	*current;
	t_token	empty;

	empty.buffer = NULL;
	empty.token_type = T_EOI;
	empty.quoted = FALSE;
	tokens = lst_new((t_con)free_token);
	current = &empty;
	while (*line)
	{
		if (!current->quoted)
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
			else
				current = new_token(tokens, T_WORD, current);
		}
		else
		{
			if (*line == *current->buffer->string)
			{
				str_cappend(current->buffer, *(line++));
				current = &empty;
				continue ;
			}
		}
		str_cappend(current->buffer, *line);
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
