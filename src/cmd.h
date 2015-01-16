#ifndef H_CMD
#define H_CMD

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 

#include "my_ls.h"
#include "my_cd.h"
#include "my_pwd.h"

/**
 * Split la ligne de commande cmd en utilisant delim et en 
 * stockant le résultat dans split.
 * Retourne le nombre d'argument (incluant la commande elle-même).
 */
extern unsigned int split_cmd(char *cmd, char **split, char *delim);

/**
 * Exécute la ligne de commande passée en paramètre.
 */
extern int execute(char *cmd);

#endif 
