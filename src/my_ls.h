#ifndef H_MY_LS
#define H_MY_LS

#include <stdio.h>
#include <string.h>

/**
 * Effectue notre version de la commande ls.
 * Les arguments incluent le nom de la commande elle même.
 * Retourne le code de retour de la commande ls.
 */
extern int my_ls(char **args, int argc);

#endif
