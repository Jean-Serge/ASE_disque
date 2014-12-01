#include "file.h"
#include <stdio.h>

/****************************** Fonctions utiles ******************************/

unsigned int create_ifile(enum file_type_e type){
	struct inode_s *inode;
	int i = create_inode(type);
	read_inode(i, inode);
	printf("inode %p\n", inode);
	printf("table add direct = %p\n", inode->bloc_direct);
	printf("table add direct = %d\n", inode->bloc_direct);
	return i;
}

int open_ifile(struct file_desc_t *fd, unsigned int inumber){
	struct inode_s *inode;
	int i;
	read_inode(inumber, inode);

	if(!fd)
		fd = (struct file_desc_t *)malloc(sizeof(struct file_desc_t));

	fd->inoeud = inumber;
	fd->cursor = 0;
	fd->size   = inode->taille;
	fd->modified = NO;
	fd->file_buffer = (unsigned char *)malloc(sizeof(unsigned char) *
	                                          HDA_SECTORSIZE);
	/* Lecture des bloc d'adressage direct */
	/* for(i = 0; i < NB_BLOCS; i++){ */

	/* } */
	return 0;
}
