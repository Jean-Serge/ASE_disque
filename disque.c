#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disque.h"
#include "hardware.h"

int taille_secteur;

void read_sector(unsigned int cylindre, unsigned int secteur, unsigned char* buffer)
{
  int reg;

  /* On place la tête de lecture */
  seek(cylindre, secteur);

  /* On lit le disque */
  reg = HDA_DATAREGS;
  ecrire_int16(&reg, 1);

  _out(HDA_CMDREG, CMD_READ);
  _sleep(HDA_IRQ);

  /* On place le contenu du secteur dans le buffer */
  memcpy(buffer, MASTERBUFFER, taille_secteur);
}

void write_sector(unsigned int cylindre, unsigned int secteur, const unsigned char* buffer)
{
  int i, reg;
  reg = HDA_CMDREG;
  seek(cylindre, secteur);
  ecrire_int16(&reg, 1);

  for(i = 0 ; i < taille_secteur ; i++)
    {
      reg = HDA_CMDREG;
      MASTERBUFFER[i] = buffer[i];
      /* printf("%d\t%c\n", i, buffer[i]); */
      /* _out(reg++, buffer[i]); */
      /* _out(reg++, CMD_WRITE); */
      /* _sleep(HDA_IRQ); */
    }
  _out(reg++, CMD_WRITE);
  _sleep(HDA_IRQ);
}

void seek(unsigned int cylindre, unsigned int secteur)
{
  int reg = HDA_DATAREGS;
  ecrire_int16(&reg, cylindre);
  ecrire_int16(&reg, secteur);
  
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
  
  /* On récupère le nombre de cylindres 
     et de secteurs du disque */
  reg = HDA_DATAREGS;
  nbCyl = lire_int16(&reg);
  nbSec = lire_int16(&reg);

  /* On formate les secteurs un par un */
  for(i = 0 ; i < nbCyl ; i++)
    {
      for(j = 0 ; j < nbSec ; j++)
	{
	  /* On place la tête de lecture */
	  seek(i,j);

	  /* On met le secteur à 0 */
	  reg = HDA_DATAREGS;
	  ecrire_int16(&reg, 1);
	  ecrire_int16(&reg, 0);
	  ecrire_int16(&reg, 0);
	  _out(HDA_CMDREG, CMD_FORMAT);
	  _sleep(HDA_IRQ);
	}   
    }

}

void afficher_registre()
{
  int i;
  int reg = HDA_DATAREGS;
  for(i = 0 ; i < 16 ; i++)
    {
      printf("Registre %d : %d\n", i, lire_int16(&reg));
    }
}

void ecrire_int16(int *reg, int val)
{
  _out((*reg)++, val >> 8);
  _out((*reg)++, val & 0xFF);  
}

int lire_int16(int *reg)
{
  int val;
  val = _in((*reg)++) << 8;
  val += _in((*reg)++);
  return val;
}
