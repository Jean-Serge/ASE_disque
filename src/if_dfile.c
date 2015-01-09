#include <stdlib.h>
#include "file.h"

#define ERR_OK            0
#define ERR_ARGT          1
#define ERR_NOT_INT       2
#define ERR_NOT_VOL       3
#define ERR_INCRT_VOL     4
#define ERR_INCRT_INUMBER 5
#define ERR_OPT           6

void usage(){
	printf("if_dfile - supprime un fichier\n");
	printf("if_dfile VOLUME INUMBER\n ");
}

int read_vol(char *volume){
	char *endptr;
	int vol = strtol(volume, &endptr, 10);
	if(*endptr){
		fprintf(stderr, "Erreur :  %s n'est pas un nombre.\n", endptr);
		exit(ERR_NOT_INT);
	}
	if(vol >= MAX_VOLUME){
		fprintf(stderr, "Erreur : le numéro de volume dépasse le nombre de volume autorisé.\n");
		exit(ERR_INCRT_VOL);
	}
	if(vol < 0){
		fprintf(stderr, "Erreur : le numéro de volume est inférieur à 0.\n");
		exit(ERR_INCRT_VOL);
	}
	return vol;
}

int read_inumber(char *inumber){
	char *endptr;
	int vol = strtol(inumber, &endptr, 10);
	if(*endptr){
		fprintf(stderr, "Erreur :  %s n'est pas un nombre.\n", endptr);
		exit(ERR_NOT_INT);
	}
	if(vol < 0){
		fprintf(stderr, "Erreur : l'inumber est inférieur à 0.\n");
		exit(ERR_INCRT_INUMBER);
	}
	return vol;
}

int main(int argc, char *argv[]){
	int inumber, vol;

	if(argc != 3){
		usage();
		exit(ERR_ARGT);
	}

	vol = read_vol(argv[1]);
	inumber = read_inumber(argv[2]);
	mkhd();
	load_super(vol);

	delete_ifile(inumber);

	return 0;
}
