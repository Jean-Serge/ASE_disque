#include <stdio.h>

#include "disque.h"
#include "hardware.h"



void dmps(unsigned int piste, unsigned int secteur)
{
  int i, reg;
  reg = HDA_DATAREGS;
  
  /* On place la tête de lecture */
  _out(reg++, piste>>8);
  _out(reg++, piste & 0xFF);
  _out(reg++, secteur>>8);
  _out(reg++, secteur & 0xFF);
  
  _out(HDA_CMDREG, CMD_SEEK);
  _sleep(HDA_IRQ);


  /* On lit le disque */
  reg = HDA_DATAREGS;
  _out(reg++, 0);
  _out(reg++, 1);

  _out(HDA_CMDREG, CMD_READ);
  _sleep(HDA_IRQ);

  /* TODO Récupérer la taille d'un secteur */
  /* On affiche le secteur lu */
  for(i = 0 ; i < 16 ; i++)
    printf("%c", MASTERBUFFER[i]);
  printf("\n");
     
}


void frmt()
{
  int i, c;
  int reg = HDA_DATAREGS;
  
  _out(HDA_CMDREG, CMD_DSKINFO);
  
  for(i = 0 ; i < 16 ; i++)
    {
      printf("%d\t",i);
      c = _in(reg++);
      printf("%d\n", c);
    }
  
}
