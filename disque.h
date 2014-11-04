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

/**
 * Lit le contenu du couple cylindre/vecteur indiqués 
 * et le stocke dans le buffer.
 */
extern void read_sector(unsigned int, unsigned int, unsigned char*);

/**
 * Ecrit le contenu du buffer à l'emplacement indiqué
 */ 
extern void write_sector(unsigned int, unsigned int, unsigned char*);
extern void format_sector(unsigned int, unsigned int, unsigned int, unsigned int);


extern void seek(unsigned int, unsigned int);
#endif
