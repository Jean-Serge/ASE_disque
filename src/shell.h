#ifndef H_SHELL
#define H_SHELL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CMD_SIZE 101

extern void display_prompt();

/**
 * Lit au plus size - 1 caractères sur l'entrée standard et les 
 * écrit dans cmd, ignore les caractères suivant.
 * Si une ligne entrée fait plus de size caractères, cette fonction
 * ignorera les caractères en plus et ne les considérera pas comme une 
 * autre chaîne.
 */ 
extern void my_fgets(char *cmd, unsigned int size);

#endif
