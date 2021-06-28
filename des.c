#include <stdio.h>
#include <stdint.h>

#include "des.h"

// Code base from: https://github.com/tfpf/data-encryption-standard.git
// Fixed bugs.
// Checking correctness with: http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm

#define FIRSTBIT 0x8000000000000000

int power(int x, int y)
{
    int result = 1;
    for (int i = 0; i < y; ++i)
        result *= x;
    return result;
}

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
}

void addbit(uint64_t *block, uint64_t from, int position_from, int position_to)
{
	if(((from << (position_from)) & FIRSTBIT) != 0)
	{
		*block += (FIRSTBIT >> position_to);
	}
}

void Permutation(uint64_t *data, int initial) {
	uint64_t data_temp = 0;

	for(int ii = 0; ii < 64; ii++)
	{
		if(initial == 1)
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

void key_schedule(uint64_t *key, uint64_t *next_key, int round) {
	uint64_t key_left = 0;
	uint64_t key_right = 0;
	uint64_t key_left_temp = 0;
	uint64_t key_right_temp = 0;

	*next_key = 0;
	if(round == 0) {
		for(int ii = 0; ii < 56; ii++) {
			if(ii < 28) {
				addbit(&key_left, *key, PC1[ii] - 1, ii);
			}
			else {
				addbit(&key_right, *key, PC1[ii] - 1, ii % 28);
			}
		}
	}
	else {
		for(int ii = 0; ii < 56; ii++) {
			if(ii < 28) {
				addbit(&key_left, *key, ii, ii);
			}
			else {
				addbit(&key_right, *key, ii, ii % 28);
			}
		}
	}

	key_left_temp = Rotations[round] == 1 ? FIRSTBIT : 0xC000000000000000;
	key_right_temp = Rotations[round] == 1 ? FIRSTBIT : 0xC000000000000000;
	key_left_temp = (key_left & key_left_temp) >> (28 - Rotations[round]);
	key_right_temp = (key_right & key_right_temp) >> (28 - Rotations[round]);
	key_left_temp += (key_left << Rotations[round]);
	key_right_temp += (key_right << Rotations[round]);
	
	for(int ii = 0; ii < 56; ii++){
		if(ii < 28) {
			addbit(next_key, key_left_temp, ii, ii);
		}
		else {
			addbit(next_key, key_right_temp, ii % 28, ii);
		}
	}

	*key = 0;
	for(int ii = 0; ii < 48; ii++) {
		addbit(key, *next_key, PC2[ii] - 1, ii);
	}
}

void rounds(uint64_t *data, uint64_t key)
{
	uint64_t right_block = 0;
	uint64_t right_block_temp = 0;
	for(int ii = 0; ii < 48; ii++) {
		addbit(&right_block, *data, DesExpansion[ii] + 31, ii);
	}
	
	right_block = right_block ^ key;
	int coordx, coordy;
	uint64_t substitued;
	for(int ii = 0; ii < 8; ii++) {
		coordx = ((right_block << 6 * ii) & FIRSTBIT) == FIRSTBIT ? 2 : 0;
		if(((right_block << (6 * ii + 5)) & FIRSTBIT) == FIRSTBIT) {
			coordx++;
		}
		coordy = 0;
		for(int jj = 1; jj < 5; jj++) {
			if(((right_block << (6 * ii + jj)) & FIRSTBIT) == FIRSTBIT) {
				coordy += power(2,4 - jj);
			}
		}
		substitued = DesSbox[ii][coordx][coordy];
		substitued = substitued << (60 - 4 * ii);
		right_block_temp += substitued;
	}
	right_block = right_block_temp;
	right_block_temp = 0;
	for(int ii = 0; ii < 32; ii++) {
		addbit(&right_block_temp, right_block, Pbox[ii] - 1, ii);
	}
	right_block = right_block_temp;
	right_block = right_block ^ *data;
	*data = (*data << 32) + (right_block >> 32);
}

int main(void) {
  uint64_t data = 0x0123456789ABCDEF;
  uint64_t key  = 1383827165325090801;
  
  printf("Plaintext:\t"); printbits(data); printf("\n");
  printf("Key:\t\t"); printbits(key); printf("\n");
  uint64_t a_key[16];
	a_key[0] = key;
	uint64_t next_key;
	for(int ii = 0; ii < 16; ii++) {
		key_schedule(&a_key[ii], &next_key, ii);
		if(ii != 15) {
			a_key[ii + 1] = next_key;
		}
	}

  printf("Roundkeys:\t"); printbits(key); printf("\n");
  for (int i = 0; i < 16; i++) {
    printf("  [%d]:\t\t", i+1); printbits(a_key[i]); 
    if (a_key[i] == correct_key[i]) printf(" OK!");
    printf("\n");
  }
  
  
  // initial permutation
  Permutation(&data, 1);
  printf("After IP:\t"); printbits(data); printf(" OK!\n");
  for(int ii = 0; ii < 16; ii++) {
    rounds(&data, a_key[ii]);
  }
  printf("After 16 rounds:"); printbits(data); printf("\n");
  
  
  uint64_t reverse = 0;
  for(int ii = 0; ii < 32; ii++) {
    addbit(&reverse, data, ii + 32, ii);
  }
  for(int ii = 0; ii < 32; ii++) {
    addbit(&reverse, data, ii, ii + 32);
  }
  data = reverse;
  
  // final permutation
  Permutation(&data, 0);

  // display encrypted or decrypted
  printf("Ciphertext:\t"); printbits(data); printf("\n");
	
  return 0;
}
