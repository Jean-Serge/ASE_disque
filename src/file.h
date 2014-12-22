#ifndef _H_FILE
#define _H_FILE

#include "filesystem.h"
#include "volume.h"
#include <stdio.h>
#include <stdlib.h>

#define READ_EOF -1

/*
  Crée un fichier de type type et retourne sont inœud.
*/
extern unsigned int create_ifile(enum file_type_e type);

/*
  Suppression du fichier correspondant à l'inœud inumber.
  Retourne 0 si le fichier à été supprimé, sinon 1.
*/
extern int delete_ifile(unsigned int inumber);

enum modified_e {YES, NO};

struct file_desc_s{
	unsigned int inoeud;
	unsigned int cursor;
	unsigned int size;
	unsigned char *file_buffer;
	enum modified_e modified;
};

typedef struct file_desc_s file_desc_t;


extern void close_ifile(file_desc_t *fd);
extern void flush_ifile(file_desc_t *fd);
extern void seek_ifile(file_desc_t *fd, int r_offset);  /* relatif */
extern void seek2_ifile(file_desc_t *fd, int a_offset); /* absolu */

extern int readc_ifile(file_desc_t *fd);
extern int writec_ifile(file_desc_t *fd, char c);
/* extern int read_ifile(file_desc_t *fd, void *buf, unsigned int nbyte); */
/* extern int write_ifile(file_desc_t *fd, const void *buf, unsigned int nbyte); */

#endif
