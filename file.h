#ifndef _H_FILE
#define _H_FILE

/*
  Crée un fichier de type type et retourne sont inœud.
*/
extern unsigned int create_ifile(enum file_type_e type);

extern int delete_ifile(unsigned int inumber);

extern int open_ifile(file_desc_t *fd, unsigned int inumber);
extern void close_ifile(file_desc_t *fd);
extern void flush_ifile(file_desc_t *fd);
extern void seek_ifile(file_desc_t *fd, int r_offset);  /* relatif */
extern void seek2_ifile(file_desc_t *fd, int a_offset); /* absolu */

extern int readc_ifile(file_desc_t *fd);
extern int writec_ifile(file_desc_t *fd, char c);
extern int read_ifile(file_desc_t *fd, void *buf, unsigned int nbyte);
extern int write_ifile(file_desc_t *fd, const void *buf, unsigned int nbyte);

#endif
