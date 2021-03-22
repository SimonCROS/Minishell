#include "minishell.h"

#include <stdio.h>

int	testFirst(char *element, char *character)
{
	return (*element == *character);
}

void	test(void)
{
	int			result;
	char		*buffer;
	t_list		*list;
	char		comp;

	result = 1;
	list = lst_new(free);
	while (result)
	{
		result = get_next_line(0, &buffer);
		lst_push(list, buffer);
	}
	printf("\n--------------------\n");
	printf("Taille avant filter %d\n", list->size);
	lst_filter_in(list, (t_pre)str_not_empty);
	printf("Taille après filter %d\n", list->size);
	comp = 't';
	printf("Premier element qui commence par %c : \"%s\"\n", comp,
		lst_find_first(list, (t_bipre)testFirst, &comp));
	lst_destroy(list);
}

int	main(void)
{
	test();
	return (0);
}
