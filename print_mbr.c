#include <stdlib.h>
#include <stdio.h>
#include "volume.h"
#include "drive.h"

void print_mbr(struct mbr_s *mbr){
	int i;

	printf("MBR MAGIC = %x\n", mbr->magic);
	printf("MBR nb volume = %x\n", mbr->nvol);
	for(i = 0; i < mbr->nvol; i++){
		struct volume_s *vol = mbr->volume+i;
		printf("Affichage du volume %d :\n", i);
		printf("Start cyl : %d\nStart sec : %d\nlenght : %d\n", vol->start_cyl,
		       vol->start_sec, vol->nsector);
		printf("Type : ");
		if(vol->type == BASE){
			printf("base\n");
		}
		else{
			if(vol->type == BASE){
				printf("annexe\n");
			}
			else{
				printf("other\n");
			}
		}
	}
}

int main(){
	struct mbr_s *mbr;
	mkhd();
	mbr = get_mbr();
	printf("\n*** MBR lu ***\n\n");
	print_mbr(mbr);
	return 0;
}
