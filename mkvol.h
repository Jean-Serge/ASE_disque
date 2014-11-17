#ifndef _H_MKVOL_
#define _H_VOLUME

#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "volume.h"

#define SUCCESS  0
#define ERR_COOR 1
#define ERR_ARGT 2
#define ERR_NEG  3

struct vol_boundary_s{
	unsigned int first_cyl;
	unsigned int first_sec;
	unsigned int last_cyl;
	unsigned int last_sec;
};

#endif
