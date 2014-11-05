#include <stdio.h>
#include <stdlib.h>
#include "disque.h"
#include "hardware.h"

void empty_it(){}

int get_sector_size()
{
  int reg = HDA_DATAREGS;
  _out(HDA_CMDREG, CMD_DSKINFO);
  /* lire_int16(&reg); */
  /* lire_int16(&reg); */
  reg += 4;
  return lire_int16(&reg);
}

int main(void)
{
  int i;
  extern int taille_secteur;
  char* chaine;
 
  /* On initialise le matériel (pour avoir le .bin à lire) 
   les fichiers vdisk sont supprimés à la compilation */
  init_hardware("hardware.ini");
  chaine = (char*)malloc(taille_secteur);
  chaine = "Bonjour";


  taille_secteur = get_sector_size();

  for(i = 0 ; i<16 ; i++)
    IRQVECTOR[i] = empty_it;

  /* printf("===============================================\n"); */
  /* dmps(6,4); */
  /* printf("===============================================\n"); */

  frmt();

  /* printf("===============================================\n"); */
  /* dmps(6,4); */
  /* printf("===============================================\n"); */
  /* dmps(4,5); */
  write_sector(4,5,(const unsigned char*)chaine);
  dmps(4,5);
  
  return 0;
}
