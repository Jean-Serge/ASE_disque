#include <stdio.h>

#include "disque.h"
#include "hardware.h"

void empty_it()
{
}

int main(void)
{
  int i;
  init_hardware("hardware.ini");
  for(i = 0 ; i<16 ; i++)
    IRQVECTOR[i] = empty_it;
  dmps(0,4);
  return 0;
}
