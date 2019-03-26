#include <stdio.h>
#include <stdint.h>

// using a union to store var associated in same chunk of memory
// every member var stored in same memory location
// useful for accessing same var in different ways
union msgblock {
  uint8_t e[64];
  uint32_t t[16];
  uint64_t s[8];
};

int main(int argc, char *argv[]){

  union msgblock M;
  
  uint64_t nobytes;

  FILE* f;
  char c;

  // open file in read mode
  f = fopen(argv[1], "r");

  //deals in bytes -->  1 = 1 byte
  // one byte at a time
 // printf("%c\n", fread(&c, 1, 1, f));

  while(!feof(f)){
    nobytes = fread(M.e, 1, 64, f);
    printf("%11u\n", nobytes);

  }

  fclose(f);

  return 0;

}

