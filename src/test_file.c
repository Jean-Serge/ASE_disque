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
  file_desc_t fd;
  /* const char* s = "Salut c'est moi."; */

  mkhd();
  mount(0);
  printf("Toto");
  i = create_file("/toto", NORMAL);
  printf("create : %d\n", i);fflush(stdout);
  /* printf("open   : %d\n",open_file(&fd, "/toto")); */
  print_fd(&fd);
  /* close_file(&fd); */
  /* printf("3\n"); */

  /* fd = create_file("Bonjour", DIRECTORY); */
  /* writec_file(fd, 'Z'); */
  return EXIT_SUCCESS;
}
