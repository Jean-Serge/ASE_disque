#include <stdio.h>
#include <stdlib.h>

#include "disque.h"
#include "hardware.h"

int taille_secteur;

void read_sector(unsigned int cylindre, unsigned int secteur, unsigned char* buffer)
{
  int i, reg;
  printf("%d\n", taille_secteur);
  /* On place la tête de lecture */
  seek(cylindre, secteur);

  /* On lit le disque */
  reg = HDA_DATAREGS;
  ecrire_int(&reg, 1);

  /* _out(HDA_CMDREG, CMD_READ); */
  /* _sleep(HDA_IRQ); */

  /* On affiche le secteur lu */
  for(i = 0 ; i < taille_secteur ; i++)
    {

    buffer[i] = MASTERBUFFER[i];
    printf("%c", buffer[i]);

    }
}

void write_sector(unsigned int cylindre, unsigned int secteur, unsigned char* buffer)
{
  
}

void seek(unsigned int cylindre, unsigned int secteur)
{
  int reg = HDA_DATAREGS;
  ecrire_int(&reg, cylindre);
  ecrire_int(&reg, secteur);
  
  _out(HDA_CMDREG, CMD_SEEK);
  _sleep(HDA_IRQ);
}


void dmps(unsigned int piste, unsigned int secteur)
{
  int i;
  unsigned char *buffer = (unsigned char*) malloc(taille_secteur);

  read_sector(piste, secteur, buffer);

  /* On affiche le secteur lu */
  for(i = 0 ; i < taille_secteur ; i++)
    printf("%c", buffer[i]);
  printf("\n");
     
}



void frmt()
{
  int i, j, nbCyl, nbSec;
  int reg = HDA_DATAREGS;
  
  _out(HDA_CMDREG, CMD_DSKINFO);
  /* _sleep(HDA_IRQ); */
  
  /* On récupère le nombre de cylindres 
     et de secteurs du disque */
  reg = HDA_DATAREGS;
  nbCyl = lire_int(&reg);
  nbSec = lire_int(&reg);
  /* On formate les secteurs un par un */
  for(i = 0 ; i < nbCyl ; i++)
    {
      for(j = 0 ; j < nbSec ; j++)
	{
	  /* On place la tête de lecture */
	  reg = HDA_DATAREGS;
	  ecrire_int(&reg, i);
	  ecrire_int(&reg, j);
	  _out(HDA_CMDREG, CMD_SEEK);

	  /* On met le secteur à 0 */
	  reg = HDA_DATAREGS;
	  ecrire_int(&reg, 1);
	  ecrire_int(&reg, 0);
	  ecrire_int(&reg, 0);
	  _out(HDA_CMDREG, CMD_FORMAT);
	  _sleep(HDA_IRQ);
	}   
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
