#include <stdlib.h>
#include <stdio.h>
#include "include/hardware.h"
#include "drive.h"
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define SUCCESS  0

void frmt_sector(unsigned int cyl, unsigned int sec, unsigned char value){
	int i = 0;

	move_head(cyl, sec);

	for(; i < HDA_SECTORSIZE; i++){
		MASTERBUFFER[i] = value;
	}
	_out_16bits(HDA_DATAREGS, 1);
	_out(HDA_CMDREG, CMD_WRITE);
	_sleep(HDA_IRQ);
}


void frmt(){
	int i;
	int j;
	struct disk_info_s *dinfo = get_disk_info();

	for(i = 0; i < dinfo->nb_cyl; i++){
		for(j = 0 ; j < dinfo->nb_sec; j++){
			frmt_sector(i, j, 0);
		}
	}
}

int main(){
	mkhd();
	frmt();
	return 0;
}
