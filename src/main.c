#include "minishell.h"

#include <stdio.h>
int	main(void)
{
	char	*buffer;

	while (1)
	{
		get_next_line(0, &buffer);
		printf("Line : \"%s\"\n", buffer);
		free(buffer);
	}
	return (0);
}
