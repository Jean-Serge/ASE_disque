#include <stdio.h>

#include "disque.h"
#include "hardware.h"



void dmps(unsigned int piste, unsigned int secteur)
{
  int i, reg, sizeSec;
  reg = HDA_DATAREGS;

  
  /* On place la tête de lecture */
  ecrire_int(&reg, piste);
  ecrire_int(&reg, secteur);
  
  _out(HDA_CMDREG, CMD_SEEK);
  _sleep(HDA_IRQ);


  /* On récupère la taille d'un secteur */
  _out(HDA_CMDREG, CMD_DSKINFO);
  sizeSec = _in(HDA_DATAREGS+4) << 8;
  sizeSec += _in(HDA_DATAREGS+5);


  /* On lit le disque */
  reg = HDA_DATAREGS;
  ecrire_int(&reg, 1);

  _out(HDA_CMDREG, CMD_READ);
  _sleep(HDA_IRQ);

  /* On affiche le secteur lu */
  for(i = 0 ; i < sizeSec ; i++)
    printf("%c", MASTERBUFFER[i]);
  printf("\n");
     
}


void frmt()
{
  int i, c, nbCyl, nbSec;
  int reg = HDA_DATAREGS;
  
  _out(HDA_CMDREG, CMD_DSKINFO);
  
  for(i = 0 ; i < 16 ; i++)
    {
      printf("%d\t",i);
      c = _in(reg++);
      printf("%d\n", c);
    }
  
  /* On récupère le nombre de cylindres 
  et de secteurs du disque */
  reg = HDA_DATAREGS;
  nbCyl = lire_int(&reg);
  nbSec = lire_int(&reg);

  for(i = 0 ; i < nbCyl ; i++)
    {
      reg = HDA_DATAREGS;
      ecrire_int(&reg, i);
      ecrire_int(&reg, 0);
      _out(HDA_CMDREG, CMD_SEEK);
      reg = HDA_DATAREGS;
      ecrire_int(&reg, nbSec);
      ecrire_int(&reg, 0);
      _out(HDA_CMDREG, CMD_FORMAT);
    }

  for(i = 0 ; i < 16 ; i++)
    {
      printf("%d\t",i);
      c = _in(reg++);
      printf("%d\n", c);
    }
}

void ecrire_int(int *reg, int val)
{
  _out((*reg)++, val >> 8);
  _out((*reg)++, val & 0xFF);  
}

int lire_int(int *reg)
{
  int val;
  val = _in((*reg)++) << 8;
  val += _in((*reg)++);
  return val;
}
