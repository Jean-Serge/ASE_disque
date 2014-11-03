#ifndef H_DISQUE
#define H_DISQUE

/**********************  Macros pour le hardware **************************/

#define HDA_DATAREGS 0x110
#define HDA_CMDREG 0x3F6

#define HDA_IRQ 14

/**************************************************************************/


/**
 * Affiche le contenu du secteur indiqué à la piste indiquée.
 */
extern void dmps(unsigned int, unsigned int);

/**
 * Formate le disque courrant (met les secteurs à 0).
 */ 
extern void frmt();

/**
 * Ecrit la valeur 16 bit dans le registre indiqué (et le suivant)
 */
extern void ecrire_int(int*, int);

/**
 * Lit un mot de 16 bits au registre indiqué
 */
extern int lire_int(int*);
#endif
