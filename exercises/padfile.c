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
enum status {
  READ,
  PAD0,
  PAD1,
  FINISH
};

int main(int argc, char * argv[]) {

  union msgblock M;
  uint64_t nobytes;
  uint64_t nobits = 0;

  // pointer for file
  FILE * f;
//read status
  enum status S = READ;
  // open file in read mode
  f = fopen(argv[1], "r");
  //deals in bytes -->  1 = 1 byte
  // one byte at a time
  // loop until end of file
  while (S == READ) {

    // Read in file 64 bytes at a time
    nobytes = fread(M.e, 1, 64, f);
    printf("Read %2llu bytes\n", nobytes);
    nobits = nobits + (nobytes * 8);

    // if msg block has < 56 byte ---> pad it out with 0s
    if (nobytes < 56) {

      printf("I've found a block with less than 55 bytes!\n");

      //1 in hex (as per spec)
      M.e[nobytes] = 0x80;

      // write 0's for padding
      while (nobytes < 56) {
        nobytes = nobytes + 1;
        // 0 in hex for padding
        M.e[nobytes] = 0x00;
      } // end while

      // write in the number of bits of the original message
      M.s[7] = nobits;
      // completed reading the file
      S = FINISH;
    }

    // if read > 55 but <64 bits
    else if (nobytes < 64) {
      S = PAD0;
      // append 1
      M.e[nobytes] = 0x80;

      // pad with 0's
      while (nobytes < 64) {
        nobytes = nobytes + 1;
        M.e[nobytes] = 0x00;
      } // end while
    }

    //  if finished reading file, no bits left over
    else if (feof(f)) {
      // still need to create msg block with padding
      S = PAD1;
    }
  } // end while

  if (S == PAD0 || S == PAD1) {
    for (int i = 0; i < 56; i++)
      M.e[i] = 0x00;
    M.s[7] = nobits;
  }
  // write 1 to padding
  if (S == PAD1) {
    M.e[0] = 0x80;
  }

  // completed all reading
  fclose(f);

  // test with output
  for (int i = 0; i < 64; i++)
    printf("%x ", M.e[i]);

  printf("\n");

  return 0;
} // end main
