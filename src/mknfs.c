#include <stdlib.h>
#include <string.h>
#include "drive.h"
#include "volume.h"
#include "filesystem.h"
#include "drive.h"

#define ENV_VAR_NAME "CURRENT_VOLUME"

#define ERR_OK        0
#define ERR_NO_ENV    1
#define ERR_NOT_INT   2
#define ERR_NOT_VOL   3
#define ERR_INCRT_VOL 4

int main(int argc, char *argv[]){
	char *endptr;
	char *env_value;
	char name[SUPER_SZ_NAME];
	int crt_vol;
	struct mbr_s *mbr;
	env_value = getenv(ENV_VAR_NAME);
	if(!env_value){
		fprintf(stderr, "Pas de variable d'environement $%s\n", ENV_VAR_NAME);
		return ERR_NO_ENV;
	}
	crt_vol = strtol(env_value, &endptr, 10);
	if(*endptr){
		fprintf(stderr, "Erreur :  %s n'est pas un nombre.\n", endptr);
		return ERR_NOT_INT;
	}
	if(crt_vol >= MAX_VOLUME){
		fprintf(stderr, "Erreur : le numéro de volume dépasse le nombre de volume autorisé.\n");
	}
	if(crt_vol < 0){
		fprintf(stderr, "Erreur : le numéro de volume est inférieur à 0.\n");
		return ERR_INCRT_VOL;
	}

	if(argc == 2){
		strncpy(name, argv[1], SUPER_SZ_NAME);
	}

	mkhd();

	mbr = get_mbr();
	if(crt_vol > mbr->nvol-1){
		fprintf(stderr, "Erreur : le numéro de volume est supérieur au nombre de volume crée sur le disque.\n");
		return ERR_INCRT_VOL;
	}

	printf("Initilisation du système de fichier numéro %d", crt_vol);
	if(argc == 2){
		printf(" avec comme nom de volume: \"%s\".\n", name);
		init_super(crt_vol, name);
		return 0;
	}
	printf(".\n");
	init_super(crt_vol, NULL);
	return 0;
}
