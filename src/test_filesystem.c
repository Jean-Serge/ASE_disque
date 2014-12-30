#include "filesystem.h"

/****************************************************/
/* Fichier de test pour la bibliothéque filesystem. */
/* Pour tester la bibliothéque modifier le main.    */
/****************************************************/

int main (int argv, char *argc[]){

	/* Test de vbloc_of_fbloc : test des tables indirections de deux niveaux */
	int inumber, bloc, i;
	unsigned char *buf = (unsigned char *)malloc(sizeof(char) * HDA_SECTORSIZE);
	mkhd();

	load_super(0);

	for(i = 0; i < HDA_SECTORSIZE; i++)
		buf[i] = 0xFF;

	inumber = create_inode(NORMAL);

	bloc = vbloc_of_fbloc(inumber, 20, TRUE);
	/* printf("nbloc = %d\n", bloc); */
	write_bloc(0, bloc, buf);

	bloc = vbloc_of_fbloc(inumber, 29, TRUE);
	/* printf("nbloc = %d\n", bloc); */
	write_bloc(0, bloc, buf);

	bloc = vbloc_of_fbloc(inumber, 35, TRUE);
	/* printf("nbloc = %d\n", bloc); */
	write_bloc(0, bloc, buf);

	print_inode(inumber);

	return 0;
}
