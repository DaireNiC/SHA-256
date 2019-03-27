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
  uint64_t nobits = 0;

  FILE* f;
  char c;
  // open file in read mode
  f = fopen(argv[1], "r");
  //deals in bytes -->  1 = 1 byte
  // one byte at a time
  while(!feof(f)){
    nobytes = fread(M.e, 1, 64, f);
    printf("Read %211u\n bytes\n", nobytes);
    //keeps track of all bits read
    nobits = nobits +(nobytes * 8);
    if(nobytes < 56){
      printf("I've found a block with less htan 55 bytes!\n");
      M.e[nobytes] = 0x80; //1 in hex
      while(nobytes < 56){
	//index in the block
        nobytes = nobytes + 1;
	//set all the bytes to 0 
        M.e[nobytes] = 0x00;
      }
     //access using union
      M.s[7] = nobits;
    }
  }
  fclose(f);
// print to check output
  for (int i = 0; i < 64; i++) {
    printf("%x ", M.e[i]);
    printf("\n");
  }

  return 0;
}

