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

#endif


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
  int bloc_direct[NB_BLOCS];
  int bloc_indirect[NB_BLOCS];
  int bloc_double[NB_BLOCS];
};
extern void read_inode(unsigned int inumber, struct inode_s* inode);

extern void write_inode(unsigned int inumber, struct inode_s* inode);

extern unsigned int create_inode(enum file_type_e type);

extern int delete_inode(unsigned int inumber);

extern unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc, bool_t do_allocate);
