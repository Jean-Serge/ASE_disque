#include "volume.h"

#define ST_MBR_MAGIC   0
#define ST_MBR_NVOL    2
#define ST_MBR_VOL     3
#define LN_MBR_VOL     5

static struct mbr_s *mbr = NULL;
/******************************* Gestion du MBR *******************************/

void init_mbr_s(){
	unsigned char *buffer;
	int i;
	int it_buf;
	struct volume_s *vol;
	buffer = (unsigned char *)malloc(sizeof(unsigned char) * HDA_SECTORSIZE);
	mbr = (struct mbr_s *)malloc(sizeof(struct mbr_s));
	read_sector(0, 0, buffer);

	/* Lecture du magic */
	mbr->magic = (buffer[ST_MBR_MAGIC] << 8) + (buffer[ST_MBR_MAGIC+1]);

	/* Lecture du nombre de volume créer */
	mbr->nvol = buffer[ST_MBR_NVOL];

	/* Lecture des informations sur les volumes */
	it_buf = ST_MBR_VOL;
	for(i = 0; i < mbr->nvol; i++){
		vol = mbr->volume+i;
		vol->start_cyl = buffer[it_buf];
		vol->start_sec = buffer[it_buf+1];
		vol->nsector = (buffer[it_buf+2] << 8) + buffer[it_buf+3];
		switch(buffer[it_buf+4]){
		case 0:
			vol->type = BASE;
			break;
		case 1:
			vol->type = ANNEXE;
			break;
		case 2:
			vol->type = OTHER;
			break;
		}
		it_buf += LN_MBR_VOL;
	}
	i+=1;
	free(buffer);
}

struct mbr_s *get_mbr(){
	if(!mbr)
		init_mbr_s();
	return mbr;
}

void save_mbr(){
	unsigned char *buffer = (unsigned char *)calloc(HDA_SECTORSIZE,
	                                                sizeof(unsigned char));
	int it_buf;
	int i;
	struct volume_s *vol;

	/* Enregistrement du MAGIC */
	buffer[ST_MBR_MAGIC] = mbr->magic>>8;
	buffer[ST_MBR_MAGIC+1] = ((mbr->magic<<8)>>8) & 0xFF;

	/* Enregistrement du nombre de volume */
	buffer[ST_MBR_NVOL] = mbr->nvol;

	/* Enrefistrement des volumes */
	it_buf = ST_MBR_VOL;
	for(i = 0; i < mbr->nvol; i++){
		vol = mbr->volume+i;
		buffer[it_buf] = vol->start_cyl;
		buffer[it_buf+1] = vol->start_sec;
		buffer[it_buf+2] = vol->nsector>>8;
		buffer[it_buf+3] = (vol->nsector) & 0xF;
		if(vol->type == BASE){
			buffer[it_buf+4] = 0;
		}
		else{
			if(vol->type == BASE){
				buffer[it_buf+4] = 1;
			}
			else{
				buffer[it_buf+4] = 2;
			}
		}
		it_buf += LN_MBR_VOL;
	}

	write_sector(0, 0, buffer);

	free(buffer);
}

/***********************  Fonction d'IO sur des blocs *************************/
/*
  Retourne -1 si erreur.
*/
int convert_bloc(unsigned int nvol, unsigned int bloc, unsigned int *cyl,
                 unsigned int *sec){
	/* Vérification de surface des paramêtre */
	struct volume_s vol;
	if(nvol >= MAX_VOLUME || nvol < 0 || bloc < 0 || nvol > mbr->nvol){
		return -1;
	}
	vol = mbr->volume[nvol];
	*cyl = (vol.start_cyl + (vol.start_sec + bloc) + HDA_MAXSECTOR);
	*sec = (vol.start_sec + bloc) % HDA_MAXSECTOR;
	return 0;
}

void read_bloc(unsigned int vol, unsigned int nbloc,
               unsigned char *buffer){
	unsigned int sec, cyl;
	assert(convert_bloc(vol, nbloc, &cyl, &sec) == 0);
	read_sector(cyl, sec, buffer);
	return;
}

void write_bloc(unsigned int vol, unsigned int nbloc,
                const unsigned char *buffer){
	unsigned int sec, cyl;
	assert(convert_bloc(vol, nbloc, &cyl, &sec) == 0);
	write_sector(cyl, sec, buffer);
	return;
}

void format_vol(unsigned int nvol){
	struct volume_s vol;
	int i;
	unsigned char *buffer;
	if(nvol > mbr->nvol){
		return;
	}
	buffer = (unsigned char *)calloc(HDA_SECTORSIZE, sizeof(unsigned char));
	vol = mbr->volume[nvol];
	for(i = 0; i < vol.nsector; i++){
		write_bloc(nvol, i, buffer);
	}
	return;
}
