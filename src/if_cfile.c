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
	printf("if_cfile - copie un fichier vers un nouveau nouvellement créer\n");
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
	int inumber, vol, ifile;
	file_desc_t src, dest;
	char c;

	if(argc != 3){
		usage();
		exit(ERR_ARGT);
	}

	vol = read_vol(argv[1]);
	inumber = read_inumber(argv[2]);

	mkhd();
	load_super(vol);

	switch(get_fd_type(&src)){
	case NORMAL:
		ifile = create_ifile(NORMAL);
		break;
	case DIRECTORY:
		ifile = create_ifile(DIRECTORY);
		break;
	case LINK:
		ifile = create_ifile(LINK);
		break;
	case SPECIAL:
		ifile = create_ifile(SPECIAL);
		break;
	}



	printf("Nouveau fichier d'inumber %d\n", ifile);

	open_ifile(&src, inumber);
	open_ifile(&dest, ifile);

	while(1){
		c = readc_ifile(&src);
		if(c == READ_EOF)
			break;
		writec_ifile(&dest, c);
	}

	close_ifile(&src);
	close_ifile(&dest);

	return 0;
}
