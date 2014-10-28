#include "drive.h"

static struct disk_info_s *d_info = NULL;

/*
  Fonction ne faissant rien. Utile pour initialiser les fonctions appelé lors
  des interruptions du matériel.
 */
void empty_fun(){return;}


/*********** Opération d'I/O d'entier de 2octets dans les registres ***********/
/*
  Mets dans le registre addr les bites de poid fort de val et dans le
  registre addr+1 les bite de poids faible de val.
  val : la valeur à mettre dans les registre
  addr : la valeur du premier registre
 */
void _out_16bits(unsigned int val, unsigned int addr){
	/* TODO : Vérifier que la valeur et addr sont stocké sur 16bits*/
	_out(addr, val>>8);
	_out(addr+1,(val << 8) >> 8);
}

/*
  Retourne l'entier codé sur 16bits lu à l'adress addr et addr+1
 */
unsigned int _in_16bits(unsigned int addr){
	/* TODO : Vérifier qu'addr est stocké sur 16bits*/
	unsigned int val_read;
	val_read = _in(addr) << 8;
	val_read += _in(addr+1);
	return val_read;
}

/********************* Gestion des informations du disque *********************/
struct disk_info_s *get_disk_info(){
	if(!d_info)
		set_disk_info();
	return d_info;
}

void set_disk_info(){
	d_info = (struct disk_info_s *)malloc(sizeof(struct disk_info_s));
	_out(HDA_CMDREG, CMD_DSKINFO);
	d_info->nb_cyl = _in_16bits(HDA_DATAREGS);
	d_info->nb_sec = _in_16bits(HDA_DATAREGS+2);
	d_info->sec_size = _in_16bits(HDA_DATAREGS+4);
}


/***************************  Utilitaire multiples  ***************************/
/*
  Retourne 1 si les coordonnées secteur sec et cylindre cyl sont valide.
  Sinon 0.
  Vérifie que sec est compris entre 0 et le nombre de secteur du disque
  et que le nombre de cylindre est compris entre 0 et le nombre de cylindre
  du disque.
  Pour ne pas vérifier une des coordonnées, il suffit de passer une valeur
  inférieur à 0.
 */
int is_correct_coordinates(int sec, int cyl){
	if(!d_info){
		set_disk_info();
	}
	return ((cyl<0)?1:((cyl<d_info->nb_cyl)?1:0))&
	       ((sec<0)?1:((sec<d_info->nb_sec)?1:0));
}

/*
  Créer un disque dur. Appel la fonction init_hardware() de hardware.h.
 */
void mkhd(){
	int i;
	/* Initialisation du matériel */
	if(init_hardware("hardware.ini") == 0){
		printf("Erreur lors de l'initialisation du matériel.\n");
		exit(HW_INIT_ERROR);
	}
	printf("Disque initialisé.\n");

	/* Association de handlers à tous les IRQ */
	for(i = 0; i < NB_HW_IRQ; i++){
		IRQVECTOR[i] = empty_fun;
	}
	printf("Handler initialisé.\n");
	_mask(1);
	return;
}
