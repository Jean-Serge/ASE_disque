#include "my_cd.h"

/**
 * TODO 
 */
int my_cd(char **args, int argc)
{
  	printf("Je change de répertoire.\n");
  	printf("Il y a %d arguments.\n", argc);
	
	if(argc == 1)
	{
		wd = get_home();
	}
  return 0;
}
