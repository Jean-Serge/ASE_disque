#include <stdio.h>

#include "disque.h"
#include "hardware.h"

void empty_it(){}

int get_sector_size()
{
  int reg = HDA_DATAREGS;
  _out(HDA_CMDREG, CMD_DSKINFO);
  lire_int(&reg);
  lire_int(&reg);
  return lire_int(&reg);
}

int main(void)
{
  int i;
  extern int taille_secteur;


  /* On initialise le matériel (pour avoir le .bin à lire) 
   les fichiers vdisk sont supprimés à la compilation */
  init_hardware("hardware.ini");
  taille_secteur = get_sector_size();  printf("%d\n", taille_secteur);
  for(i = 0 ; i<16 ; i++)
    IRQVECTOR[i] = empty_it;
  
  dmps(6,4);
  /* frmt(); */
  /* dmps(8,4); */

  return 0;
}
