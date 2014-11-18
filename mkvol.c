#include "mkvol.h"

static struct mbr_s *mbr = NULL;

void usage(){
	printf("Création d'un disque.\n");
	exit(SUCCESS);
}

void init_boundary(struct vol_boundary_s *boundary, unsigned int nb_vol){
	int i = 0;
	unsigned int cyl;
	unsigned int sec;
	struct volume_s *volumes;
	boundary = (struct vol_boundary_s *)malloc(sizeof(struct vol_boundary_s) *
	            nb_vol);
	volumes = mbr->volume;
	for(; i < nb_vol; i++){
		boundary[i].first_cyl = volumes[i].start_cyl;
		boundary[i].first_sec = volumes[i].start_sec;
		convert_bloc(i, volumes[i].nsector, &cyl, &sec);
		boundary[i].last_cyl = cyl;
		boundary[i].last_sec = sec;
	}
}


int chck_possible(unsigned int fc, unsigned int fs, unsigned int size){
	struct vol_boundary_s *boundary = NULL;
	int i = 0;
	if(!mbr)
		mbr = get_mbr();
	init_boundary(boundary, mbr->nvol);
	for(; i < mbr->nvol; i++){
		if((fc >= boundary[i].first_cyl) && (fs >= boundary[i].first_sec)
		    && (fc <= boundary[i].last_cyl) && (fs <= boundary[i].last_sec)){
			return 0;
		}
	}
	return 1;
}

int chck_present_flag(unsigned int fc, unsigned int fs, unsigned int sz){
	int all = 1;
	if(fc == 0){
		printf("Missing argument : fc not found.\n");
		all = 0;
	}
	if(fs == 0){
		printf("Missing argument : fs not found.\n");
		all = 0;
	}
	if(sz == 0){
		printf("Missing argument : s not found.\n");
		all = 0;
	}
	return all;
}

void create_volume(int fc, int fs, int size){
	struct volume_s vol; /* = (struct volume_s *)malloc(sizeof(struct volume_s)); */
	vol.start_cyl = fc;
	vol.start_sec = fs;
	vol.nsector = size;
	vol.type = BASE;
	mbr->volume[mbr->nvol] = vol;
	save_mbr();
	printf("Volume créé.\n");
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

	if(!chck_present_flag(fc_flag, fs_flag, sz_flag)){
		fprintf(stderr, "Tous les arguments ne sont pas présent.\n");
		exit(ERR_ARGT);
	}
	mbr = get_mbr();
	if(MAX_VOLUME == mbr->nvol){
		fprintf(stderr, "%d volume, vous ne pouvez pas en créer d'autre.\n", MAX_VOLUME);
		exit(ERR_MX_VOL);
	}
	if(mbr->nvol > 0 && !chck_possible(fc, fs, size)){
		exit(ERR_COOR);
	}

	create_volume(fc, fs, size);

	return SUCCESS;
}
