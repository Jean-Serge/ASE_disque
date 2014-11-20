#include "filesystem.h"
#include "drive.h"
#include <stdlib.h>

int main(){
	int c = 0;
	int s = 0;
	mkhd();
	for(c = 0; c < HDA_MAXCYLINDER; c++){
		printf("Cylinder %d.\n", c);
		fflush(stdout);
		for(s = 0; s < HDA_MAXSECTOR; s++){
			printf("%d : ", s);
			if(convert_cyl_sec(0, c, s) == -1){
				printf("\x1b[31mKO\x1b[0m");
			}
			else{
				printf("\x1b[32mOK\x1b[0m");
			}
			printf(" | ");
		}
		printf("\n");
	}
	return 0;
}
