#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "d2slib.h"

int main() {

//  return 0;

  D2S * save = D2Sloadsave("diablo/Diablo.d2s");
  
  printf("%s %d\n", save->header->characterName, save->header->level);

  return 0;  

}