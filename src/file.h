#ifndef _H_FILE
#define _H_FILE

#include "filesystem.h"
#include "volume.h"
#include <stdio.h>
#include <stdlib.h>

#define READ_EOF -1

enum modified_e {YES, NO};

struct file_desc_s{
	unsigned int inoeud;
	unsigned int cursor;
	unsigned int size;
	unsigned char *file_buffer;
	enum file_type_e type;
	enum modified_e modified;
	enum modified_e closed;
};

typedef struct file_desc_s file_desc_t;

/*
  Crée un fichier de type type et retourne sont inœud.
*/
extern unsigned int create_ifile(enum file_type_e type);

/*
  Suppression du fichier correspondant à l'inœud inumber.
  Retourne 0 si le fichier à été supprimé, sinon 1.
*/
extern unsigned int delete_ifile(unsigned int inumber);

extern int open_ifile(file_desc_t *fd, unsigned int inumber);

extern void print_fd(file_desc_t *fd);

extern void close_ifile(file_desc_t *fd);

extern void flush_ifile(file_desc_t *fd);

extern void seek_ifile(file_desc_t *fd, int r_offset);  /* relatif */

extern void seek2_ifile(file_desc_t *fd, int a_offset); /* absolu */

extern int readc_ifile(file_desc_t *fd);

extern int writec_ifile(file_desc_t *fd, unsigned char c);

extern int read_ifile(file_desc_t *fd, unsigned char *buf, unsigned int nbyte);

extern int write_ifile(file_desc_t *fd, const unsigned char *buf, unsigned int nbyte);

extern enum file_type_e get_fd_type(file_desc_t *fd);

#endif
