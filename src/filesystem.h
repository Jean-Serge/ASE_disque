#ifndef _H_FILESYSTEM
#define _H_FILESYSTEM

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "volume.h"
#include "drive.h"
#include "../include/hardware.h"
#include "../include/hw_info.h"

/***************************** Fonctions utiles *******************************/
/**
 * Lit le bloc indiqué du volume.
 * Copie size octets de ce contenu dans buffer
 * Retourne le buffer.
 */
extern unsigned char* read_struct(unsigned int vol, unsigned int nbloc,
                                  unsigned int size);
int vol_courant;

/**************************  Gestion des superblocs ***************************/
#define SUPER_MAGIC   0xABCD
#define SUPER_SZ_NAME 32
#define FREE_MAGIC    0xDCBA


struct superbloc_s{
	int magic;          /* 2 octets */
	int serial;         /* 4 octets : timestamp */
	int inode;          /* le premier inode sur le disque : 2 octets */
	int nb_free_blc;    /* 2 octets */
	int first_free;     /* le premier bloc libre sur le disque : 2 octets */
	char *name;         /* 32 octets */
};

struct superbloc_s *super_courant = NULL;

extern void init_super(unsigned int vol, char *name);

extern int load_super(unsigned int vol);

extern void print_super();

extern void save_super();

extern void clear_super();

extern int super_loaded();

/***************************** Gestion des blocs ******************************/
struct free_bloc_s{
	int magic;          /* 2o */
	int nb_free_blocs;  /* 2o : nombre de bloc libre à la suite */
	int next;           /* 2o : prochain free_bloc. 0 si pas d'autre free_blc*/
};

extern unsigned int new_bloc();

extern void free_bloc(unsigned int bloc);

extern void clean_bloc(unsigned int bloc);

/****************************  Gestion des inodes *****************************/
#define NB_BLOCS    10
#define INODE_MAGIC 0xCAFE
#define NULL_BLOC   0

enum file_type_e {
	NORMAL,
	DIRECTORY,
	LINK,
	SPECIAL
};

typedef enum {
	FALSE = 0,
	TRUE  = 1
} bool_t;

struct inode_s{
	int magic;                      /* 2 octets */
	enum file_type_e type;          /* 1 octet */
	int taille;                     /* 2 octets */
	int bloc_direct[NB_BLOCS];      /* NB_BLOCS * 2 octets */
	int bloc_indirect;              /* 2 octets */
	int bloc_double;                /* 2 octets */
};

extern void read_inode(unsigned int inumber, struct inode_s* inode);

extern void write_inode(unsigned int inumber, struct inode_s* inode);

extern unsigned int create_inode(enum file_type_e type);

extern int delete_inode(unsigned int inumber);

/* return the bloc index on the volume of a given bloc index in a
   file.
   Return BLOC_NULL for a bloc full of zeros */
extern unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc);

/* allocate and return a bloc on the volume (in order to write in the
   file).
   This may imply indirect and d_indirect bloc creation.
   Return BLOC_NULL if no allocation was possible. */
extern unsigned int allocate_vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc);

extern void print_inode(unsigned int inumber);

#endif
