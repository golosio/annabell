#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "train_virtual.h"

int main()
{
  int pos;
  char command[100];

  pos = 12;

  setup_doors();
  setup_names();
  //setup_train();
  setup_house();

  for(;;) {
    display(pos);
    scanf("%s", command);
    pos = move(pos, command);
  }

  return 0;
}

