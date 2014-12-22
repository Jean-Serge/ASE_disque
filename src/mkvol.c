#include "mkvol.h"

static struct mbr_s *mbr = NULL;

void usage(){
	printf("Création d'un disque.\n");
	printf("Liste des options :\n");
	printf("\t-fc first cylinder : cylindre de départ de la partition.\n");
	printf("\t-fs first sector   : secteur de départ de la parition.\n");
	printf("\t-s size            : taille de la partition à créer.\n");
	printf("\t-h help            : affiche cette aide.\n");
	exit(SUCCESS);
}

int chck_possible(unsigned int fc, unsigned int fs, unsigned int size){
	int i  = 0;
	int lc = fc;                    /* last cylinder */
	int ls = fs;                    /* last sector */
	int rest;                       /* the number of sector remaining */

	if(!mbr)
		mbr = get_mbr();

	lc = fc + size / HDA_MAXSECTOR;
	rest = size % HDA_MAXSECTOR;
	ls = rest - (rest - fs) - 1;

	for(; i < mbr->nvol; i++){
		if((convert_cyl_sec(i, fc, fs) != -1) ||
		   (convert_cyl_sec(i, lc, ls) != -1)){
			return 0;
		}
	}
	return 1;
}

int chck_present_flag(unsigned int fc, unsigned int fs, unsigned int sz){
	int all = 1;
	if(fc == 0){
		printf("Missing argument : -fc not found.\n");
	}
		all = 0;
	if(fs == 0){
		printf("Missing argument : -fs not found.\n");
		all = 0;
	}
	if(sz == 0){
		printf("Missing argument : -s not found.\n");
		all = 0;
	}
	return all;
}

void create_volume(int fc, int fs, int size){
	struct volume_s vol;
	vol.start_cyl = fc;
	vol.start_sec = fs;
	vol.nsector = size;
	vol.type = BASE;
	mbr->volume[mbr->nvol] = vol;
	mbr->nvol++;
	save_mbr();
	printf("Volume créé de taille %d.\n", size);
}

int main(int argc, char **argv){
	int opt;
	extern char *optarg;
	int fc =      0;          /* First cylinder */
	int fs =      0;          /* First sector */
	int size =    0;          /* Size */
	int fs_flag = 0;
	int fc_flag = 0;
	int sz_flag = 0;

	static struct option long_opt[] =  {
		{"fc", required_argument, 0, 'a'},
		{"fs", required_argument, 0, 'b'},
		{0,    0,                 0, 0}
	};

	if(argc == 1)
		usage();

	while((opt = getopt_long_only(argc, argv, "hs:", long_opt, 0)) != EOF){
		switch(opt){
		case 'a':          /* first cylinder */
			fc = atoi(optarg);
			fc_flag = 1;
			break;
		case 'b':          /* first sector */
			fs = atoi(optarg);
			fs_flag = 1;
			break;
		case 's':
			size = atoi(optarg);
			sz_flag = 1;
			break;
		case 'h':          /* size */
			usage();
			break;
		default:
			fprintf(stderr, "mkvol : option invalide -- '%c'\n", opt);
			fprintf(stderr, "Saisissez « mkvol -h » pour plus d'informations.");
			exit(ERR_ARGT);
		}
	}

	mkhd();

	if(chck_present_flag(fc_flag, fs_flag, sz_flag)){
		fprintf(stderr, "Tous les arguments ne sont pas présent.\n");
		exit(ERR_ARGT);
	}
	mbr = get_mbr();
	if(MAX_VOLUME == mbr->nvol){
		fprintf(stderr, "%d volume, vous ne pouvez pas en créer d'autre.\n", MAX_VOLUME);
		exit(ERR_MX_VOL);
	}
	if(!chck_possible(fc, fs, size)){
		fprintf(stderr, "Vous ne pouvez pas placer un volume aux coordonnées passé en paramêtre.\n");
		exit(ERR_COOR);
	}

	create_volume(fc, fs, size);

	return SUCCESS;
}
