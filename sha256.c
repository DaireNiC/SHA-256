// Daire Ni Chathain
// The secure hash algorithm, 256 bit version
// https://csrc.nist.ggov/csrc/media/publications/fips/180/4/final/documents/fips180-4-draft-aug2014.pdf

// input/output header file
# include <stdio.h>
// For using fixed bit length integers
# include <stdint.h>


//  section 3.2 for definitions
uint32_t rotr(uint32_t n, uint32_t x);
uint32_t shr(uint32_t n, uint32_t x);

//  section 4.1.2 for definitions
uint32_t SIG0(uint32_t x);
uint32_t SIG1(uint32_t x);

//  section 4.1.2 for definitions
uint32_t Ch(uint32_t x, uint32_t y, uint32_t z);
uint32_t Maj(uint32_t x, uint32_t y, uint32_t z);

void sha256();

// see sections 4.1.2 for definitions
uint32_t sig0(uint32_t x);
uint32_t sig1(uint32_t x);

int main(int argc, char *argv[]){

	//call the hash algorithim  
	sha256();

} // end main

void sha256(){
	//the  message schedule (section 6.2)
	// 
	uint32_t W[64];

	// the working variables (section 6.2)
	uint32_t a,b,c,d,e,f,g,h;

	// two temporary vaiables (section 6.2)
	uint32_t T1, T2;

	// the hash values (section 6.2)
	// the values come from (section 5.3.3)
	// unit guarantees 32 bits 
	// 0X for hex literals
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

	// the current message block
	uint32_t M[16] = {0, 0, 0, 0, 0, 0, 0, 0};

	// for looping
	int i, t;

	// loop through message block page 22
	for(i = 0; i <= 1; i++)
	{
		// from page 22, W[t] = M[t] for 0 <= t <=15
		for (t = 0; t < 16; t++){
			W[t] = M[t];
		}

		// from page 22, W[t] = ...
		for (t = 16; t < 64; t++){
			W[t] = sig1(W[t-2]) + W[t-7] + sig0(W[t-15]) + W[t-16];
		}

		// initialise a,b,c,d...h as per step 2 page 22
		a = H[0]; b = H[1]; c = H[2]; d = H[3]; e = H[4];
		f = H[5]; g = H[6]; h = H[7];

		// Step 3.
		// creating new values for working variables
		for(t = 0; t < 64; t ++){

			// Do the working variables operations as per NIST.
			T1 = h + SIG1(e) + Ch(e,f,g) + K[t] + W[t];
			T2 = SIG0(a) + Maj(a,b,c);
			h = g;
			g = f;
			f = e;
			e = d + T1; // Makes sure that we are still using 32 bits.
			d = c;
			c = b;
			b = a;
			a = T1 + T2; // Makes sure that we are still using 32 bits.

		}// end for

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

	}// end for

}// end void sha265()

// see sections 3.2 for definitions
uint32_t rotr(uint32_t n, uint32_t x){
	return (x >> n) | (x << (32-n));
}

// shift right n positions 
uint32_t shr(uint32_t n, uint32_t x){
	return (x >> n);
}

// see sections 3.2 & 4.1.2 for definitions
uint32_t sig0(uint32_t x){
	return (rotr(7,x) ^ rotr(18,x) ^ shr(3,x));
}

uint32_t sig1(uint32_t x){
	return (rotr(17,x) ^ rotr(19,x) ^ shr(10,x));
}

uint32_t SIG0(uint32_t x){
	return (rotr(2,x) ^ rotr(13,x) ^ rotr(22,x));
}

uint32_t SIG1(uint32_t x){
	return (rotr(6,x) ^ rotr(11,x) ^ rotr(25,x));
}

uint32_t Ch(uint32_t x, uint32_t y, uint32_t z){
	return ((x & y) ^ ((!x) & z));
}

uint32_t Maj(uint32_t x, uint32_t y, uint32_t z){
	return ((x & y) ^ (x & z) ^ (y & z));
}

