#ifndef _H_FILESYSTEM
#define _H_FILESYSTEM

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "volume.h"
#include "drive.h"
#include "include/hardware.h"
#include "include/hw_info.h"

/***************************** Fonctions utiles *******************************/
/**
 * Lit le bloc indiqué du volume.
 * Copie size octets de ce contenu dans buffer
 * Retourne le buffer.
 */
extern unsigned char* read_struct(unsigned int vol, unsigned int nbloc,
                                  unsigned int size);


/**************************  Gestion des superblocs ***************************/
#define SUPER_MAGIC   0xABCD
#define SUPER_SZ_NAME 32


struct superbloc_s{
	int magic;          /* 2 octets */
	int serial;         /* 4 octets : timestamp */
	int inode;          /* 2 octets */
	int nb_free_blc;    /* 2 octets */
	int first_free;     /* 2 octets */
	char *name;         /* 32 octets */
};

struct free_bloc_s{
	int nb_free_blocs;  /* 2 octets */
	int next;           /* 2 octets */
};

extern void init_super(unsigned int vol);

extern int load_super(unsigned int vol);

extern void save_super();

extern unsigned int new_bloc();

extern void free_bloc(unsigned int bloc);


/****************************  Gestion des inodes *****************************/
#define NB_BLOCS 12

enum file_type_e {
  NORMAL,
  REPOSITORY,
  LINK,
  SPECIAL
};

typedef enum {
  FALSE,
  TRUE
} bool_t;

struct inode_s{
  enum file_type_e type;
  int taille;
  int *bloc_direct;
  int bloc_indirect;
  int bloc_double;
};

extern void read_inode(unsigned int inumber, struct inode_s* inode);

extern void write_inode(unsigned int inumber, struct inode_s* inode);

extern unsigned int create_inode(enum file_type_e type);

extern int delete_inode(unsigned int inumber);

extern unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
                                   bool_t do_allocate);

#endif
