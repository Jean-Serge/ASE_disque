#include "filesystem.h"

static struct mbr_s *mbr = NULL;

/********************* Fonction d'I/O sur les structures **********************/
/**
   Écrit la structure free_bloc_s free_blc sur le bloc bloc du volume vol.
 */
void write_free_bloc(unsigned int vol, unsigned int bloc,
                     struct free_bloc_s *free_blc){
	unsigned char *buf = (unsigned char *)calloc(HDA_SECTORSIZE,
	                                             sizeof(unsigned char));
	/* Écriture du magic */
	buf[0] = free_blc->magic>>8;
	buf[1] = ((free_blc->magic<<8)>>8) & 0xFF;

	/* Écriture du nombre de bloc libre */
	buf[2] = free_blc->nb_free_blocs >> 8;
	buf[3] = ((free_blc->nb_free_blocs<<8)>>8) & 0xFF;

	/* Écriture de l'adresse de la prochaine série de bloc libre */
	buf[4] = free_blc->next >> 8;
	buf[5] = ((free_blc->next<<8)>>8) & 0xFF;
	write_bloc(vol, bloc, buf);
}
/**
   Retourne la structure free_bloc_s du bloc bloc sur le volume vol.
   Stoppe le fonctionnement du programme si le magic n'est pas correct.
 */
struct free_bloc_s *read_free_bloc(unsigned int vol, unsigned int bloc){
	struct free_bloc_s *free_blc;
	unsigned char *buf = (unsigned char *)malloc(sizeof(char) * HDA_SECTORSIZE);
	free_blc = (struct free_bloc_s *)malloc(sizeof(struct free_bloc_s));
	read_bloc(vol, bloc, buf);

	/* lecture du magic */
	free_blc->magic = buf[0] + (buf[1] <<8);
	assert(free_blc->magic == FREE_MAGIC);

	/* lecture du nombre de bloc libres */
	free_blc->nb_free_blocs = buf[2] + (buf[3] <<8);

	/* lecture du prochain de l'adresse du prochain free_bloc_s */
	free_blc->nb_free_blocs = buf[4] + (buf[5] <<8);
	return free_blc;
}

/**
   Écrit la structure superbloc_s sur le bloc 0 du volume vol.
 */
void write_super_bloc(unsigned int vol, struct superbloc_s *super_blc){
	unsigned char *buf = (unsigned char *)calloc(HDA_SECTORSIZE,
	                                             sizeof(unsigned char));
	int i = 0;
	/* écriture du magic */
	buf[0] = super_blc->magic >> 8;
	buf[1] = ((super_blc->magic<<8)>>8) & 0xFF;

	/* écriture du numéro de série */
	buf[2] = super_blc->serial >> 24;
	buf[3] = (super_blc->serial >> 16);
	buf[4] = (super_blc->serial >> 8);
	buf[5] = super_blc->serial;

	/* écriture du prochain inœud */
	buf[6] = super_blc->inode >>8;
	buf[7] = ((super_blc->inode<<8)>>8) & 0xFF;

	/* écriture du nombre d'inode libre */
	buf[8] = super_blc->nb_free_blc >>8;
	buf[9] = ((super_blc->nb_free_blc<<8)>>8) & 0xFF;

	/* écrire le premier l'adresse bloc libre */
	buf[8] = super_blc->first_free >>8;
	buf[9] = ((super_blc->first_free<<8)>>8) & 0xFF;

	/* écrire le nom du volume */
	while(super_blc->name[i] != '\0'){
		buf[10+i] = super_blc->name[i];
		i++;
	}
	write_bloc(vol, 0, buf);
}

/**
   Retourne le super bloc du volume passé en paramêtre.
   Stoppe l'éxécution du programme si le magic n'est pas correct.
 */
struct superbloc_s *read_super_bloc(unsigned int vol){
	struct superbloc_s *super;
	char *buf;
	buf = (char *)malloc(sizeof(char) * HDA_SECTORSIZE);
	read_bloc(vol, 0, (unsigned char *)buf);

	super = (struct superbloc_s *)malloc(sizeof(struct superbloc_s));
	/* lecture du magic */
	super->magic = buf[0] + (buf[1] <<8);
	assert(super->magic == SUPER_MAGIC);

	/* lecture du numéro de série */
	super->serial = buf[2] + (buf[3] << 8) + (buf[4] << 16) + (buf[5] << 24);

	/* lecture de l'adresse du prochain inœud */
	super->inode = buf[6] + (buf[7] <<8);

	/* lecture du nombre de bloc libre */
	super->nb_free_blc = buf[8] + (buf[9] <<8);

	/* lecture de l'adresse du premier bloc libre */
	super->nb_free_blc = buf[10] + (buf[11] <<8);

	/* lecture du nom du volume */
	super->name = strncpy(super->name, buf+12, SUPER_SZ_NAME);
	return super;
}

/************************** Gestion des superblocs ****************************/
static int vol_courant;
static struct superbloc_s *super_courant = NULL;

/**
   Initialise le superbloc du volume vol.
 */
void init_super(unsigned int vol){
	struct superbloc_s *super;
	struct volume_s volume;
	struct free_bloc_s *free_blc;

	if(!mbr)
		mbr = get_mbr();

	volume = mbr->volume[vol];

	super = (struct superbloc_s *)malloc(sizeof(struct superbloc_s));
	super->magic = SUPER_MAGIC;

	/* TODO à voir pour le n° de série, pour l'instant
	   je prend le n° du volume */
	super->serial = 0x00112233;
	super->name = (char *)malloc(sizeof(char) * SUPER_SZ_NAME);
	super->name = strncpy(super->name, "no_name_volume", SUPER_SZ_NAME);
	/* On écrit le super dans le 1er bloc */
	super->nb_free_blc = volume.nsector-1;
	super->inode = volume.start_cyl * HDA_MAXSECTOR + volume.start_sec;
	super->first_free = 1;
	write_super_bloc(vol, super);


	free_blc = (struct free_bloc_s *)malloc(sizeof(struct free_bloc_s));
	free_blc->magic = FREE_MAGIC;
	free_blc->nb_free_blocs = volume.nsector-1;
	free_blc->next = 0;
	write_free_bloc(vol, 1, free_blc);
	free(free_blc);
	free(super);
}

/**
   Charge le superbloc du volume passé en paramêtre.
   Attention, cette fonction remplace le superbloc du volume courant,
   le volume courant change.
*/
int load_super(unsigned int vol){
	if(!mbr)
		mbr = get_mbr();

	if(vol >= mbr->nvol){
		return 1;
	}

	super_courant = read_super_bloc(vol);
	return 0;
}

/**
   Enregistre le superbloc courant sur le disque.
 */
void save_super(){
	if(!mbr)
		mbr = get_mbr();

	write_super_bloc(vol_courant, super_courant);
}

/**
   Crée un nouveau bloc sur le volume courant.
   Retourne 0 si aucun bloc n'est libre.
 */
unsigned int new_bloc(){
	/************/
	/* À TESTER */
	/************/
	/* On récupère le 1er bloc libre du superbloc */
	struct free_bloc_s *free_blc;

	if(super_courant->nb_free_blc == 0){
		return 0;
	}

	if(!mbr)
		mbr = get_mbr();

	/* Cas où le premier bloc libre est le seul de sa série */
	free_blc = read_free_bloc(vol_courant, super_courant->first_free);
	if(free_blc->nb_free_blocs == 1){
		super_courant->first_free = free_blc->next;
		return super_courant->first_free;
	}
	else{
		free_blc->nb_free_blocs--;
		super_courant->first_free++;
		return super_courant->first_free-1;
	}
}

/**
   Libére le bloc passé en paramêtre.
 */
void free_bloc(unsigned int bloc){
	struct free_bloc_s *first_free;
	struct free_bloc_s *nw_first_free;
	if(super_courant == NULL){
	 	super_courant = read_super_bloc(vol_courant);
	}

	first_free = read_free_bloc(vol_courant, super_courant->first_free);

	nw_first_free = (struct free_bloc_s *)malloc(sizeof(struct free_bloc_s));
	nw_first_free->magic = FREE_MAGIC;
	nw_first_free->nb_free_blocs = 1;
	nw_first_free->next = first_free->next;

	super_courant->first_free = bloc;
	write_free_bloc(vol_courant, bloc, nw_first_free);
	super_courant->nb_free_blc--;
	write_super_bloc(vol_courant, super_courant);
	free(nw_first_free);
	free(first_free);
}


/**************************** Gestion des inodes ******************************/
void read_inode(unsigned int inumber, struct inode_s* inode){
	unsigned char *buf = (unsigned char *)malloc(sizeof(char) * HDA_SECTORSIZE);
	int tmp;
	int i;
	inode = (struct inode_s *)malloc(sizeof(struct inode_s));
	read_bloc(vol_courant, inumber, buf);

	/* lecture du magic */
	inode->magic = buf[0] + (buf[1] <<8);
	assert(inode->magic == INODE_MAGIC);

	/* lecture du type du fichier */
	tmp = buf[2];
	switch(tmp){
	case NORMAL:
		inode->type = NORMAL;
		break;
	case REPOSITORY:
		inode->type = REPOSITORY;
		break;
	case LINK:
		inode->type = LINK;
		break;
	 case SPECIAL:
		inode->type = SPECIAL;
		break;
	default:
		fprintf(stderr, "Le type de fichier n'est pas correct!");
		exit(1);
	}
	/* lecture de la taile du fichier */
	inode->taille = buf[3] + (buf[4] <<8);

	/* lecture de la table d'adressage direct */
	for(i = 0; i < NB_BLOCS; i++){
		inode->bloc_direct[i] = buf[4+1+i*2] + (buf[4+2+i*2]<<8);
	}
	i = 4+2+i*2;

	/* lecture du bloc d'adressage indirect */
	inode->bloc_indirect = buf[i+1] + (buf[i+2]<<8);

	/* lecture du bloc d'adressage double indirect */
	inode->bloc_double = buf[i+3] + (buf[i+4]<<8);

	free(buf);
}

void write_inode(unsigned int inumber, struct inode_s* inode){
	/* TODO */
	/* write_struct(vol_courant, inumber, (char *) inode, sizeof(struct inode_s)); */
}

unsigned int create_inode(enum file_type_e type){
	/* Initialisation de l'inode */
	/* struct inode_s inode; */
	/* int inumber; */

	/* if(!mbr) */
	/* 	mbr = get_mbr(); */

	/* inode.taille = 0; */
	/* inode.type = type; */
	/* inode.bloc_direct = (int *) calloc(NB_BLOCS, 1); */
	/* inode.bloc_indirect = 0; */
	/* inode.bloc_double = 0; */

	/* inumber = new_bloc(); */
	/* write_inode(inumber, &inode); */
	/* return inumber; */
	return 1;
}

int delete_inode(unsigned inumber){
	unsigned char *buf = calloc(HDA_SECTORSIZE, sizeof(unsigned char));
}

unsigned int allocate(int bloc, bool_t do_allocate){
	if(bloc == 0){
		if(do_allocate){
			bloc = new_bloc();
			return bloc;
		}
		else{
			return 0;
		}
	}
	else{
		return bloc;
	}
}

/**
 * Essaie de lire le fbloc-ième de l'inode.
 * Si le bloc est alloué, sont numero est retourné.
 * Sinon si do_allocate est vrai un nouveau bloc est alloué et retourné
 * Sinon retourne 0.
 */
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
                            bool_t do_allocate){
	struct inode_s inode;
	int bloc, *buffer;
	fbloc--; /* Pour travailler sur les indices directement. */
	read_inode(inumber, &inode);

	if(fbloc == 0)
		return 0;

	/* Si le fbloc-ième est référencé directement */
	if(fbloc < NB_BLOCS){
			bloc = inode.bloc_direct[fbloc];
			bloc = allocate(bloc, do_allocate);
			inode.bloc_direct[fbloc] = bloc;
			write_inode(inumber, &inode);
			return bloc;
	}
	/* Si le fbloc-ième est référencé indirectement */
	else{
		if(fbloc < 2 * NB_BLOCS){
			buffer = (int *)malloc(HDA_SECTORSIZE);
			read_bloc(vol_courant, inode.bloc_indirect, (unsigned char*)buffer);
			bloc = buffer[fbloc - NB_BLOCS];
			bloc = allocate(bloc, do_allocate);
			/* On écrit le bloc contenant le tableau et non l'inode */
			write_bloc(vol_courant, inode.bloc_indirect,
			           (unsigned char*)buffer);
			return bloc;
		}
	}

	/* Si le fbloc-ième est doublement référencé indirectement */

	return 0;
}
