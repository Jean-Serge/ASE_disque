#include <stdlib.h>
#include <stdio.h>
#include "include/hardware.h"
#include "drive.h"
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define SUCCESS  0
#define ERR_COOR 1
#define ERR_OPT  2

void dmps(unsigned int cyl, unsigned int sec, unsigned char *buffer){
	struct disk_info_s *dinfo = get_disk_info();
	int i = 0;
	if(!is_correct_coordinates(sec, cyl)){
		fprintf(stderr, "Au moins une des coordonnées passé en paramêtres est hors limite.\n");
		fprintf(stderr, "Les valeurs autorisé pour les cylindres vont de 0 à %d exclu.\n", dinfo->nb_cyl);
		fprintf(stderr, "Les valeurs autorisé pour les secteurs vont de 0 à %d exclu.\n", dinfo->nb_sec);
		exit(ERR_COOR);
	}

	move_head(cyl, sec);

	/* Demande de données du disque */
	_out_16bits(HDA_DATAREGS, 1);
	_out(HDA_CMDREG, CMD_READ);

	/* Attente de l'IRQ */
	_sleep(HDA_IRQ);
	for(; i < HDA_SECTORSIZE; i++)
		buffer[i] = MASTERBUFFER[i];
}

void usage(){
	printf("Affichage d'un secteur du disque.\n");
	printf("Si plusieurs option d'affichage sont passé en paramêtres, seul la dernière est prise en compte.\n");
	printf("Liste des options :\n");
	printf("\t-a affichage des secteurs en ASCII.\n");
	printf("\t-x affichage des secteurs en hexadécimale.\n");
	printf("\t-c spécifie le cylindre à afficher, 0 par défaut.\n");
	printf("\t-s spécifie le secteur à afficher, 0 par défaut.\n");
	printf("\t-h affiche cette aide.\n");
	exit(SUCCESS);
}

int main(int argc, char **argv){
	int ascii_flag = 0;   /* Si égale à 1 affichage ascii sinon affichage hex */
	int cyl        = 0;
	int sec        = 0;
	int i          = 0;
	int opt;
	unsigned char *buffer = (unsigned char *)malloc(HDA_SECTORSIZE);
	char str_print[4];
	extern char *optarg;

	if(argc == 1)
		usage();
	while((opt = getopt(argc, argv, "axhc:s:")) != EOF){
		switch(opt){
		case 'a':
			ascii_flag = 1;
			break;
		case 'x':
			ascii_flag = 0;
			break;
		case 'h':
			usage();
			break;
		case 'c':
			cyl = atoi(optarg);
			break;
		case 's':
			sec = atoi(optarg);
			break;
		default:
			fprintf(stderr, "dmps : option invalide -- '%c'\n", opt);
			fprintf(stderr, "Saisissez « dmps -h » pour plus d'informations.");
			exit(ERR_OPT);
		}
	}

	mkhd();

	dmps(cyl, sec, buffer);

	if(ascii_flag){
		strncpy(str_print, "%c ", 3);
	}
	else{
		strncpy(str_print, "%X ", 3);
	}
	str_print[4-1] = 0;


	for(; i < HDA_SECTORSIZE; i++){
		printf(str_print, buffer[i]);
	}

	exit(SUCCESS);
}
