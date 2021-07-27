#include <stdio.h>
#include "minishell.h"

void	print(char *str)
{
	printf("|%s| ", str);
}

void	print_redirect(t_redirect *redirection)
{
	print(redirection->str);
}

void	printtoken(t_token *token, t_token *parent)
{
	char	*token_name;

	if (!token || token->type == T_NONE)
		return ;
	switch (token->type)
	{
		case T_WORD:
			token_name = "word";
			break;
		case T_WHITESPACE:
			token_name = "space";
			break;
		case T_SEPARATOR:
			token_name = "separator";
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
		case T_VAR:
			token_name = "variable";
			break;
		default:
			token_name = "eoi";
	}
	if (token->children->size)
		lst_foreachp(token->children, (t_bicon)printtoken, token);
	else
	{
		if (!parent)
			printf("\033[33m%10s \033[31m\"\033[32m%s\033[31m\"\033[0m\n",
				token_name, *(token->buffer));
		else
			printf("\033[34m%10s \033[31m\"\033[32m%s\033[31m\"\033[0m\n",
				token_name, *(token->buffer));
	}
}

void	printcommand(t_command *command)
{
	parse(command);
	if (command->children->size)
		lst_foreachp(command->children, (t_bicon)printtoken, NULL);
	if (command->args->size)
	{
		lst_foreach(command->args, (t_con)print);
		printf("\n");
	}
	if (command->redirect_out->size)
	{
		printf("> ");
		lst_foreach(command->redirect_out, (t_con)print_redirect);
		printf("\n");
	}
	if (command->redirect_in->size)
	{
		printf("< ");
		lst_foreach(command->redirect_in, (t_con)print_redirect);
		printf("\n");
	}
	lst_clear(command->args);
	lst_clear(command->redirect_in);
	lst_clear(command->redirect_out);
	printf("---------------\n");
}
