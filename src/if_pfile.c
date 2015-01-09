#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "filesystem.h"
#include "file.h"
#include "volume.h"

#define ERR_OK            0
#define ERR_ARGT          1
#define ERR_NOT_INT       2
#define ERR_NOT_VOL       3
#define ERR_INCRT_VOL     4
#define ERR_INCRT_INUMBER 5
#define ERR_OPT           6
#define MAX_VOLUME        8

void usage(){
	printf("if_pfile - affichage du contenu d'un fichier sur un volume donnée.\n\n");
	printf("\t-v volume : volume.\n");
	printf("\t-i inode  : numéro d'inode du fichier dont on souhaite afficher le contenu.\n");
	printf("\t-h help   : affiche cette aide.\n");
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
	int vol = -1;
	unsigned int inumber = -1;
	int opt;
	extern char *optarg;
	struct mbr_s *mbr;
	file_desc_t fd;
	char c;

	if(argc == 1)
		exit(ERR_ARGT);

	while((opt = getopt(argc, argv, "v:i:h")) != EOF){
		switch(opt){
		case 'v':
			vol = read_vol(optarg);
			break;
		case 'i':
			inumber = read_inumber(optarg);
			break;
		case 'h':
			usage();
			break;
		default:
			fprintf(stderr, "if_pfile : option invalide -- '%c'\n", opt);
			fprintf(stderr, "Saisissez « if_ipfile -h » pour plus d'informations.");
			exit(ERR_OPT);
		}
	}

	if(vol == -1 || inumber == -1){
		usage();
		exit(ERR_ARGT);
	}

	mkhd();

	mbr = get_mbr();
	if(vol > mbr->nvol-1){
		fprintf(stderr, "Erreur : le numéro de volume est supérieur au nombre de volume crée sur le disque.\n");
		return ERR_NOT_VOL;
	}

	load_super(vol);

	open_ifile(&fd, inumber);

	while(1){
		c = readc_ifile(&fd);
		if(c == READ_EOF)
			break;
		printf("%c ", c);
	}

	close_ifile(&fd);
	return 0;
}
