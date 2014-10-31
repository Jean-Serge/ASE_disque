#include <stdlib.h>
#include <stdio.h>
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
	struct volume_s *vol;
	mkhd();
	mbr = get_mbr();
	printf("\n*** MBR lu ***\n\n");
	print_mbr(mbr);

	printf("\n*** Modification du MBR ***\n");
	mbr->magic = 0xAAAA;
	mbr->nvol = 2;
	vol = (struct volume_s *)malloc(sizeof(struct volume_s));
	vol->start_cyl = 10;
	vol->start_cyl = 10;
	vol->nsector = 1;
	vol->type = OTHER;
	mbr->volume[1] = *vol;

	printf("*** Écriture du MBR modifié sur le disque ***\n");
	save_mbr();
	printf("*** MBR modifié lu sur le disque ***\n\n");
	init_mbr_s();
	print_mbr(mbr);

	return 0;
}
