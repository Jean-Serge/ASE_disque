#include <stdio.h>

#include "disque.h"
#include "hardware.h"



void dmps(unsigned int piste, unsigned int secteur)
{
  int i, reg;
  reg = HDA_DATAREGS;
  
  /* On set les registre pour le SEEK */
  _out(reg++, piste>>8);
  _out(reg++, piste & 0xFF);
  _out(reg++, secteur>>8);
  _out(reg++, secteur & 0xFF);
  
  _out(HDA_CMDREG, CMD_SEEK);
  _sleep(HDA_IRQ);

  reg = HDA_DATAREGS;
  _out(reg++, 0);
  _out(reg++, 1);

  _out(HDA_CMDREG, CMD_READ);
  _sleep(HDA_IRQ);

  /* TODO Récupérer la taille d'un secteur */
  for(i = 0 ; i < 16 ; i++)
    printf("%c", MASTERBUFFER[i]);
  printf("\n");
     
}
