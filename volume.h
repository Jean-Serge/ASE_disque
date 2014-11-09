#ifndef _H_VOLUME
#define _H_VOLUME

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "drive.h"
#include "include/hardware.h"
#include "include/hw_info.h"

/******************************* Gestion du MBR *******************************/
#define MAX_VOLUME 8
#define MBR_MAGIC 0xB0B0

enum vol_type{BASE, ANNEXE, OTHER};

struct volume_s{
	int start_cyl;
	int start_sec;
	int nsector;
	enum vol_type type;
};

struct mbr_s{
	int magic;
	unsigned int nvol;
	struct volume_s volume[MAX_VOLUME];
};

extern struct mbr_s *get_mbr();

extern void init_mbr_s();

extern void save_mbr();

#endif
