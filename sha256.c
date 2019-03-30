// Daire Ni Chathain
// The secure hash algorithm, 256 bit version
// https://csrc.nist.ggov/csrc/media/publications/fips/180/4/final/documents/fips180-4-draft-aug2014.pdf

// input/output header file
#include <stdio.h>
  // For using fixed bit length integers
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

//  section 3.2 for definitions
static inline uint32_t rotr(uint32_t n, uint32_t x);
static uint32_t  shr(uint32_t n, uint32_t x);

//  section 4.1.2 for definitions
static uint32_t SIG0(uint32_t x);
static uint32_t SIG1(uint32_t x);

//  section 4.1.2 for definitions
static uint32_t Ch(uint32_t x, uint32_t y, uint32_t z);
static uint32_t Maj(uint32_t x, uint32_t y, uint32_t z);

//calculates hash of a file
void sha256(FILE *f);

// see sections 4.1.2 for definitions
static uint32_t sig0(uint32_t x);
static uint32_t sig1(uint32_t x);

// K constants --> cubed roots
// defined in section 4.2.2
uint32_t K[] = {
  0x428a2f98,
  0x71374491,
  0xb5c0fbcf,
  0xe9b5dba5,
  0x3956c25b,
  0x59f111f1,
  0x923f82a4,
  0xab1c5ed5,
  0xd807aa98,
  0x12835b01,
  0x243185be,
  0x550c7dc3,
  0x72be5d74,
  0x80deb1fe,
  0x9bdc06a7,
  0xc19bf174,
  0xe49b69c1,
  0xefbe4786,
  0x0fc19dc6,
  0x240ca1cc,
  0x2de92c6f,
  0x4a7484aa,
  0x5cb0a9dc,
  0x76f988da,
  0x983e5152,
  0xa831c66d,
  0xb00327c8,
  0xbf597fc7,
  0xc6e00bf3,
  0xd5a79147,
  0x06ca6351,
  0x14292967,
  0x27b70a85,
  0x2e1b2138,
  0x4d2c6dfc,
  0x53380d13,
  0x650a7354,
  0x766a0abb,
  0x81c2c92e,
  0x92722c85,
  0xa2bfe8a1,
  0xa81a664b,
  0xc24b8b70,
  0xc76c51a3,
  0xd192e819,
  0xd6990624,
  0xf40e3585,
  0x106aa070,
  0x19a4c116,
  0x1e376c08,
  0x2748774c,
  0x34b0bcb5,
  0x391c0cb3,
  0x4ed8aa4a,
  0x5b9cca4f,
  0x682e6ff3,
  0x748f82ee,
  0x78a5636f,
  0x84c87814,
  0x8cc70208,
  0x90befffa,
  0xa4506ceb,
  0xbef9a3f7,
  0xc67178f2
};

// Retrieves the next message msgblock
int nextmsgblock(FILE *msgf, union msgblock *M, enum status *S, uint64_t *nobits);

// Start of app
int main(int argc, char *argv[]) {

  // pointer for file
  FILE *msgf;

  // open file in read mod// ADD ERROR CHECK
  msgf = fopen(argv[1], "r");
  //call the hash algorithim
  sha256(msgf);
  // completed all reading
  fclose(msgf);

} // end main

void sha256(FILE *msgf) {

  // the current msg blcok
  union msgblock M;
  // the number of bits read from the file
  uint64_t nobits = 0;
  // flag for state of msg block padding
  enum status S = READ;
  //the  message schedule (section 6.2)
  uint32_t W[64];

  // the working variables (section 6.2)
  uint32_t a, b, c, d, e, f, g, h;

  // two temporary vaiables (section 6.2)
  uint32_t T1, T2;

  // the hash values (section 6.2)
  // the values come from (section 5.3.3)
  // unit guarantees 32 bits
  // 0X for hex literals
  //malloc in future
  uint32_t H[8] = {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19,
  };

  // for looping
  int i, t;

  // loop through message block page 22
  while (nextmsgblock(msgf, &M, &S, &nobits)) {
    // from page 22, W[t] = M[t] for 0 <= t <=15
    for (t = 0; t < 16; t++) {
      W[t] = M.t[t];
    }

    // from page 22, W[t] = ...
    for (t = 16; t < 64; t++) {
      W[t] = sig1(W[t - 2]) + W[t - 7] + sig0(W[t - 15]) + W[t - 16];
    }

    // initialise a,b,c,d...h as per step 2 page 22
    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];
    e = H[4];
    f = H[5];
    g = H[6];
    h = H[7];

    // Step 3.
    // creating new values for working variables
    for (t = 0; t < 64; t++) {

      // Do the working variables operations as per NIST.
      T1 = h + SIG1(e) + Ch(e, f, g) + K[t] + W[t];
      T2 = SIG0(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1; // Makes sure that we are still using 32 bits.
      d = c;
      c = b;
      b = a;
      a = T1 + T2; // Makes sure that we are still using 32 bits.

    } // end for

    // Step 4.
    // Add up all the working variables to each hash and make sure we are still
    // working with solely 32 bit variables.
    H[0] = a + H[0];
    H[1] = b + H[1];
    H[2] = c + H[2];
    H[3] = d + H[3];
    H[4] = e + H[4];
    H[5] = f + H[5];
    H[6] = g + H[6];
    H[7] = h + H[7];

    // testimg

  } // end for

  printf("%08x %x %x %x %x %x  %x %x \n ", H[0], H[1], H[2], H[3], H[4], H[5], H[6], H[7]);
} // end void sha265()

// see sections 3.2 for definitions
// rotate n bits to the right
static inline uint32_t rotr(uint32_t n, uint32_t x) {
  return (x >> n) | (x << (32 - n));
}

// shift right n positions
static uint32_t shr(uint32_t n, uint32_t x) {
  return (x >> n);
}

// see sections 3.2 & 4.1.2 for definitions
static uint32_t sig0(uint32_t x) {
  return (rotr(7, x) ^ rotr(18, x) ^ shr(3, x));
}

static uint32_t sig1(uint32_t x) {
  return (rotr(17, x) ^ rotr(19, x) ^ shr(10, x));
}

static uint32_t SIG0(uint32_t x) {
  return (rotr(2, x) ^ rotr(13, x) ^ rotr(22, x));
}

static uint32_t SIG1(uint32_t x) {
  return (rotr(6, x) ^ rotr(11, x) ^ rotr(25, x));
}

static uint32_t Ch(uint32_t x, uint32_t y, uint32_t z) {
  return ((x & y) ^ ((!x) & z));
}

//majority vote
static uint32_t Maj(uint32_t x, uint32_t y, uint32_t z) {
  return ((x & y) ^ (x & z) ^ (y & z));
}

int nextmsgblock(FILE *msgf, union msgblock *M, enum status *S, uint64_t *nobits) {

  uint64_t nobytes;

  // completed message blocks
  if ( *S == FINISH) {
      return 0;
  }

  // otherwise check if need another block of padding
  if (*S == PAD0 || *S == PAD1) {
    // set first 56 bytes to all 0 bits
    for (int i = 0; i < 56; i++){
      M->e[i] = 0x00;
    }
    //set last 64 bits to int (num of bits in file)--> should be bigEndian
    M->s[7] = *nobits;
    // finished reading
    *S = FINISH;
  
  // wSET FIRST BIT TO 1
    if (*S == PAD1) {
      M->e[0] = 0x80;
    }
    //  keep the loop for another iteration
    return 1;
  }

  //deals in bytes -->  1 = 1 byte
  // one byte at a time
  // loop until end of file

  //at this point we havent finished reading file (S==READ)

  // Read in file 64 bytes at a time
  nobytes = fread(M->e, 1, 64, msgf);

  // keep track num bytes read
  *nobits = *nobits + (nobytes * 8);

  // if msg block has < 56 byte ---> pad it out with 0s
  if (nobytes < 56) {
    //1 in hex (as per spec)
    M->e[nobytes] = 0x80;

    // write 0's for padding until final 64 bits
    while (nobytes < 56) {
      nobytes = nobytes + 1;
      // 0 in hex for padding
      M->e[nobytes] = 0x00;
    } // end while

    // write in the number of bits of the original message as
    M->s[7] = *nobits;
    // completed reading the file
    *S = FINISH;
    // otherwise check if can put some padding in block
  } else if (nobytes < 64) {
    // need another msg blcok with only padding, no 1 bit
    *S = PAD0;
    // append 1
    M->e[nobytes] = 0x80;

    // pad with 0's if file is multiple of 64
    while (nobytes < 64) {
      nobytes = nobytes + 1;
      M->e[nobytes] = 0x00;
    } // end while

  }
  //  if finished reading file, no bits left over
  else if (feof(msgf)) {
    // still need to create msg block with padding
    *S = PAD1;
  }

  // completed all reading
  return 1;
} // end main


