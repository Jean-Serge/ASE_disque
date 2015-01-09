#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
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

void usage(){
	printf("if_cfile - créer un fichier\n");
	printf("if_cfile VOLUME TYPE\n ");
	printf("Type possible :\tN -> NORMAL\n\t D -> DIRECTORY\n\t L -> LINK\n\t S -> SPECIAL.\n");
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

int main(int argc, char *argv[]){
	int ifile, vol;
	char ch;
	file_desc_t fd;

	if(argc != 3){
		usage();
		exit(ERR_ARGT);
	}

	vol = read_vol(argv[1]);

	mkhd();
	load_super(vol);

	printf("Création d'un fichier de type ");
	if(argv[2][0] == 'N'){
		printf("NORMAL");
		ifile = create_ifile(NORMAL);
	}
	if(argv[2][0] == 'D'){
		printf("DIRECTORY");
		ifile = create_ifile(DIRECTORY);
	}
	if(argv[2][0] == 'L'){
		printf("LINK");
		ifile = create_ifile(LINK);
	}
	if(argv[2][0] == 'S'){
		printf("SPECIAL");
		ifile = create_ifile(SPECIAL);
	}

	printf("\nInode %d\n", ifile);
	fflush(stdout);
	open_ifile(&fd, ifile);

	while(read(STDIN_FILENO, &ch, 1) > 0){
		printf("%c", ch);
		writec_ifile(&fd, ch);
	}

	close_ifile(&fd);
	return 0;
}
