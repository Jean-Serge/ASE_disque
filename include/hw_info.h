#ifndef _H_HW_INFO
#define _H_HW_INFO

#define HDA_CMDREG      0x3F6    /* registre de commande du disque maitre */
#define HDA_DATAREGS    0x110    /* registres de données (r,r+1,...r+15) */
#define HDA_IRQ         14       /* Interruption du disque */
#define NB_HW_IRQ       16       /* nombre d'iRQ */
#define TIMER_IRQ       2        /* Niveau dinterruption de l'horloge */
#define HDA_SECTORSIZE  256      /* Nombres d'octets constituant un bloc */
#define HDA_MAXCYLINDER 16       /* Nombre de pistes du disque maître */
#define HDA_MAXSECTOR   16       /* Nombre de secteurs du disque maître */

#define xHDA_STPS       2        /* nombre de SYSTICK pour changer de secteur */
#define HDA_STPC        1        /* nombre de SYSTICK pour changer de piste */
#define HDA_PON_DELAY   30       /* nombre de SYSTICK avant amorce du disque */
#define HDA_POFF_DELAY  30       /* nombre de SYSTICK avant arret du disque */

#endif
