#include "my_pwd.h"

/**
 * TODO 
 */
int my_pwd(char **args, int argc)
{
	if(wd == NULL)
	{
		printf("Working directory not defined.\n");
		return 1;
	}
	printf("%s\n", wd);
 
 	return 0;
}
