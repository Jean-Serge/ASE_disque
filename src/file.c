#include "file.h"


/****************************** Fonctions utiles ******************************/

unsigned int create_ifile(enum file_type_e type){
	struct inode_s *inode = NULL;
	int i = create_inode(type);
	read_inode(i, inode);
	printf("inode %p\n", inode);
	for(i = 0; i < NB_BLOCS; i++){
		printf("table direct %d : @bloc = %d\n", i, inode->bloc_direct[i]);
	}
	return i;
}

int open_ifile(file_desc_t *fd, unsigned int inumber){
	struct inode_s *inode = NULL;
	int frst_blc = vbloc_of_fbloc(inumber, 0 , FALSE);
	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char)
	                                             * HDA_SECTORSIZE);
	read_inode(inumber, inode);

	if(!fd)
		fd = (file_desc_t *)malloc(sizeof(file_desc_t));

	fd->inoeud = inumber;
	fd->cursor = 0;
	fd->size   = inode->taille;
	fd->modified = NO;
	fd->file_buffer = (unsigned char *)
	                  calloc(sizeof(unsigned char),  HDA_SECTORSIZE);
	/* Lecture du premier bloc d'adressage direct */
	if(frst_blc != NULL_BLOC){
		read_bloc(vol_courant, frst_blc, buf);
		memcpy(fd->file_buffer, buf, HDA_SECTORSIZE);
	}
	free(buf);
	return 0;
}

void flush_ifile(file_desc_t *fd){
	if(fd->modified == YES){
		int i;
		unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char *) *
		                                             HDA_SECTORSIZE);
		for(i = 0; i < fd->size; i++){
			memcpy(buf, fd->file_buffer+i, HDA_SECTORSIZE);
			write_bloc(vol_courant, vbloc_of_fbloc(fd->inoeud, i , FALSE), buf);
		}
	}
}

/**
   Déplacement relatif. Déplacement du curseur de offset octets
 */
void seek_ifile(file_desc_t *fd, int r_offset){
	int nw_pos = fd->cursor * HDA_SECTORSIZE + r_offset;
	int nxt_blc, fbloc;

	if(nw_pos / HDA_SECTORSIZE != fd->cursor / HDA_SECTORSIZE){
		flush_ifile(fd);

		fbloc = nw_pos / HDA_SECTORSIZE;
		nxt_blc = vbloc_of_fbloc(fd->inoeud, fbloc, FALSE);

		if(nxt_blc != NULL_BLOC){
			read_bloc(vol_courant, nxt_blc, fd->file_buffer);
		}
		else{
			fprintf(stderr,
			        "Impossible de crée un nouveau bloc pour le fichier.\n");
			exit(1);
		}
	}
	fd->cursor = nw_pos;
}

/**
   Déplacement absolu. Déplacement du curseur à l'octer offset
 */
void seek2_ifile(file_desc_t *fd, int a_offset){
	seek_ifile(fd, a_offset - fd->cursor * HDA_SECTORSIZE);
}

void close_ifile(file_desc_t *fd){
	flush_ifile(fd);
}

int readc_ifile(file_desc_t *fd){
	int value;
	if(fd->cursor >= fd->size * HDA_SECTORSIZE)
		return READ_EOF;
	value = fd->file_buffer[fd->cursor % HDA_SECTORSIZE];

	seek_ifile(fd, 1);
	return value;
}

void write_ifile(file_desc_t fd, char c){
	fd->file_buffer[fd->cursor] = c;
	seek_ifile(fd, 1);
}
