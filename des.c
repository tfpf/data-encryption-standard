#include <inttypes.h>
#include <stdio.h>

#include "mt19937_64.h"

// Initial permutation.
int const IP[64] =
{
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6,
    64, 56, 48, 40, 32, 24, 16,  8,
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7,
};

// Final permutation.
int const FP[64] =
{
    40,  8, 48, 16, 56, 24, 64, 32,
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25,
};

// Expansion function.
int const E[48] =
{
    32,  1,  2,  3,  4,  5,  4,  5,
     6,  7,  8,  9,  8,  9, 10, 11,
    12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21,
    22, 23, 24, 25, 24, 25, 26, 27,
    28, 29, 28, 29, 30, 31, 32,  1,
};

// PBox.
int const P[32] =
{
    16,  7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26,  5, 18, 31, 10,
     2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25,
};

// Permuted choice 1.
int const PC1[56] =
{
    57, 49, 41, 33, 25, 17,  9,  1,
    58, 50, 42, 34, 26, 18, 10,  2,
    59, 51, 43, 35, 27, 19, 11,  3,
    60, 52, 44, 36, 63, 55, 47, 39,
    31, 23, 15,  7, 62, 54, 46, 38,
    30, 22, 14,  6, 61, 53, 45, 37,
    29, 21, 13,  5, 28, 20, 12,  4,
};

// Permuted choice 2.
int const PC2[48] =
{
    14, 17, 11, 24,  1,  5,  3, 28,
    15,  6, 21, 10, 23, 19, 12,  4,
    26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56,
    34, 53, 46, 42, 50, 36, 29, 32,
};

// SBox.
int const unsigned S[8][64] =
{
    {
        14,  0,  4, 15, 13,  7,  1,  4,  2, 14, 15,  2, 11, 13,  8,  1,
         3, 10, 10,  6,  6, 12, 12, 11,  5,  9,  9,  5,  0,  3,  7,  8,
         4, 15,  1, 12, 14,  8,  8,  2, 13,  4,  6,  9,  2,  1, 11,  7,
        15,  5, 12, 11,  9,  3,  7, 14,  3, 10, 10,  0,  5,  6,  0, 13,
    },
    {
        15,  3,  1, 13,  8,  4, 14,  7,  6, 15, 11,  2,  3,  8,  4, 14,
         9, 12,  7,  0,  2,  1, 13, 10, 12,  6,  0,  9,  5, 11, 10,  5,
         0, 13, 14,  8,  7, 10, 11,  1, 10,  3,  4, 15, 13,  4,  1,  2,
         5, 11,  8,  6, 12,  7,  6, 12,  9,  0,  3,  5,  2, 14, 15,  9,
    },
    {
        10, 13,  0,  7,  9,  0, 14,  9,  6,  3,  3,  4, 15,  6,  5, 10,
         1,  2, 13,  8, 12,  5,  7, 14, 11, 12,  4, 11,  2, 15,  8,  1,
        13,  1,  6, 10,  4, 13,  9,  0,  8,  6, 15,  9,  3,  8,  0,  7,
        11,  4,  1, 15,  2, 14, 12,  3,  5, 11, 10,  5, 14,  2,  7, 12,
    },
    {
         7, 13, 13,  8, 14, 11,  3,  5,  0,  6,  6, 15,  9,  0, 10,  3,
         1,  4,  2,  7,  8,  2,  5, 12, 11,  1, 12, 10,  4, 14, 15,  9,
        10,  3,  6, 15,  9,  0,  0,  6, 12, 10, 11,  1,  7, 13, 13,  8,
        15,  9,  1,  4,  3,  5, 14, 11,  5, 12,  2,  7,  8,  2,  4, 14,
    },
    {
         2, 14, 12, 11,  4,  2,  1, 12,  7,  4, 10,  7, 11, 13,  6,  1,
         8,  5,  5,  0,  3, 15, 15, 10, 13,  3,  0,  9, 14,  8,  9,  6,
         4, 11,  2,  8,  1, 12, 11,  7, 10,  1, 13, 14,  7,  2,  8, 13,
        15,  6,  9, 15, 12,  0,  5,  9,  6, 10,  3,  4,  0,  5, 14,  3,
    },
    {
        12, 10,  1, 15, 10,  4, 15,  2,  9,  7,  2, 12,  6,  9,  8,  5,
         0,  6, 13,  1,  3, 13,  4, 14, 14,  0,  7, 11,  5,  3, 11,  8,
         9,  4, 14,  3, 15,  2,  5, 12,  2,  9,  8,  5, 12, 15,  3, 10,
         7, 11,  0, 14,  4,  1, 10,  7,  1,  6, 13,  0, 11,  8,  6, 13,
    },
    {
         4, 13, 11,  0,  2, 11, 14,  7, 15,  4,  0,  9,  8,  1, 13, 10,
         3, 14, 12,  3,  9,  5,  7, 12,  5,  2, 10, 15,  6,  8,  1,  6,
         1,  6,  4, 11, 11, 13, 13,  8, 12,  1,  3,  4,  7, 10, 14,  7,
        10,  9, 15,  5,  6,  0,  8, 15,  0, 14,  5,  2,  9,  3,  2, 12,
    },
    {
        13,  1,  2, 15,  8, 13,  4,  8,  6, 10, 15,  3, 11,  7,  1,  4,
        10, 12,  9,  5,  3,  6, 14, 11,  5,  0,  0, 14, 12,  9,  7,  2,
         7,  2, 11,  1,  4, 14,  1,  7,  9,  4, 12, 10, 14,  8,  2, 13,
         0, 15,  6, 12, 10,  9, 13,  0, 15,  3,  3,  5,  5,  6,  8, 11,
    },
};

// Rotations.
int const R[16] =
{
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1,
};

/******************************************************************************
 * Parse a string into number.
 *
 * @param str String.
 *
 * @return Number represented by at most 16 rightmost characters of the string
 *     if the entire string contains only hexadecimal digits, else 0.
 *****************************************************************************/
uint64_t parse_hexadecimal(char const *str)
{
    uint64_t val = 0;
    while(*str != '\0')
    {
        int unsigned nibble;
        switch(*str)
        {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            nibble = *str - '0';
            break;

            case 'A': case 'a':
            nibble = 10;
            break;

            case 'B': case 'b':
            nibble = 11;
            break;

            case 'C': case 'c':
            nibble = 12;
            break;

            case 'D': case 'd':
            nibble = 13;
            break;

            case 'E': case 'e':
            nibble = 14;
            break;

            case 'F': case 'f':
            nibble = 15;
            break;

            default:
            return 0;
        }
        val = val << 4 | nibble;
        ++str;
    }
    return val;
}

/******************************************************************************
 * Apply a transformation to a number. `txn` indicates the left-to-right order
 * in which the bits of `num` should be arranged. The elements of `txn` are
 * 1-based big endian indices.
 *
 * @param txn Transformation.
 * @param tx_len Number of elements in `txn`.
 * @param num
 * @param num_len Number of canonical bits in `num`. (I use a 64-bit type to
 *     store values of all sizes.)
 *
 * @return Result of the transformation.
 *****************************************************************************/
uint64_t des_transform(int const txn[], int txn_len, uint64_t num, int num_len)
{
    uint64_t val = 0;
    for(int i = 0; i < txn_len; ++i)
    {
        // Convert 1-based big endian to 0-based little endian. Append the bit
        // at that position to the result.
        int pos = num_len - txn[i];
        val = (val << 1) | (num >> pos & 1);
    }
    return val;
}

/******************************************************************************
 * Generate round keys.
 *
 * @param key Secret key.
 * @param subkeys Array to store round keys in.
 *****************************************************************************/
void des_key_schedule(uint64_t key, uint64_t subkeys[16])
{
    key = des_transform(PC1, 56, key, 64);
    uint32_t left = key >> 28;
    uint32_t right = key & 0xFFFFFFFU;
    for(int r = 0; r < 16; ++r)
    {
        left = (left << R[r]) | (left >> (28 - R[r]));
        right = (right << R[r]) | (right >> (28 - R[r]));
        uint64_t lr = (uint64_t)left << 28 | right;
        subkeys[r] = des_transform(PC2, 48, lr, 56);
    }
}

/******************************************************************************
 * Pass a block of data through the rounds of DES. Helper function for
 * encryption and decryption.
 *
 * @param subkeys Round keys.
 * @param state Data to encrypt.
 * @param begin Index of the first round: 0 or 15.
 * @param end Index of the last round: 15 or 0.
 * @param step Direction: 1 or -1.
 *
 * @return Block of data, mutated.
 *****************************************************************************/
uint64_t des_rounds(uint64_t subkeys[16], uint64_t state, int begin, int end, int step)
{
    state = des_transform(IP, 64, state, 64);
    uint32_t left = state >> 32;
    uint32_t right = state;
    for(int i = begin;; i += step)
    {
        uint32_t right_ = right;
        uint64_t sbox_ip = des_transform(E, 48, right, 32) ^ subkeys[i];
        uint32_t sbox_op = 0;
        for(int j = 0, rshift = 42; j < 8; ++j, rshift -= 6)
        {
            sbox_op = sbox_op << 4 | S[j][sbox_ip >> rshift & 0x3FU];
        }
        right = des_transform(P, 32, sbox_op, 32) ^ left;
        left = right_;
        if(i == end)
        {
            break;
        }
    }

    // In the final round, the two halves are not swapped. However, the swap
    // has already occurred in the above loop. Hence, concatenate the halves in
    // reverse.
    state = (uint64_t)right << 32 | left;
    return des_transform(FP, 64, state, 64);
}

/******************************************************************************
 * Encrypt a block of data.
 *
 * @param subkeys Round keys.
 * @param state Data to encrypt.
 *
 * @return Block of data, encrypted.
 *****************************************************************************/
uint64_t des_encrypt(uint64_t subkeys[16], uint64_t state)
{
    return des_rounds(subkeys, state, 0, 15, 1);
}

/******************************************************************************
 * Decrypt a block of data.
 *
 * @param subkeys Round keys.
 * @param state Data to decrypt.
 *
 * @return Block of data, decrypted.
 *****************************************************************************/
uint64_t des_decrypt(uint64_t subkeys[16], uint64_t state)
{
    return des_rounds(subkeys, state, 15, 0, -1);
}

/******************************************************************************
 * Main function.
 *****************************************************************************/
int main(int const argc, char const *argv[])
{
    mt19937_64_seed(0);

    uint64_t key = argc >= 2 ? parse_hexadecimal(argv[1]) : mt19937_64_rand();
    uint64_t plaintext = argc >= 3 ? parse_hexadecimal(argv[2]) : mt19937_64_rand();

    uint64_t subkeys[16];
    des_key_schedule(key, subkeys);
    uint64_t ciphertext = des_encrypt(subkeys, plaintext);
    uint64_t plaintext_ = des_decrypt(subkeys, ciphertext);
    printf("%016" PRIX64 " %016" PRIX64 " %016" PRIX64 " %016" PRIX64 "\n", key, plaintext, ciphertext, plaintext_);
}
