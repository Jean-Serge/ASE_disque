#include "include/hardware.h"
#include "drive.h"
#include "volume.h"

void frmt(){
	int i;
	int j;
	struct disk_info_s *dinfo = get_disk_info();

	for(i = 0; i < dinfo->nb_cyl; i++){
		for(j = 0 ; j < dinfo->nb_sec; j++){
			format_sector(i, j, 1, 0);
		}
	}
}

void write_mbr(){
	struct mbr_s *mbr;
	mbr = get_mbr();
	mbr->magic = MBR_MAGIC;
	mbr->nvol = 0;
	save_mbr();
}

int main(){
	mkhd();
	frmt();
	write_mbr();
	return 0;
}
