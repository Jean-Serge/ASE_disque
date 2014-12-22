#include "volume.h"
#include "drive.h"
#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 0

void usage(){
	printf("Liste les information sur le volumes du disque désigné en paramêtre.\n");
	printf("Le volume est désigné par un entier supérieur à 0 et inférieur à %d.\n", MAX_VOLUME);
	exit(SUCCESS);
}

void print_info(int nvol,struct mbr_s *mbr){
	struct volume_s vol;
	vol = mbr->volume[nvol];
	printf("Affichage des informations du volume %d :\n", nvol);
	printf("\tCylindre de départ : %d\n", vol.start_cyl);
	printf("\tSecteur de départ : %d\n", vol.start_sec);
	printf("\tNombre de secteur : %d\n", vol.nsector);
	printf("\tType : ");
	if(vol.type == BASE){
		printf("base\n");
	}
	else{
		if(vol.type == BASE){
			printf("annexe\n");
		}
		else{
			printf("other\n");
		}
	}
}


int main(int argc, char **argv){
	int nvol;
	struct mbr_s *mbr;

	if(argc != 2){
		usage();
	}
	mkhd();
	nvol = atoi(argv[1]);
	mbr = get_mbr();

	/* vérification de surface */
	if(nvol < 0 || nvol > MAX_VOLUME){
		usage();
	}

	if(nvol < mbr->nvol){
		print_info(nvol, mbr);
	}
	else{
		printf("Le numéro de volume correspond à un volume non initialisé.\n");
		return 1;
	}
	return 0;
}
