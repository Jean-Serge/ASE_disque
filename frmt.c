#include "include/hardware.h"
#include "drive.h"

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

int main(){
	mkhd();
	frmt();
	return 0;
}
