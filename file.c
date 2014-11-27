#include "file.h"

unsigned int create_ifile(enum file_type_e type){
	return create_inode(type);
}

int open_ifile(struct file_desc_t *fd, unsigned int inumber){
	struct inode_s *inode;
	read_inode(inumber, inode);

	if(!fd)
		fd = (struct file_desc_t *)malloc(sizeof(struct file_desc_t));

	fd->inoeud = inumber;
	fd->cursor = 0;
	fd->size   = inode->taille;
	fd->modified = NO;

}
