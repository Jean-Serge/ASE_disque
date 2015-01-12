#include "filesystem.h"

static struct mbr_s *mbr = NULL;
static struct superbloc_s *super_courant = NULL;

/************** Fonctions d'I/O pour les structures free_bloc_s ***************/
/**
   Écrit la structure free_bloc_s free_blc sur le bloc bloc du volume vol.
 */
void write_free_bloc(unsigned int vol, unsigned int bloc,
                     struct free_bloc_s *free_blc){
	unsigned char *buf = (unsigned char *)calloc(HDA_SECTORSIZE,
	                                             sizeof(unsigned char));
	/* Écriture du magic */
	buf[0] = free_blc->magic>>8;
	buf[1] = (free_blc->magic) & 0xFF;

	/* Écriture du nombre de bloc libre */
	buf[2] = free_blc->nb_free_blocs >> 8;
	buf[3] = (free_blc->nb_free_blocs) & 0xFF;

	/* Écriture de l'adresse de la prochaine série de bloc libre */
	buf[4] = free_blc->next >> 8;
	buf[5] = (free_blc->next) & 0xFF;
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
	free_blc->magic = ((buf[0] & 0xFF ) << 8) + buf[1];
	assert(free_blc->magic == FREE_MAGIC);

	/* lecture du nombre de bloc libres */
	free_blc->nb_free_blocs = (buf[2] << 8) + buf[3];

	/* lecture du prochain de l'adresse du prochain free_bloc_s */
	free_blc->next = (buf[4] <<8 ) + buf[5];
	return free_blc;
}


/************************** Gestion des superblocs ****************************/
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
void init_super(unsigned int vol, char *name){
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
	if(name == NULL){
		super_courant->name = strncpy(super_courant->name, "no_name_volume",
		                              SUPER_SZ_NAME);
	}
	else{
		strncpy(super_courant->name, name, SUPER_SZ_NAME);
	}
	super_courant->nb_free_blc = volume.nsector-1;
	super_courant->first_free = 1;
	super_courant->inode = 0;
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
		fprintf(stderr, "Le superbloc du volume n'est pas initialisé. Avait vous chargé un superbloc?\n");
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
/* XX : Utile? */
void save_super(){
	if(!mbr)
		mbr = get_mbr();

	write_super_bloc(vol_courant, super_courant);
}

int super_loaded()
{
  return super_courant == NULL;
}

/***************************** Gestion des blocs ******************************/

void clean_bloc(unsigned int bloc){
	unsigned char *buf = (unsigned char*)calloc(sizeof(char), HDA_SECTORSIZE);
	write_bloc(vol_courant, bloc, buf);
	free(buf);
}

/**
   Retourne un nouveau bloc sur le volume courant.
   Retourne 0 si aucun bloc n'est libre.
 */
unsigned int new_bloc(){
	/* On récupère le 1er bloc libre du superbloc */
	struct free_bloc_s *free_blc;

	if(!mbr)
		mbr = get_mbr();

	if(super_courant->nb_free_blc == 0){
		fprintf(stderr, "Pas assez de bloc\n");
		return 0;
	}


	/* Cas où le premier bloc libre est le seul de sa série */
	free_blc = read_free_bloc(vol_courant, super_courant->first_free);
	super_courant->nb_free_blc--;
	if(free_blc->nb_free_blocs == 1){
		super_courant->first_free = free_blc->next;
		save_super();
		return super_courant->first_free;
	}
	else{
		super_courant->first_free++;
		save_super();
		write_free_bloc(vol_courant, super_courant->first_free, free_blc);
		return super_courant->first_free-1;
	}
}

/**
   Libére le bloc passé en paramêtre.
 */
void free_bloc(unsigned int bloc){
	struct free_bloc_s nw_free;
	if(super_courant == NULL){
		super_courant = read_super_bloc(vol_courant);
	}

	nw_free.magic = FREE_MAGIC;
	nw_free.nb_free_blocs = 1;
	nw_free.next = super_courant->first_free;
	super_courant->first_free = bloc;
	super_courant->nb_free_blc++;
	save_super();
	clean_bloc(bloc);
	write_free_bloc(vol_courant, bloc, &nw_free);
}


/**************************** Gestion des inodes ******************************/
void read_inode(unsigned int inumber, struct inode_s* inode){
	unsigned char *buf = (unsigned char *)malloc(sizeof(char) * HDA_SECTORSIZE);
	int tmp;
	int i;
	read_bloc(vol_courant, inumber, buf);

	/* lecture du magic */
	inode->magic = ((buf[0] & 0xFF)<<8) + buf[1];
	assert(inode->magic == INODE_MAGIC);

	/* lecture du type du fichier */
	tmp = buf[2];
	switch(tmp){
	case NORMAL:
		inode->type = NORMAL;
		break;
	case DIRECTORY:
		inode->type = DIRECTORY;
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
	inode->taille = ((buf[3] & 0xFF) << 8)+ buf[4];

	/* lecture de la table d'adressage direct */
	for(i = 0; i < NB_BLOCS; i++){
		inode->bloc_direct[i] = ((buf[4+1+i*2] & 0xFF) << 8) + (buf[4+2+i*2]);
	}

	/* lecture du bloc d'adressage indirect */
	i = 4+2+i*2;
	inode->bloc_indirect = ((buf[i+1] & 0xFF) << 8) + (buf[i+2]);

	/* lecture du bloc d'adressage double indirect */
	inode->bloc_double = ((buf[i+3] & 0xFF) << 8) + buf[i+4];

	free(buf);
}

void write_inode(unsigned int inumber, struct inode_s* inode){
	unsigned char *buf = (unsigned char *)calloc(HDA_SECTORSIZE,
                                                 sizeof(unsigned char));
	int i;

	/* écriture du magic */
	buf[0] = inode->magic >> 8;
	buf[1] = inode->magic & 0xFF;

	/* écriture du type du fichier */
	buf[2] = inode->type;

	/* écriture de la taile du fichier */
	buf[3] = inode->taille >> 8;
	buf[4] = inode->taille & 0xFF;

	/* écriture de la table d'adressage direct */
	for(i = 0; i < NB_BLOCS; i++){
		buf[4+1+i*2] = (inode->bloc_direct[i] >> 8) & 0xFF;
		buf[4+2+i*2] = inode->bloc_direct[i] & 0xFF;
	}
	i = 4+2+i*2;

	/* écriture du bloc d'adressage indirect */
	buf[i+1] = inode->bloc_indirect >> 8;
	buf[i+2] = inode->bloc_indirect & 0xFF;

	/* écriture du bloc d'adressage double indirect */
	buf[i+3] = inode->bloc_double >> 8;
	buf[i+4] = inode->bloc_double & 0xFF;

	write_bloc(vol_courant, inumber, buf);
	free(buf);
}

unsigned int create_inode(enum file_type_e type){
	/* Initialisation de l'inode */
	struct inode_s inode;
	int i;
	int inumber;

	if(!mbr)
		mbr = get_mbr();

	inode.magic = INODE_MAGIC;
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

extern void print_inode(unsigned int inumber){
	struct inode_s inode;
	int i;
	read_inode(inumber, &inode);
	printf("Information sur l'inode %d :\n", inumber);
	printf("\tType                      : ");
	switch(inode.type){
	case NORMAL:
		printf("NORMAL");
		break;
	case DIRECTORY:
		printf("DIRECTORY");
		break;
	case LINK:
		printf("LINK");
		break;
	case SPECIAL:
		printf("SPECIAL");
		break;
	default:
		printf("ERREUR");
	}
	printf("\n\tTaille                    : %d\n", inode.taille);
	printf("\tBloc direct               : {");
	for(i = 0; i < NB_BLOCS; i++){
		printf("%d", inode.bloc_direct[i]);
		if(i != NB_BLOCS -1)
			printf(" ,");
	}
	printf("}\n\tBloc d'indirection simple : %d\n", inode.bloc_indirect);
	printf("\tBloc d'indirection double : %d\n", inode.bloc_double);
}

/* TESTER */
void free_bloc_array(unsigned char *buffer){
	int i;
	int bloc;
	for(i = 0; i < NB_BLOCS; i += 2){
		bloc = ((buffer[i] & 0xFF) << 8) + buffer[i+1];
		if(bloc != NULL_BLOC){
			free_bloc((buffer[i] << 8) + buffer[i+1]);
		}
	}
}

int delete_inode(unsigned int inumber){
	int i, bloc;
	struct inode_s *inode;
	unsigned char *indirect_lvl1 = NULL;
	unsigned char *indirect_lvl2 = NULL;
	inode = (struct inode_s *)malloc(sizeof(struct inode_s));
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
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
                            bool_t do_allocate){
	struct inode_s inode;
	read_inode(inumber, &inode);

	if(!super_courant){
		fprintf(stderr, "Super bloc non initialisé. Avait vous mounté un volume?\n");
		exit(1);
	}

	/* Si le fbloc-ième peut être référencé directement */
	if(fbloc < NB_BLOCS){
		if(inode.bloc_direct[fbloc] == NULL_BLOC){
			if(do_allocate == TRUE){
				inode.bloc_direct[fbloc] = new_bloc();

				/* Mise à jour de la taille de l'inode */
				if(inode.taille <= fbloc)
					inode.taille = fbloc+1;
				write_inode(inumber, &inode);
				clean_bloc(inode.bloc_direct[fbloc]);
			}
			else{
				return NULL_BLOC;
			}
		}
		return inode.bloc_direct[fbloc];
	}



	/* Si le fbloc-ième peut être référencé indirectement */
	if(fbloc < 2 * NB_BLOCS){
		unsigned char *indirect = (unsigned char *)malloc(sizeof(unsigned char)
		                                                  * HDA_SECTORSIZE);
		int nbloc = NULL_BLOC;

		/* Si la table d'indirection n'est pas initialisé */
		if(inode.bloc_indirect == NULL_BLOC){
			if(do_allocate){
				inode.bloc_indirect = new_bloc();
				clean_bloc(inode.bloc_indirect);
				if(inode.taille <= fbloc)
					inode.taille = fbloc+1;
				write_inode(inumber, &inode);
			}
			else{
				return NULL_BLOC;
			}
		}

		read_bloc(vol_courant, inode.bloc_indirect, indirect);

		nbloc  = indirect[(fbloc-NB_BLOCS)*2]<<8;
		nbloc += indirect[(fbloc-NB_BLOCS)*2+1];
		if(nbloc == NULL_BLOC){         /* Le bloc voulu n'est pas initialisé */
			if(do_allocate){
				nbloc = new_bloc();
				indirect[(fbloc-NB_BLOCS)*2]   = nbloc >> 8;
				indirect[(fbloc-NB_BLOCS)*2+1] = nbloc & 0xFF;
				write_bloc(vol_courant, inode.bloc_indirect, indirect);
				clean_bloc(nbloc);
			}
			else{
				return NULL_BLOC;
			};
		}
		return nbloc;
	}


	/* Si le fbloc-ième est doublement référencé indirectement */
	if(fbloc < (2 + NB_BLOCS) * NB_BLOCS){
		int bloc_table1, bloc_table2;
		int idx_lvl1,idx_lvl2;
		unsigned char *table1 = (unsigned char *)malloc(sizeof(unsigned char) *
		                                                HDA_SECTORSIZE);
		unsigned char *table2 = (unsigned char *)malloc(sizeof(unsigned char) *
		                                                HDA_SECTORSIZE);
		/* Si la table d'indirection de niveau 1 n'est pas initialisé */
		if(inode.bloc_double == NULL_BLOC){
			if(do_allocate){
				inode.bloc_double = new_bloc();
				clean_bloc(inode.bloc_double);
				write_inode(inumber, &inode);
			}
			else{
				return NULL_BLOC;
			}
		}

		bloc_table1 = inode.bloc_double;

		/*************************************************/
		/* Lecture de la table d'indirection de niveau 1 */
		/*************************************************/

		/* la "case du tableau" à lire pour accéder au bon tableau */
		/* d'indirection de niveau 2 */
		idx_lvl1 = ((fbloc - NB_BLOCS) / NB_BLOCS) - 1;
		read_bloc(vol_courant, bloc_table1, table1);
		bloc_table2 = (table1[idx_lvl1*2]>>8) + table1[idx_lvl1*2+1];

		if(bloc_table2 == NULL_BLOC){
			if(do_allocate){
				/* On crée la table d'indirection de niveau 1 */
				bloc_table2 = new_bloc();
				clean_bloc(bloc_table2);
				table1[idx_lvl1*2] = bloc_table2 >> 8;
				table1[idx_lvl1*2+1] = bloc_table2 & 0xFF;
				write_bloc(vol_courant, inode.bloc_double, table1);
			}
			else{
				return NULL_BLOC;
			}
		}

		/*************************************************/
		/* Lecture de la table d'indirection de niveau 2 */
		/*************************************************/
		idx_lvl2 = (fbloc - NB_BLOCS * 2) - idx_lvl1 * NB_BLOCS;
		read_bloc(vol_courant, bloc_table2, table2);

		if((table2[idx_lvl2] << 8) + table2[idx_lvl2] == NULL_BLOC){
			if(do_allocate){
				/* On crée la table de niveau 2 */
				int bloc_final = new_bloc();
				clean_bloc(bloc_final);
				if(inode.taille <= fbloc)
					inode.taille = fbloc+1;
				write_inode(inumber, &inode);
				table2[idx_lvl2*2] = bloc_final >> 8;
				table2[idx_lvl2*2+1] = bloc_final & 0xFF;
				write_bloc(vol_courant, bloc_table2, table2);
				clean_bloc(bloc_final);
			}
			else{
				return NULL_BLOC;
			}
		}
		return (table2[idx_lvl2*2] << 8) + table2[idx_lvl2*2+1];
	}
	return NULL_BLOC;
}
