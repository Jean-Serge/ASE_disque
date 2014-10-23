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


#endif
