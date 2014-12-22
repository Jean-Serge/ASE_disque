#include "filesystem.h"

int main (int argv, char *argc[]){
	mkhd();
	load_super(0);
	print_super();
	return 0;
}
