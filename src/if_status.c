#include <stdlib.h>
#include <string.h>
#include "filesystem.h"
#include "volume.h"

#define ERR_OK        0
#define ERR_ARGT      1
#define ERR_NOT_INT   2
#define ERR_NOT_VOL   3
#define ERR_INCRT_VOL 4

void usage(){
	printf("if_status - affichage des informations sur un volume.\n\n");
	printf("\t if_status VOLUME\n");
}

int main(int argc, char *argv[]){
	char *endptr;
	int vol;
	struct mbr_s *mbr;
	if(argc != 2){
		usage();
		exit(ERR_ARGT);
	}

	vol = strtol(argv[1], &endptr, 10);
	if(*endptr){
		fprintf(stderr, "Erreur :  %s n'est pas un nombre.\n", endptr);
		return ERR_NOT_INT;
	}
	if(vol >= MAX_VOLUME){
		fprintf(stderr, "Erreur : le numéro de volume dépasse le nombre de volume autorisé.\n");
	}
	if(vol < 0){
		fprintf(stderr, "Erreur : le numéro de volume est inférieur à 0.\n");
		return ERR_INCRT_VOL;
	}

	mkhd();

	mbr = get_mbr();
	if(vol > mbr->nvol-1){
		fprintf(stderr, "Erreur : le numéro de volume est supérieur au nombre de volume crée sur le disque.\n");
		return ERR_NOT_VOL;
	}

	load_super(vol);

	print_super(vol);

	return 0;
}
