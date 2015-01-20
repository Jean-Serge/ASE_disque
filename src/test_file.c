#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include "filesystem.h"
#include "file.h"
#include "volume.h"

#define ERR_OK            0
#define ERR_ARGT          1
#define ERR_NOT_INT       2
#define ERR_NOT_VOL       3
#define ERR_INCRT_VOL     4
#define ERR_INCRT_INUMBER 5
#define ERR_OPT           6


int main(int argc, char *argv[]){
  int i;
  /* const char* s = "Salut c'est moi."; */
  file_desc_t fd;
  unsigned char buf[9] /* = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'} */;

  mkhd();
  mount(0);
  i = create_file("/toto", DIRECTORY);
  printf("create : %d\n", i);fflush(stdout);
  i = create_file("/toto/maman", NORMAL);
  printf("create : %d\n", i);fflush(stdout);

  open_file(&fd, "/toto/maman");

  /* /\* write_file(&fd, (void *)&buf, 9); *\/ */
  /* read_file(&fd, (void *)&buf, 9); */
  /* printf("%s\n", buf); */
  close_file(&fd);

  return EXIT_SUCCESS;
}
