#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CMD_SIZE 101


void display_prompt()
{
  printf(">");
}

/** 
 * TODO : sécuriser les appels à realloc
 * TODO : voir la fonction strtok_r pour sécuriser 
 * en cas de multi-thread (voir pour la suite du cours)
 */ 
void split_cmd(char *cmd, char **split, char *delim)
{
  char *token;
  int i = 1, j;
  split[0] = strtok(cmd, delim);
  
  
  while((token = strtok(NULL, delim)) != NULL)
    {
      if(i >= 10)
	split = (char **)realloc(split, (i+1)*sizeof(char *));
      split[i++] = token;
    }

    for(j = 0 ; j < i ; j++)
      printf("%s\n", split[j]);  
}

int execute(char *cmd)
{
  char **split = (char **)malloc(sizeof(char *) * 10);

  split_cmd(cmd, split, " \t\n");

  return 0;
}

int main(void)
{
  char *cmd = (char *)malloc(CMD_SIZE);
 
  while(1)
    {
      display_prompt();
      /* On lit la ligne de commande entrée */
      fgets(cmd, CMD_SIZE, stdin);
      execute(cmd);
    }
  exit(EXIT_SUCCESS);
}
