#include "filesystem.h"

static struct mbr_s *mbr = NULL;

/********************* Fonction d'I/O sur les structures **********************/
/**
   Écrit la structure free_bloc_s free_blc sur le bloc bloc du volume vol.
 */
/* TESTER */
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
/* TESTER */
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


/************************** Gestion des superblocs ****************************/
static struct superbloc_s *super_courant = NULL;

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

	/* écriture du nombre de blocs libre */
	buf[8] = super_blc->nb_free_blc >>8;
	buf[9] = ((super_blc->nb_free_blc<<8)>>8) & 0xFF;

	/* écrire le premier l'adresse bloc libre */
	buf[10] = super_blc->first_free >>8;
	buf[11] = ((super_blc->first_free<<8)>>8) & 0xFF;

	/* écrire le nom du volume */
	while(super_blc->name[i] != '\0'){
		buf[12+i] = super_blc->name[i];
		i++;
	}
	write_bloc(vol, 0, buf);
}

/**
   Retourne le super bloc du volume passé en paramêtre.
   Stoppe l'éxécution du programme si le magic n'est pas correct.
 */
struct superbloc_s *read_super_bloc(unsigned int vol){
	char *buf;
	struct superbloc_s *super;
	buf = (char *)malloc(sizeof(char) * HDA_SECTORSIZE);
	read_bloc(vol, 0, (unsigned char *)buf);

	super = (struct superbloc_s *)malloc(sizeof(struct superbloc_s));
	/* lecture du magic */
	super->magic = ((buf[0] & 0xFF)<<8) + (buf[1] & 0xFF);
	assert(super->magic == SUPER_MAGIC);

	/* lecture du numéro de série */
	super->serial = (buf[2] << 24) + (buf[3] << 16) + (buf[4] << 8)
		+ (buf[5] & 0xFF);

	/* lecture de l'adresse du prochain inœud */
	super->inode =  + (buf[6] <<8) + (buf[7] & 0xFF);

	/* lecture du nombre de bloc libre */
	super->nb_free_blc = (buf[8] <<8) + (buf[9] & 0xFF);

	/* lecture de l'adresse du premier bloc libre */
	super->first_free = (buf[10] <<8) + (buf[11] & 0xFF);

	/* lecture du nom du volume */
	super->name = (char *)calloc(SUPER_SZ_NAME, sizeof(char));
	super->name = strncpy(super->name, buf+12, SUPER_SZ_NAME);
	return super;
}

/**
   Initialise le superbloc du volume vol.
 */
void init_super(unsigned int vol){
	/* struct superbloc_s *super; */
	struct volume_s volume;
	struct free_bloc_s *free_blc;

	if(!mbr)
		mbr = get_mbr();

	volume = mbr->volume[vol];

	if(!super_courant)
		super_courant = (struct superbloc_s*)malloc(sizeof(struct superbloc_s));

	super_courant->magic = SUPER_MAGIC;

	/* TODO à voir pour le n° de série, pour l'instant
	   je prend le n° du volume */
	super_courant->serial = 0x00112233;
	super_courant->name = (char *)malloc(sizeof(char) * SUPER_SZ_NAME);
	super_courant->name = strncpy(super_courant->name, "no_name_volume",
		                          SUPER_SZ_NAME);
	/* On écrit le super dans le 1er bloc */
	super_courant->nb_free_blc = volume.nsector-1;
	super_courant->inode = volume.start_cyl * HDA_MAXSECTOR + volume.start_sec;
	super_courant->first_free = 1;
	write_super_bloc(vol, super_courant);


	free_blc = (struct free_bloc_s *)malloc(sizeof(struct free_bloc_s));
	free_blc->magic = FREE_MAGIC;
	free_blc->nb_free_blocs = volume.nsector-1;
	free_blc->next = 0;
	write_free_bloc(vol, 1, free_blc);
	free(free_blc);
}

void print_super(){
	if(!super_courant){
		fprintf(stderr, "Le superbloc du volume n'est pas initialisé. Avait vu chargé un superbloc?\n");
		return;
	}
	printf("Super courant :\n");
	printf("magic        : %x\n", super_courant->magic);
	printf("serial       : %x\n", super_courant->serial);
	printf("inode        : %d\n", super_courant->inode);
	printf("nb_free_bloc : %d\n", super_courant->nb_free_blc);
	printf("first_free   : %d\n", super_courant->first_free);
	printf("name         : %s\n", super_courant->name);
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
/* TESTER */
/* XX : Utile? */
void save_super(){
	if(!mbr)
		mbr = get_mbr();

	write_super_bloc(vol_courant, super_courant);
}

/**
   Crée un nouveau bloc sur le volume courant.
   Retourne 0 si aucun bloc n'est libre.
 */
/* TESTER */
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
/* TESTER */
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
/* TESTER */
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
/* TESTER */
void write_inode(unsigned int inumber, struct inode_s* inode){
	unsigned char *buf = (unsigned char *)calloc(HDA_SECTORSIZE,
                                                 sizeof(unsigned char));
	int i;

	/* écriture du magic */
	buf[0] = inode->magic >> 8;
	buf[1] = inode->magic & 0xF;

	/* écriture du type du fichier */
	buf[2] = inode->type;

	/* écriture de la taile du fichier */
	buf[3] = inode->taille >> 8;
	buf[4] = inode->taille & 0xF;

	/* écriture de la table d'adressage direct */
	for(i = 0; i < NB_BLOCS; i++){
		buf[4+1+i*2] = inode->bloc_direct[i] >> 8;
		buf[4+2+i*2] = inode->bloc_direct[i] & 0xF;
	}
	i = 4+2+i*2;

	/* écriture du bloc d'adressage indirect */
	buf[i+1] = inode->bloc_indirect >> 8;
	buf[i+2] = inode->bloc_indirect & 0xF;

	/* écriture du bloc d'adressage double indirect */
	buf[i+3] = inode->bloc_double >> 8;
	buf[i+4] = inode->bloc_double & 0xF;

	write_bloc(vol_courant, inumber, buf);
	free(buf);
}

/* TESTER */
unsigned int create_inode(enum file_type_e type){
	/* Initialisation de l'inode */
	struct inode_s inode;
	int i;
	int inumber;

	if(!mbr)
		mbr = get_mbr();

	inode.taille = 0;
	inode.type = type;
	for(i = 0; i < NB_BLOCS; i++){
		inode.bloc_direct[i] = NULL_BLOC;
	}
	inode.bloc_indirect = NULL_BLOC;
	inode.bloc_double = NULL_BLOC;

	inumber = new_bloc();
	write_inode(inumber, &inode);
	return inumber;
}

/* TESTER */
void free_bloc_array(unsigned char *buffer){
	int i;
	int bloc;
	for(i = 0; i < NB_BLOCS; i += 2){
		bloc = (buffer[i] << 8) + buffer[i+1];
		if(bloc != NULL_BLOC){
			free_bloc((buffer[i] << 8) + buffer[i+1]);
		}
	}
}

/* TESTER */
int delete_inode(unsigned inumber){
	int i, bloc;
	struct inode_s *inode = NULL;
	unsigned char *indirect_lvl1 = NULL;
	unsigned char *indirect_lvl2 = NULL;
	read_inode(inumber, inode);

	/* Suppression des blocs en référencement direct */
	for(i = 0 ; i < NB_BLOCS; i++){
		if(inode->bloc_direct[i] != 0){
			free_bloc(inode->bloc_direct[i]);
		}
	}

	/* Suppression des blocs en référencement indirect */
	if(inode->bloc_indirect != NULL_BLOC){
		read_bloc(vol_courant, inode->bloc_indirect, indirect_lvl1);
		free_bloc_array(indirect_lvl1);
		free(indirect_lvl1);
	}

	/* Suppression des blocs en double référencement indirect */
	if(inode->bloc_double != NULL_BLOC){
		read_bloc(vol_courant, inode->bloc_indirect, indirect_lvl1);
		for(i = 0; i < NB_BLOCS; i=+2){
			bloc = (indirect_lvl1[i] << 8) + indirect_lvl1[i+1];
			if(indirect_lvl1 != NULL_BLOC){
				read_bloc(vol_courant, bloc, indirect_lvl2);
				free_bloc_array(indirect_lvl2);
				free(indirect_lvl2);
			}
		}
	}

	/* Suppression de l'inode */
	free_bloc(inumber);
	free(inode);
	return 0;
}

/* TESTER */
unsigned int allocate(int bloc){
	if(bloc == 0){
		bloc = new_bloc();
		return bloc;
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
/* TESTER */
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
                            bool_t do_allocate){
	struct inode_s inode;
	fbloc--; /* Pour travailler sur les indices directement. */
	read_inode(inumber, &inode);

	/* Si fbloc est négatif et qu'il atteint un bloc inategnable */
	if(fbloc == 0 || fbloc >= inode.taille)
		return NULL_BLOC;

	/* Si le fbloc-ième est référencé directement */
	if(fbloc < NB_BLOCS){
		if(inode.bloc_direct == NULL_BLOC){
			if(do_allocate == TRUE){
				inode.bloc_direct[fbloc] = new_bloc();
				write_inode(inumber, &inode);
			}
			else{
				return NULL_BLOC;
			}
		}
		return inode.bloc_direct[fbloc];
	}

	/* Si le fbloc-ième est référencé indirectement */
	if(fbloc < 2 * NB_BLOCS){
		unsigned char *indirect = (unsigned char *)malloc(sizeof(unsigned char)
		                                                  * HDA_SECTORSIZE);
		int nbloc = NULL_BLOC;
		/* La table d'indirection n'est pas initialisé */
		if(inode.bloc_indirect == NULL_BLOC){
			if(do_allocate){
				inode.bloc_indirect = new_bloc();
				write_inode(inumber, &inode);
			}
			else{
				return NULL_BLOC;
			}
		}
		read_bloc(vol_courant, inode.bloc_indirect, indirect);

		/* Le bloc voulu n'est pas initialisé */
		nbloc = (indirect[2*fbloc-NB_BLOCS-1]<<8) + indirect[2*fbloc-NB_BLOCS];
		if(nbloc == NULL_BLOC){
			if(do_allocate){
				int nw_bloc = new_bloc();
				indirect[2*fbloc-NB_BLOCS-1] = nw_bloc >> 8;
				indirect[2*fbloc-NB_BLOCS] = nw_bloc & 0xF;
				write_bloc(vol_courant, inode.bloc_indirect, indirect);
			}
			else{
				return NULL_BLOC;
			}
			return (indirect[2*fbloc-NB_BLOCS-1]<<8)
			       + indirect[2*fbloc-NB_BLOCS];
		}
	}

	/* Si le fbloc-ième est doublement référencé indirectement */
	if(fbloc < (2 + NB_BLOCS) * NB_BLOCS){
		int idx_lvl1,idx_lvl2;
		unsigned char *table1 = (unsigned char *)malloc(sizeof(unsigned char) *
		                                                HDA_SECTORSIZE);
		unsigned char *table2 = (unsigned char *)malloc(sizeof(unsigned char) *
		                                                HDA_SECTORSIZE);
		/* Si la table d'indirection de niveau 1 n'est pas initialisé */
		if(inode.bloc_double == NULL_BLOC){
			if(do_allocate){
				inode.bloc_double = new_bloc();
				write_inode(inumber, &inode);
			}
			else{
				return NULL_BLOC;
			}
		}

		/*************************************************/
		/* Lecture de la table d'indirection de niveau 1 */
		/*************************************************/

		/* la "case du tableau" à lire pour accéder au bon tableau */
		/* d'indirection de niveau 2 */
		idx_lvl1 = ((fbloc - NB_BLOCS) / NB_BLOCS) - 1;
		read_bloc(vol_courant, inode.bloc_double, table1);
		if((table1[idx_lvl1*2] << 8) + (table1[idx_lvl1*2+1]) == 0){
			if(do_allocate){
				/* On crée la table d'indirection de niveau 1 */
				int nw_bloc = new_bloc();
				table1[idx_lvl1*2] = nw_bloc >> 8;
				table1[idx_lvl1*2+1] = nw_bloc & 0xF;
				write_bloc(vol_courant, inode.bloc_double, table1);
			}
			else{
				return NULL_BLOC;
			}
		}

		/*************************************************/
		/* Lecture de la table d'indirection de niveau 1 */
		/*************************************************/
		idx_lvl2 = (fbloc - NB_BLOCS * 2) - idx_lvl1 * NB_BLOCS;
		read_bloc(vol_courant, table1[idx_lvl1], table2);

		if(((table2[idx_lvl2] << 8) + table2[idx_lvl2]) == NULL_BLOC){
			if(do_allocate){
				/* On crée la table de niveau 2 */
				int nw_bloc = new_bloc();
				int blc = (table1[idx_lvl1*2] << 8) + table1[idx_lvl1*2+1];
				table2[idx_lvl2*2] = nw_bloc >> 8;
				table2[idx_lvl2*2+1] = nw_bloc & 0xF;
				write_bloc(vol_courant, blc, table2);
			}
			else{
				return NULL_BLOC;
			}
		}
		return (table1[idx_lvl1*2] << 8) + table1[idx_lvl1*2+1];
	}
	return NULL_BLOC;
}
