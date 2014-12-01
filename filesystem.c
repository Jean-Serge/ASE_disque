#include "filesystem.h"

static struct mbr_s *mbr = NULL;

/***************************** Fonctions utiles *******************************/

/**
 * Lit le bloc indiqué du volume.
 * Copie size octets de ce contenu dans buffer
 * Retourne le buffer.
 */
unsigned char* read_struct(unsigned int vol, unsigned int nbloc,
                           unsigned int size)
{
	unsigned char *buffer = (unsigned char *) malloc(HDA_SECTORSIZE);

	read_bloc(vol, nbloc, buffer);
	buffer = (unsigned char *) realloc(buffer, size);

	return buffer;
}

/**
 * Ecrit le bloc nbloc avec buffer contenant un structure.
 * Complète avec des \0 pour écrire la totalité du bloc.
 */
void write_struct(unsigned int vol, unsigned int nbloc, char* buffer,
                  unsigned int size)
{
	int i;
	char* new_buffer;

	new_buffer = (char *) malloc(size);
	new_buffer = memcpy(new_buffer, buffer, size);
	new_buffer = (char*) realloc(new_buffer, HDA_SECTORSIZE);

	for(i = size ; i < HDA_SECTORSIZE ; i++)
		{
			*(new_buffer+i) = '\0';
		}

	write_bloc(vol, 0, (const unsigned char*)new_buffer);
}

/************************** Gestion des superblocs ****************************/
static int vol_courant;
static struct superbloc_s *super_courant = NULL;

void init_super(unsigned int vol){
	struct superbloc_s super;
	struct volume_s volume;
	struct free_bloc_s free;

	if(!mbr)
		mbr = get_mbr();

	volume = mbr->volume[vol];

	super.magic = SUPER_MAGIC;

	/* TODO à voir pour le n° de série, pour l'instant
	   je prend le n° du volume */
	super.serial = vol;
/**********************************************************************/
/* JS, tu peux corriger la ligne suivante, j'ai mis le 14 à l'arrache */
/**********************************************************************/
	super.nom = (char *)malloc(sizeof(char) * 14);
	super.nom = strcpy(super.nom, "Nom_A_Definir");
	/* On écrit le super dans le 1er bloc */
	super.nb_free_node = volume.nsector-1;
	super.inoeud = volume.start_cyl * HDA_MAXSECTOR + volume.start_sec;
	super.free_node = 1;
	write_struct(vol, 0, (char *) &super, sizeof(struct superbloc_s));

	/* On écrit la structure de bloc libres dans le 2nd bloc */
	free.nb_free_blocs = volume.nsector-1;
	free.next = 0;
	write_struct(vol, 0, (char *) &free, sizeof(struct free_bloc_s));
}

int load_super(unsigned int vol){
	if(!mbr)
		mbr = get_mbr();

	super_courant = (struct superbloc_s *)
	    read_struct(vol,0,sizeof(struct superbloc_s));
	vol_courant = vol;

	return 0;
}

void save_super(){
	if(!mbr)
		mbr = get_mbr();

	write_struct(vol_courant, 0, (char *)&super_courant,
	             sizeof(struct superbloc_s));
}

/* Retourne 0 si aucun bloc n'est libre */
unsigned int new_bloc(){
	/* On récupère le 1er bloc libre du superbloc */
	unsigned int bloc = super_courant->free_node;
	struct free_bloc_s free;

	if(super_courant->nb_free_node == 0){
		return 0;
	}

	if(!mbr)
		mbr = get_mbr();

	/* Cas où le premier bloc libre est le seul de sa série */
	if(free.nb_free_blocs < 2){
		super_courant->free_node = free.next;
		(super_courant->nb_free_node)--;
		return bloc;
	}
	else{
		/* On écrit la structure dans le bloc libre suivant */
		(free.nb_free_blocs)--;
		(super_courant->nb_free_node)--;
		super_courant->free_node = bloc+1;

		write_struct(vol_courant, bloc+1,
		             (char *) &free, sizeof(struct free_bloc_s));
		return bloc;
	}
}

void free_bloc(unsigned int bloc){
	struct free_bloc_s free;

	if(!mbr)
		mbr = get_mbr();

	free.nb_free_blocs = 1;
	free.next = super_courant->free_node;
	super_courant->free_node = bloc;

	write_struct(vol_courant, bloc, (char *) &free, sizeof(struct free_bloc_s));
}


/**************************** Gestion des inodes ******************************/

void read_inode(unsigned int inumber, struct inode_s* inode)
{
	inode = (struct inode_s*) read_struct(vol_courant,
	                                      inumber, sizeof(struct inode_s));
}

void write_inode(unsigned int inumber, struct inode_s* inode)
{
	write_struct(vol_courant, inumber, (char *) inode, sizeof(struct inode_s));
}

unsigned int create_inode(enum file_type_e type)
{
	/* Initialisation de l'inode */
	struct inode_s inode;
	int inumber;

	if(!mbr)
		mbr = get_mbr();

	inode.taille = 0;
	inode.type = type;
	inode.bloc_direct = (int *) calloc(NB_BLOCS, 1);
	inode.bloc_indirect = 0;
	inode.bloc_double = 0;

	inumber = new_bloc();
	write_inode(inumber, &inode);
	return inumber;
}

int delete_inode(unsigned inumber)
{
	int i, j;
	struct inode_s *inode;
	/* TODO A revoir, problème de type (unsigned int requis) */
	unsigned char* buffer_direct = (unsigned char *)malloc(HDA_SECTORSIZE);
	unsigned char* buffer_indirect = (unsigned char *)malloc(HDA_SECTORSIZE);
	unsigned char* tmp = (unsigned char *)malloc(HDA_SECTORSIZE);

	inode = (struct inode_s*) buffer_direct;

	/* Suppression des blocs en référencement direct */
	for(i = 0 ; i < NB_BLOCS ; i++)
		{
			if(inode->bloc_direct[i] == 0)
				return 0;
			free_bloc(inode->bloc_direct[i]);
		}

	/* Suppression des blocs en référencement indirect */
	buffer_direct = read_struct(vol_courant, inode->bloc_indirect,
	                            sizeof(int *));
	for(i = 0 ; i < NB_BLOCS ; i++)
		{
			if(buffer_indirect[i] == 0)
				return 0;
			free_bloc(buffer_direct[i]);
		}

	/* Suppression des blocs en double référencement indirect */
	buffer_indirect = read_struct(vol_courant, inode->bloc_double,
	                              sizeof(int *));
	for(i = 0 ; i < NB_BLOCS ; i++)
		{
			tmp = read_struct(vol_courant, buffer_indirect[i], sizeof(int *));
			for(j = 0 ; j < NB_BLOCS ; j++)
				{
					if(tmp[i] == 0)
						return 0;
					free_bloc(tmp[i]);
				}
		}

	/* Suppression de l'inode */
	free_bloc(inumber);
	return 0;
}

unsigned int allocate(int bloc, bool_t do_allocate)
{
	if(bloc == 0)
		if(do_allocate)
		{
			bloc = new_bloc();
			return bloc;
		}
		else
			return 0;
	else
		return bloc;
}
/**
 * Essaie de lire le fbloc-ième de l'inode.
 * Si le bloc est alloué, sont numero est retourné.
 * Sinon si do_allocate est vrai un nouveau bloc est alloué et retourné
 * Sinon retourne 0.
 */
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
                            bool_t do_allocate)
{
	struct inode_s inode;
	int bloc, *buffer;
	fbloc--; /* Pour travailler sur les indices directement. */
	read_inode(inumber, &inode);

	if(fbloc == 0)
		return 0;

	/* Si le fbloc-ième est référencé directement */
	if(fbloc < NB_BLOCS)
		{
			bloc = inode.bloc_direct[fbloc];
			bloc = allocate(bloc, do_allocate);
			inode.bloc_direct[fbloc] = bloc;
			write_inode(inumber, &inode);
			return bloc;
		}

	/* Si le fbloc-ième est référencé indirectement */
	else if(fbloc < 2 * NB_BLOCS)
		{
			buffer = (int *)malloc(HDA_SECTORSIZE);
			read_bloc(vol_courant, inode.bloc_indirect, (unsigned char*)buffer);
			bloc = buffer[fbloc - NB_BLOCS];
			bloc = allocate(bloc, do_allocate);
			/* On écrit le bloc contenant le tableau et non l'inode */
			write_bloc(vol_courant, inode.bloc_indirect,
			           (unsigned char*)buffer);
			return bloc;
		}
	/* Si le fbloc-ième est doublement référencé indirectement */

	return 0;
}
