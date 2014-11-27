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

/**
 * Ecrit le bloc nbloc avec buffer contenant un structure.
 * Complète avec des \0 pour écrire la totalité du bloc.
 */
extern void write_struct(unsigned int vol, unsigned int nbloc,
                         char* buffer, unsigned int size);


/**************************  Gestion des superblocs ***************************/
#define SUPER_MAGIC 0xD0D0

struct superbloc_s{
	int magic;
	int serial;
	int inoeud;
	int nb_free_node;
	int free_node;
	char *nom;
};

struct free_bloc_s{
	int nb_free_blocs;
	int next;
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
