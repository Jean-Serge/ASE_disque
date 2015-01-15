#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmd.h"

#define CMD_SIZE 101

/** 
 * Affiche le prompt.
 * TODO : compléter pour un affichage plus complet.
 */
void display_prompt()
{
  printf(">");
}

/**
 * Lit au plus size - 1 caractères sur l'entrée standard et les 
 * écrit dans cmd, ignore les caractères suivant.
 * Si une ligne entrée fait plus de size caractères, cette fonction
 * ignorera les caractères en plus et ne les considérera pas comme une 
 * autre chaîne.
 */ 
void my_fgets(char *cmd, unsigned int size)
{
  int i;
  char c;
  /* On lit CMD_SIZE - 1 caractères sur l'entrée standard. */
  for(i = 0 ; i < size - 1 ; i++)
    {
      c = getchar();

      if(c == '\n' || c == EOF)
	{
	  cmd[i] = '\0';
	  return;
	}

      cmd[i] = c;
    }

  cmd[i+1] = '\0';

  /* Ignore les caractère suivant. */
  while(1)
    {
      c = getchar();
      if(c == EOF || c == '\n')
	return;
    }
}

int main(void)
{
  char *cmd = (char *)malloc(CMD_SIZE);
 
  while(1)
    {
      display_prompt();
      /* On lit la ligne de commande entrée */
      my_fgets(cmd, CMD_SIZE);

      execute(cmd);
    }

  exit(EXIT_SUCCESS);
}
