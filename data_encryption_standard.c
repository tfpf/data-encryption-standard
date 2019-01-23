#include<math.h>
#include<stdbool.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define FIRSTBIT 0x8000000000000000

// LIST OF GLOBAL VARIABLES

/*
*	permuted choice 1
*	how to obtain left and right halves of the key
*	first 28 numbers form the left half
*	remaining 28 numbers form the right half
*/
int const PC1[56] =
{
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
};

/*
*	how many times to rotate the key at each stage
*	total of 16 stages
*	at stages 1, 2, 9 and 16, the key is rotated once
*	at every other stage, it is rotated twice
*/
int const Rotations[16] =
{
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/*
*	permuted choice 2
*	select 48 bits out of 56 bits to get the subkey
*/
int const PC2[48] = {
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/*
*	the permutation to be performed on the input
*	this is done once at the beginning
*/
int const InitialPermutation[64] =
{
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7
};

/*
*	the permutation performed just before taking output
*	this is done once at the end
*	it is the inverse operation of InitialPermutation
*/
int const FinalPermutation[64] =
{
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25
};

/*
*	expands the number
*	converts a 32-bit number into a 48-bit number
*	this indicates which bits are to be placed where is
*/
int const DesExpansion[48] =
{
	32,  1,  2,  3,  4,  5,  4,  5,
	 6,  7,  8,  9,  8,  9, 10, 11,
	12, 13, 12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 20, 21,
	22, 23, 24, 25, 24, 25, 26, 27,
	28, 29, 28, 29, 30, 31, 32,  1
};

/*
*	performs the substitution step
*	emulates the S-box
*	there are 8 tables, and each table gives the substitution
*/
int const DesSbox[8][4][16] =
{
	{
		{14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
		{ 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
		{ 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
		{15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},
	},
	{
		{15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
		{ 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
		{ 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
		{13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
	},
	{
		{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
		{13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
		{13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
		{ 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
	},
	{
		{ 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
		{13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
		{10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
		{ 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
	},
	{
		{ 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
		{14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
		{ 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
		{11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
	},
	{
		{12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
		{10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
		{ 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
		{ 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},
	},
	{
		{ 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
		{13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
		{ 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
		{ 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
	},
	{
		{13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
		{ 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
		{ 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
		{ 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11},
	},
};

/*
*	performs the permutation step
*	emulates the P-box
*	shuffles a block of 32 bits
*/
int const Pbox[32] =
{
	16,  7, 20, 21, 29, 12, 28, 17,
	 1, 15, 23, 26,  5, 18, 31, 10,
	 2,  8, 24, 14, 32, 27,  3,  9,
	19, 13, 30,  6, 22, 11,  4, 25
};


// LIST OF FUNCTIONS

/*
*	display a 64-bit binary number
*	print the binary representation of above number
*/
void printbits(uint64_t v)
{
	for(int ii = 0; ii < 64; ii++)
	{
		if(((v << ii) & FIRSTBIT) == (uint64_t)0)
		{
			printf("0");
		}
		else
		{
			printf("1");
		}
	}
	printf("\t%20" PRIu64, v);
	printf("\t%16" PRIX64 "\n", v);
}

/*
*	assume that a 64-bit number has MSB at position 0 and LSB at position 63
*	get the 'position_from'th bit of the 64-bit number 'from'
*	place it at 'position_to'th position in 64-bit number stored at 'block'
*	this function will be used in loops to permute 'from'
*	permuted number is stored at 'block'
*/
void addbit(uint64_t *block, uint64_t from, int position_from, int position_to)
{
	if(((from << (position_from)) & FIRSTBIT) != 0)
	{
		*block += (FIRSTBIT >> position_to);
	}
}

/*
*	permute the input 64-bit number
*	this function is used for the initial and final permutation steps
*	the second argument is true if it is the initial permutation
*	and false if it is the final permutation
*/
void Permutation(uint64_t *data, bool initial)
{
	// create a variable to store the permuted number
	uint64_t data_temp;
	data_temp = 0;

	// write each bit of data_temp according to the permuted order
	for(int ii = 0; ii < 64; ii++)
	{
		if(initial)
		{
			addbit(&data_temp, *data, InitialPermutation[ii] - 1, ii);
		}
		else
		{
			addbit(&data_temp, *data, FinalPermutation[ii] - 1, ii);
		}
	}
	*data = data_temp;
}

/*
*	check whether the parity bits of the key are correct
*	although the key is a 64-bit number, only 56 bits are used
*	remaining 8 bits are parity bits
*	64 bits: there are 8 blocks of 8 bits each
*	in each block of 8 bits,
*	the 8th bit (counting from the left) is the parity of remaining 7-bit number
*/
bool key_parity_verify(uint64_t key)
{
	// flag to indicate whether parity is correct
	int parity_bit;
	parity_bit = 0;

	// loop over all bits, checking the parity of 7-bit blocks
	for(int ii = 0; ii < 64; ii++)
	{
		// if it is the parity bit (8th bit)
		if(ii % 8 == 7)
		{
			if(parity_bit == 0)
			{
				/*
				*	the parity of the previous 7-bit block is found to be 0
				*	so, the bit at the current position should be 0
				*/
				if(((key << ii) & FIRSTBIT) != (uint64_t)0)
				{
					printf("parity error at bit #%i\n", ii + 1);
					return false;
				}
			}
			else
			{
				/*
				*	the parity of the previous 7-bit block is found to be 1
				*	so, the bit at the current position should be 1
				*/
				if( ((key << ii) & FIRSTBIT) != FIRSTBIT)
				{
					printf("parity error at bit #%i\n", ii + 1);
					return false;
				}
			}
			parity_bit = 0; // re-initialize parity_bit for next 8-bit block
		}
		/*
		*	this is the part which actually calculates the parity
		*	if you are in this else block, you are traversing non-parity bits
		*/
		else
		{
			if(((key << ii) & FIRSTBIT) == FIRSTBIT)
			{
				/*
				*	1 has been encountered
				*	toggle parity_bit
				*/
				parity_bit = parity_bit == 0 ? 1 : 0;
			}
		}
	}
	return true;
}

/*
*	key scheduling
*	take the current key and the encryption round as input
*	generate the key for the next round using this data
*/
void key_schedule(uint64_t *key, uint64_t *next_key, int round)
{
	// initialize some variables to do the operations
	uint64_t key_left = 0;
	uint64_t key_right = 0;
	uint64_t key_left_temp = 0;
	uint64_t key_right_temp = 0;

	// initialize key stored at next_key
	*next_key = 0;

	/*
	*	there are 16 rounds of encryption
	*	each round uses one key
	*	each key is generated from the key for the previous round
	*	first key is generated randomly in genkey function
	*	first round: PC1, left rotate, PC2
	*	remaining rounds: left rotate, PC2
	*	left rotation is by 1 bit in rounds 1, 2, 9 and 16
	*	and by 2 bits in all other rounds
	*/
	if(round == 0)
	{
		// perform PC1 permutation
		for(int ii = 0; ii < 56; ii++)
		{
			if(ii < 28)
			{
				addbit(&key_left, *key, PC1[ii] - 1, ii);
			}
			else
			{
				addbit(&key_right, *key, PC1[ii] - 1, ii % 28);
			}
		}
		// splitting the key into left and right halves has been taken care of
	}
	else
	{
		// no PC1, just split the key into left and right halves
		for(int ii = 0; ii < 56; ii++)
		{
			if(ii < 28)
			{
				addbit(&key_left, *key, ii, ii);
			}
			else
			{
				addbit(&key_right, *key, ii, ii % 28);
			}
		}
	}

	/*
	*	perform left rotation by the correct number of bits
	*	note that key_left and key_right are of type uint64_t
	*	but they have been used to store 28-bit numbers
	*	which are two halves of the 56-bit key
	*	the more significant 28 bits of key_left and key_right have been used
	*	how so, is clear from the else block immediately above
	*	therefore, rotation requires rotating the more significant 28 bits
	*	the remaining bits are left as zeros
	*	now the following rotation statements should make sense
	*/
	key_left_temp = Rotations[round] == 1 ? FIRSTBIT : 0xC000000000000000;
	key_right_temp = Rotations[round] == 1 ? FIRSTBIT : 0xC000000000000000;
	key_left_temp = (key_left & key_left_temp) >> (28 - Rotations[round]);
	key_right_temp = (key_right & key_right_temp) >> (28 - Rotations[round]);
	key_left_temp += (key_left << Rotations[round]);
	key_right_temp += (key_right << Rotations[round]);

	/*
	*	combine the two halves to get a new 56-bit key, next_key
	*	the two halves, which are 28 bits long each, are simply joined together
	*	of course, again, it is the more significant 56 bits which are used
	*	the rest are zeros, as per the initialization earlier
	*/
	for(int ii = 0; ii < 56; ii++)
	{
		if(ii < 28)
		{
			addbit(next_key, key_left_temp, ii, ii);
		}
		else
		{
			addbit(next_key, key_right_temp, ii % 28, ii);
		}
	}

	/*
	*	perform PC2 permutation
	*	get a 48-bit number (called the subkey) from the 56-bit number
	*	out of the 56 bits, 48 bits in some order are taken
	*/
	*key = 0;
	for(int ii = 0; ii < 48; ii++)
	{
		addbit(key, *next_key, PC2[ii] - 1, ii);
	}
}

/*
*	generate a 64-bit key
*	the key should have correct parity bits
*	like already mentioned, only 56 bits of the key are actually used
*	remaining 8 are parity bits
*/
void genkey(uint64_t *key)
{
	// to get random numbers
	srand(time(NULL));

	/*
	*	see previous function to understand
	*	it works the same way here
	*	this variable calculates the parity of 7-bit blocks
	*	then sets the 8th bit of that block as that parity
	*/
	int parity_bit;
	parity_bit = 0;

	/*
	*	traverse all 64 bits of the key
	*	calculate parity of 7-bit blocks
	*	if encounter the parity bit position, set it equal to parity_bit
	*/
	for(int ii = 0; ii < 64; ii++)
	{
		// if it is the parity bit ...
		if(ii % 8 == 7)
		{
			// ... and parity is 1 ...
			if(parity_bit == 1)
			{
				// ... set the parity bit to 1
				*key += (FIRSTBIT >> ii);
			}
			parity_bit = 0; // re-initialize parity_bit for next byte block
		}
		else
		{
			/*
			*	it is not the parity bit
			*	use a randomly generated bit
			*	if 1 was generated, update parity
			*/
			if(rand() % 2 == 1)
			{
				*key += (FIRSTBIT >> ii);
				parity_bit = parity_bit == 0 ? 1 : 0;
			}
		}
	}

	/*
	*	DES performs encryption in 16 rounds
	*	each round used a different key
	*	those keys are generated from the original key by key scheduling
	*/
	uint64_t a_key[16];
	a_key[0] = *key; // the key generated above
	uint64_t next_key; // this is created by scheduling

	// obtain the remaining 15 keys
	for(int ii = 0; ii < 16; ii++)
	{
		key_schedule(&a_key[ii], &next_key, ii);
		if(ii != 15)
		{
			a_key[ii + 1] = next_key;
		}
	}

	/*
	*	do not use weak keys
	*	a key is considered weak if the scheduling results in repeated keys
	*/
	bool weak;
	weak = false;
	for(int ii = 0; ii < 15; ii++)
	{
		for(int jj = ii + 1; jj < 16; jj++)
		{
			if(a_key[ii] == a_key[jj])
			{
				weak = true;
			}
		}
	}

	// redo if the key is weak
	if(weak)
	{
		genkey(key);
	}
}

/*
*	rounds of DES
*	implements the Fiestel Function
*	then XORs the two halves of the number
*	Fiestel Function does 4 things to the right half of the data:
*	expand 32 bits to 48 bits,
*	XOR with 48-bit subkey (received by this function as 'key'),
*	use S-box to get 32-bit number from 48-bit number, and
*	use P-box to permute above to get final 32-bit number
*	after these, XOR it with 32-bit left half
*	finally, output becomes new right block
*	old right block becomes left block
*/
void rounds(uint64_t *data, uint64_t key)
{
	// initializing variables to perform operations
	uint64_t right_block = 0;
	uint64_t right_block_temp = 0;

	// expand 32-bit right block to 48 bits
	for(int ii = 0; ii < 48; ii++)
	{
		/*
		*	the rightmost 32 bits are to be expanded
		*	hence, use the latter 32 bits in the number stored at 'data'
		*	leave the first 32 bits untouched
		*/
		addbit(&right_block, *data, DesExpansion[ii] + 31, ii);
	}

	// XOR the 48-bit block with 48-bit subkey
	right_block = right_block ^ key;

	// S-box substitution; this is straightforward
	int coordx, coordy;
	uint64_t substitued;
	for(int ii = 0; ii < 8; ii++)
	{
		coordx = ((right_block << 6 * ii) & FIRSTBIT) == FIRSTBIT ? 2 : 0;
		if(((right_block << (6 * ii + 5)) & FIRSTBIT) == FIRSTBIT)
		{
			coordx++;
		}
		coordy = 0;
		for(int jj = 1; jj < 5; jj++)
		{
			if(((right_block << (6 * ii + jj)) & FIRSTBIT) == FIRSTBIT)
			{
				coordy += 2 ^ (4 - jj);
			}
		}
		substitued = DesSbox[ii][coordx][coordy];
		substitued = substitued << (60 - 4 * ii);
		right_block_temp += substitued;
	}

	// right block is ready
	right_block = right_block_temp;

	// P-box permutation; also straightforward
	right_block_temp = 0;
	for(int ii = 0; ii < 32; ii++)
	{
		addbit(&right_block_temp, right_block, Pbox[ii] - 1, ii);
	}
	right_block = right_block_temp;

	// XOR with left block (left 32 bits of number stored at 'data')
	right_block = right_block ^ *data;

	// recombine the two halves with the above number as the new right block
	*data = (*data << 32) + (right_block >> 32);
}

// main function
int main(void)
{
	// get a random key
	uint64_t key;
	key = 0;
	genkey(&key);

	// verify the parity bits of 'key'
	if(!key_parity_verify(key))
	{
		printf("The key you used is malformed.\n");
		return 1;
	}

	// data (the plaintext which is to be encrypted)
	uint64_t data;
	data = 0xFEDCBA9876543210;

	// get 16 subkeys (48-bit keys for each of the 16 rounds)
	uint64_t a_key[16];
	a_key[0] = key;
	uint64_t next_key;
	for(int ii = 0; ii < 16; ii++)
	{
		key_schedule(&a_key[ii], &next_key, ii);
		if(ii != 15)
		{
			a_key[ii + 1] = next_key;
		}
	}

	// display the key and data
	printf("key:\t\t");
	printbits(key);
	printf("data:\t\t");
	printbits(data);

	// default mode to start with
	int encrypt = 1;

	// a default number of times to run the program
	int count = 2;
	while(count--)
	{
		// initial permutation
		Permutation(&data, true);

		// encryption or decryption mode
		if(encrypt)
		{
			// next time, it should decrypt
			encrypt = 0;

			for(int ii = 0; ii < 16; ii++)
			{
				rounds(&data, a_key[ii]);
			}
			printf("ciphertext:\t");
		}
		else
		{
			// next time, it should encrypt
			encrypt = 1;

			// interchange the two halves for dcryption
			data = (data << 32) + (data >> 32);

			// loop run in reverse
			for(int ii = 15; ii >= 0; ii--)
			{
				rounds(&data, a_key[ii]);
			}
			printf("plaintext:\t");

			// interchange blocks again
			data = (data << 32) + (data >> 32);
		}

		// final permutation
		Permutation(&data, false);

		// display encrypted or decrypted
		printbits(data);
	}
}
