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


// flags for keeping track of status when reading file
enum status {READ, PAD0, PAD1, FINISH};

int main(int argc, char *argv[]){


  union msgblock M;
  uint64_t nobytes;
  uint64_t nobits = 0;

  // pointer for file
  FILE* f;

  // open file in read mode
  f = fopen(argv[1], "r");
  //deals in bytes -->  1 = 1 byte
  // one byte at a time
  while(!feof(f)){
    // Read in file 64 bytes at a time
    nobytes = fread(M.e, 1, 64, f);

    printf("Read %211u\n bytes\n", nobytes);
    nobits = nobits +(nobytes * 8);

    // if msg block has < 56 byte ---> pad it out with 0s
    if(nobytes < 56){
      printf("I've found a block with less htan 55 bytes!\n");
      M.e[nobytes] = 0x80; //1 in hex (as per spec)
      while(nobytes < 56){
        nobytes = nobytes + 1;
         // 0 in hex for padding
        M.e[nobytes] = 0x00;
      }

      // savea the num of bits in the entire message
      M.s[7] = nobits;
    }
  }
  fclose(f);

  for (int i = 0; i < 64; i++) {
    printf("%x ", M.e[i]);
    print("\n");
  }

  return 0;
}
