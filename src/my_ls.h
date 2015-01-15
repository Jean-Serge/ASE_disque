#ifndef H_MY_LS
#define H_MY_LS

#include <stdio.h>

/**
 * Effectue notre version de la commande ls.
 * Liste le contenu du répertoire pathname.
 * Les arguments excluent le nom de la commande elle même.
 */
extern void my_ls(char *pathname, char **args);

#endif
