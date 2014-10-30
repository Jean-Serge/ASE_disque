#ifndef _H_DRIVE
#define _H_DRIVE

#include <stdio.h>
#include <stdlib.h>
#include "include/hardware.h"
#include "include/hw_info.h"

#

/* Code de retour */
#define SUCCESS 0
#define HW_INIT_ERROR 1

/* Macro de deboggage */

#define dmps_v (int cyl, int sect){\
		printf("Lecture cylindre %d secteur %d\t", cyl, sect);	\
		dmps(cyl, sect);\
	}\

/* Structure de données utiles */
struct disk_info_s{
	/* TODO : ajouter le fabricant du disque */
	unsigned int nb_cyl;          /* Le nombre de cylindres du disque */
	unsigned int nb_sec;          /* Le nombre de secteurs du disque */
	unsigned int sec_size;        /* La taille d'un secteur */
};

extern void mkhd();

extern int is_correct_coordinates(int sec, int cyl);

extern struct disk_info_s *get_disk_info();

extern void _out_16bits(unsigned int val, unsigned int addr);

extern unsigned int _in_16bits(unsigned int addr);

extern void move_head(unsigned int cyl, unsigned int sec);

#endif
