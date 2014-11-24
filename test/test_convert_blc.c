#include "../filesystem.h"
#include "../drive.h"
#include <stdlib.h>

int main(){
	int c = 0;
	int s = 0;
	int nvol = 0;
	mkhd();
	for(; nvol < 8; nvol++){
		printf("********************************************************************************\n*                                VOLUME %d                                      *\n********************************************************************************\n", nvol);
		for(c = 0; c < HDA_MAXCYLINDER; c++){
			printf("Cylinder %d.\n", c);
			for(s = 0; s < HDA_MAXSECTOR; s++){
				printf("%d : ", s);
				if(convert_cyl_sec(nvol, c, s) == -1){
					printf("\x1b[31mKO\x1b[0m | ");
				}
				else{
					printf("\x1b[32mOK\x1b[0m | ");
				}
			}
			printf("\n\n");
		}
	}
	return 0;
}
