#include "minishell.h"

static void	push_argument(t_list **lst, t_command *command, char **arg,
	int *params)
{
	t_redirect	*redirection;
	int			fd;
	int			append;

	fd = params[1];
	append = params[2];
	if (*lst == command->redirect_in || *lst == command->redirect_out)
	{
		redirection = malloc(sizeof(t_redirect));
		if (redirection)
			*redirection = (t_redirect){fd, *arg, append};
		lst_push(*lst, redirection);
		params[1] = 1;
		params[2] = FALSE;
	}
	else
		lst_push(*lst, *arg);
	*arg = NULL;
	*lst = command->args;
}

static void	parse_token(t_command *command, t_token *cur, t_list **lst,
	t_parsing_argument pack)
{
	if (cur->type == T_WHITESPACE)
		pack.params[0] = 1;
	else if (cur->type == T_NUMBER && pack.next
		&& pack.next->type == T_REDIRECT_OUT)
		pack.params[1] = ft_atoi(*cur->buffer);
	else
	{
		if ((cur->separator || is_redirection(NULL, cur) || pack.params[0])
			&& *pack.argument)
			push_argument(lst, command, pack.argument, pack.params);
		if (cur->type == T_REDIRECT_OUT)
		{
			*lst = command->redirect_out;
			pack.params[2] = ft_strlen(*cur->buffer) == 2;
		}
		else if (cur->type == T_REDIRECT_IN)
			*lst = command->redirect_in;
		else if (!cur->separator)
			read_token(cur, pack.argument);
		pack.params[0] = FALSE;
	}
}

int	parse(t_command *command)
{
	char		*argument;
	t_token		*cur;
	t_iterator	it;
	t_list		*lst;
	int			*redirect_params;

	lst = command->args;
	it = iterator_new(command->children);
	argument = NULL;
	redirect_params = (int [3]){FALSE, 1, FALSE};
	while (iterator_has_next(&it))
	{
		cur = (t_token *)iterator_next(&it);
		parse_token(command, cur, &lst, (t_parsing_argument){
			.argument = &argument,
			.next = NULL,
			.params = redirect_params,
		});
	}
	if (argument)
		push_argument(&lst, command, &argument, redirect_params);
	return (TRUE);
}
