#include "filesystem.h"

static struct mbr_s *mbr = NULL;

/************************** Gestion des superblocs ****************************/
static int vol_courant;
static struct superbloc_s *super_courant = NULL;

void init_super(unsigned int vol){
	struct superbloc_s super;
	struct volume_s volume;
	struct free_bloc_s free;

	volume = mbr->volume[vol];

	super.magic = SUPER_MAGIC;

	/* TODO à voir pour le n° de série, pour l'instant
	   je prend le n° du volume */
	super.serial = vol;
	super.nom = strcpy(super.nom, "Nom_A_Definir");

	/* On écrit le super dans le 1er bloc */
	super.nb_free_node = volume.nsector-1;
	super.inoeud = volume.start_cyl * HDA_MAXSECTOR + volume.start_sec;
	super.free_node = 1;

	write_bloc(vol, 0, (const unsigned char*)&super);

	free.nb_free_blocs = volume.nsector-1;
	free.next = 0;

	write_bloc(vol, 1, (const unsigned char*)&free);
}

int load_super(unsigned int vol){
	char* buffer = (char *)malloc(HDA_SECTORSIZE);

	read_bloc(vol, 0, (unsigned char*)buffer);
	super_courant = (struct superbloc_s*)buffer;
	vol_courant = vol;

	return 0;
}

void save_super(){
	/* On écrit le superbloc courant dans son volume */
	write_bloc(vol_courant, 0, (const unsigned char*)&super_courant);
}

/* Retourne 0 si aucun bloc n'est libre */
unsigned int new_bloc(){
	/* On récupère le 1er bloc libre du superbloc */
	unsigned int bloc = super_courant->free_node;
	struct free_bloc_s free;

	char* buffer = (char*) malloc(HDA_SECTORSIZE);

	if(super_courant->nb_free_node == 0){
		return 0;
	}

	read_sector(vol_courant, bloc, (unsigned char*)buffer);

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
		write_sector(vol_courant, bloc+1, (const unsigned char*)&free);
		return bloc;
	}
}

void free_bloc(unsigned int bloc){
	struct free_bloc_s free;
	free.nb_free_blocs = 1;
	free.next = super_courant->free_node;
	super_courant->free_node = bloc;
	write_sector(vol_courant, bloc, (const unsigned char*) &free);
}
