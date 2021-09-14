#include "minishell.h"

static void	tokenize_char2(t_token *parent, t_token **cur, char c)
{
	if (c == ' ' || c == '\t')
		*cur = new_token(parent, T_WHITESPACE, *cur, TRUE);
	else if (ft_isdigit(c)
		&& ((*cur)->type == T_NUMBER || (*cur)->type == T_WHITESPACE
			|| (*cur)->type == T_NONE || (*cur)->separator))
		*cur = new_token(parent, T_NUMBER, *cur, TRUE);
	else if (c == '<')
		*cur = new_token(parent, T_REDIRECT_IN, *cur, TRUE);
	else if (c == '>')
		*cur = new_token(parent, T_REDIRECT_OUT, *cur, TRUE);
	else if (c == ';')
		*cur = new_token(parent, T_SEPARATOR, *cur, TRUE);
	else if ((*cur)->type != T_VAR
		|| !is_valid_variable_char(c, *(*cur)->buffer))
		*cur = new_token(parent, T_WORD, *cur, TRUE);
}

static int	tokenize_char(t_token *parent, char **line, t_token **cur, char c)
{
	if (c == '|')
		*cur = new_token(parent, T_PIPE, *cur, TRUE);
	else if (c == '\"')
	{
		*cur = new_token(parent, T_DOUBLE_QUOTE, *cur, TRUE);
		if (!tokenize(*cur, line, 0))
			return (FALSE);
		return (TRUE);
	}
	else if (c == '\'')
	{
		*cur = new_token(parent, T_SINGLE_QUOTE, *cur, TRUE);
		if (!tokenize(*cur, line, 0))
			return (FALSE);
		return (TRUE);
	}
	else
		tokenize_char2(parent, cur, c);
	return (TRUE);
}

static int	tokenize2(t_token *parent, t_parsing_arg args, char c,
	t_token **cur)
{
	if (c == '\\' && !*args.params)
	{
		if (!((*cur)->is_quote))
			*cur = new_token(parent, T_WORD, *cur, TRUE);
		*args.params = 1;
		return (1);
	}
	else if ((*cur)->type != T_SINGLE_QUOTE && c == '$' && !*args.params)
	{
		*cur = new_token(parent, T_VAR, *cur, TRUE);
		return (1);
	}
	if (!parent->is_quote && !*args.params)
	{
		if (!tokenize_char(parent, args.argument, cur, c))
			return (-1);
	}
	else if ((c == '\"' && !*args.params && parent->type == T_DOUBLE_QUOTE))
		return (2);
	else if ((c == '\'' && parent->type == T_SINGLE_QUOTE))
		return (2);
	else if ((*cur)->type != T_VAR
		|| !is_valid_variable_char(c, *(*cur)->buffer))
		*cur = new_token(parent, T_WORD, *cur, TRUE);
	return (0);
}

int	tokenize(t_token *parent, char **line, int ret)
{
	t_token	*cur;
	int		escaped;
	t_token	empty;
	char	c;

	empty = null_token();
	cur = &empty;
	escaped = parent->type == T_SINGLE_QUOTE;
	while (**line)
	{
		c = **line;
		++*line;
		ret = tokenize2(parent, (t_parsing_arg){line, &escaped, NULL}, c, &cur);
		if (ret == -1)
			return (FALSE);
		else if (ret == 1)
			continue ;
		else if (ret == 2)
			return (TRUE);
		str_cappend(cur->buffer, c);
		escaped = parent->type == T_SINGLE_QUOTE;
	}
	if (escaped || parent->is_quote)
		ft_putendl_fd("minish: syntax error: unexpected end of file", 2);
	return (!escaped && !parent->is_quote);
}
