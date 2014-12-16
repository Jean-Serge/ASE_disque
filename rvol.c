#include "volume.h"
#include "drive.h"
#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 0

void usage(){
	printf("Supprime le volumes du disque désigné en paramêtre.\n");
	printf("Le volume est désigné par un entier supérieur à 0 et inférieur à %d.\n", MAX_VOLUME);
	exit(SUCCESS);
}

void rvol(int nvol,struct mbr_s *mbr){
	mbr->nvol--;
	printf("nvol : %d\n", mbr->nvol);
	for(; nvol < MAX_VOLUME-2; nvol++){
		mbr->volume[nvol] = mbr->volume[nvol+1];
	}
	save_mbr();
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
		rvol(nvol, mbr);
		printf("Suppression terminé.\n");
	}
	else{
		printf("Le numéro de volume correspond à un volume non initialisé.\n");
		return 1;
	}
	return 0;
}
