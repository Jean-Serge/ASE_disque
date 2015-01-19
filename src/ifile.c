#include "ifile.h"

#define CLOSE_ERROR() fprintf(stderr, "Erreur! Le fichier est fermé.");

int mount(int nvol){
	return load_super(nvol);
}

int umount(){
	if(super_loaded()){
		save_super();
		clear_super();
		return 1;
	}
	return 0;
}

/****************************** Fonctions utiles ******************************/
unsigned int create_ifile(enum file_type_e type){
	int i = create_inode(type);
	return i;
}

unsigned int delete_ifile(unsigned int inumber){
	struct inode_s inode;
	int i = 0;
	read_inode(inumber, &inode);
	for(; i < inode.taille; i++){
		free_bloc(vbloc_of_fbloc(inumber, i));
	}
	free_bloc(inumber);
	return 1;
}

int open_ifile(file_desc_t *fd, unsigned int inumber){
	struct inode_s inode;
	int frst_blc = vbloc_of_fbloc(inumber, 0);
	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char)
	                                             * HDA_SECTORSIZE);
	read_inode(inumber, &inode);

	if(!fd)
		fd = (file_desc_t *)malloc(sizeof(file_desc_t));

	fd->inoeud   = inumber;
	fd->cursor   = 0;
	fd->size     = inode.taille;
	fd->modified = NO;
	fd->closed   = NO;
	fd->type     = inode.type;
	fd->file_buffer = (unsigned char *)
	                  calloc( HDA_SECTORSIZE, sizeof(unsigned char));
	/* Lecture du premier bloc d'adressage direct */
	if(frst_blc != NULL_BLOC){
		read_bloc(vol_courant, frst_blc, buf);
		memcpy(fd->file_buffer, buf, HDA_SECTORSIZE);
	}
	free(buf);
	return 0;
}

extern int iopen_ifile(file_desc_t *fd, unsigned int inumber,
                       struct inode_s *inode){
	unsigned int first_bloc;

	/* réinitialisation du file descriptor */
	fd->inoeud = inumber;
	fd->cursor = 0;
	fd->size = inode->taille;
	fd->modified = NO;
	fd->closed   = NO;
	fd->type     = inode->type;
	fd->file_buffer = (unsigned char *)
	                  calloc( HDA_SECTORSIZE, sizeof(unsigned char));
	
	/* Réinitialisation du tampon du file descriptor */
	first_bloc = vbloc_of_fbloc(inumber, 0);
	if(first_bloc == 0){
		memset(fd->file_buffer, 0, HDA_SECTORSIZE);
	}
	else{
		read_bloc(vol_courant, first_bloc, fd->file_buffer);
	}
	return 1;
}

void print_fd(file_desc_t *fd){
	printf("Présentation du File Descriptor :\n");
	printf("\tInœud     : %d\n", fd->inoeud);
	printf("\tCursor    : %d\n", fd->cursor);
	printf("\tSize      : %d\n", fd->size);
	printf("\tModified  : ");
	switch(fd->modified){
	case YES:
		printf("YES\n");
		break;
	case NO:
		printf("NO\n");
		break;
	default:
		printf("ERREUR\n");
		break;
	}
	printf("\tClosed    : ");
	switch(fd->closed){
	case YES:
		printf("YES\n");
		break;
	case NO:
		printf("NO\n");
		break;
	default:
		printf("ERREUR\n");
		break;
	}
}

int position_to_fbloc(unsigned int position){
	return position / HDA_SECTORSIZE;
}

unsigned int is_closed(file_desc_t *fd){
	return (fd->closed == YES)?1:0;
}

void flush_ifile(file_desc_t *fd){
	if(is_closed(fd)){
		CLOSE_ERROR();
		return;
	}
	if(fd->modified == YES){
		int bloc = position_to_fbloc(fd->cursor);
		unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char *) *
		                                             HDA_SECTORSIZE);
		memcpy(buf, fd->file_buffer, HDA_SECTORSIZE);
		write_bloc(vol_courant, allocate_vbloc_of_fbloc(fd->inoeud, bloc), buf);
	}
}

/**
   Déplacement relatif. Déplacement du curseur de offset octets
 */
void seek_ifile(file_desc_t *fd, int r_offset){
	if(is_closed(fd)){
		CLOSE_ERROR();
		return;
	}
	int nw_pos = fd->cursor + r_offset;
	int nxt_blc, fbloc;

	if(position_to_fbloc(nw_pos) != position_to_fbloc(fd->cursor)){;
		flush_ifile(fd);

		fbloc = nw_pos / HDA_SECTORSIZE;
		nxt_blc = allocate_vbloc_of_fbloc(fd->inoeud, fbloc);

		if(nxt_blc != NULL_BLOC)
			read_bloc(vol_courant, nxt_blc, fd->file_buffer);
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
	if(is_closed(fd)){
		CLOSE_ERROR();
		return;
	}
	seek_ifile(fd, a_offset - fd->cursor );
}

void close_ifile(file_desc_t *fd){
	if(is_closed(fd)){
		CLOSE_ERROR();
		return;
	}
	flush_ifile(fd);
	fd->closed = YES;
}

int readc_ifile(file_desc_t *fd){
	int value;
	if(is_closed(fd)){
		CLOSE_ERROR();
		return READ_EOF;
	}
	if(fd->cursor >= fd->size * HDA_SECTORSIZE){
		return READ_EOF;
	}
	value = fd->file_buffer[fd->cursor % HDA_SECTORSIZE];

	seek_ifile(fd, 1);
	return value;
}

int writec_ifile(file_desc_t *fd, unsigned char c){
	if(is_closed(fd)){
		CLOSE_ERROR();
		return 0;
	}
	fd->file_buffer[fd->cursor % HDA_SECTORSIZE] = c;
	fd->modified = YES;

	seek_ifile(fd, 1);
	return 1;
}

int read_ifile(file_desc_t *fd, void *buf, unsigned int nbyte){
	int i = 0;
	int c;
	char *tmp = (char *)malloc(nbyte);
	for(i = 0; i < nbyte; i++){
		c = readc_ifile(fd);
		if(c == READ_EOF)
			break;                /* fin du fichier, on stoppe */
		tmp[i] = c;
	}
	
	memcpy(buf, tmp, nbyte);
	free(tmp);
	if(c == READ_EOF)
	  return c;
	return i;
}

int write_ifile(file_desc_t *fd, const void *buf, unsigned int nbyte){
	int i = 0;
	char *tmp = (char *)buf;
	for(i = 0; i < nbyte; i++){
		writec_ifile(fd, tmp[i]);
	}
	return i;
}

enum file_type_e get_fd_type(file_desc_t *fd){
	return fd->type;
}
